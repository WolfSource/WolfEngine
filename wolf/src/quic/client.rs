use futures::executor::block_on;
use quinn::{
    ClientConfig, ConnectError, Connection, ConnectionError, Endpoint, NewConnection,
    ReadToEndError, SendStream, WriteError,
};
use std::{fs, io, net::SocketAddr, sync::Arc};

#[derive(Debug)]
/// Different Error variants that may happend on the client
pub enum QuicClientError {
    ConnectErr(ConnectError),
    ConnectionAttemptErr(Vec<QuicClientError>),
    ConnectionErr(ConnectionError),
    OutgoingEndpointCreationErr(io::Error),
    ReadToEndErr(ReadToEndError),
    TerminationErr(WriteError),
    WriteError(WriteError),
}
impl PartialEq for QuicClientError {
    fn eq(&self, other: &Self) -> bool {
        use QuicClientError::*;

        match (self, other) {
            (ConnectErr(a), ConnectErr(b)) => a == b,
            (ConnectionAttemptErr(a), ConnectionAttemptErr(b)) => a == b,
            (ConnectionErr(a), ConnectionErr(b)) => a == b,
            (OutgoingEndpointCreationErr(a), OutgoingEndpointCreationErr(b)) => {
                a.to_string() == b.to_string()
            }
            (ReadToEndErr(a), ReadToEndErr(b)) => a == b,
            (TerminationErr(a), TerminationErr(b)) => a == b,
            (WriteError(a), WriteError(b)) => a == b,
            _ => false,
        }
    }
}

/// A struct acting as a client for the Quic protocol
pub struct QuicClient {
    addr: SocketAddr,
    tls_crt: Option<Vec<u8>>,
    connection_attempt: u32,
    max_connection_attempt: u32,
    outgoing_endpoint_connection: Option<Endpoint>,
    quic_connection: Option<NewConnection>,
    server_name: String,
}
impl Drop for QuicClient {
    fn drop(&mut self) {
        block_on(async {
            let _ = self.disconnect().await;
        });
    }
}
impl QuicClient {
    /// Makes a new client instance with TLS verification
    pub fn new<A: ToString>(
        p_addr: A,
        p_server_name: String,
        p_max_connection_attempt: u32,
        p_tls_crt: Option<A>,
    ) -> Self {
        let tls_crt: Option<Vec<u8>>;
        if let Some(file) = p_tls_crt {
            tls_crt = Some(fs::read(file.to_string()).unwrap());
        } else {
            tls_crt = None;
        }

        Self {
            addr: p_addr.to_string().parse().unwrap(),
            connection_attempt: 0,
            max_connection_attempt: p_max_connection_attempt,
            outgoing_endpoint_connection: None,
            quic_connection: None,
            server_name: p_server_name,
            tls_crt,
        }
    }

    /// Makes a new client instance without TLS verification
    pub fn new_insecure<A: ToString>(
        p_addr: A,
        p_server_name: String,
        p_max_connection_attempt: u32,
    ) -> Self {
        Self::new(p_addr, p_server_name, p_max_connection_attempt, None)
    }

    async fn connect_inner(&mut self) -> Result<(), QuicClientError> {
        let mut endpoint = Endpoint::client("127.0.0.1:0".parse().unwrap())
            .map_err(|e| QuicClientError::OutgoingEndpointCreationErr(e))?;

        let crypto = if let Some(cert) = &self.tls_crt {
            let mut roots = rustls::RootCertStore::empty();
            roots.add(&rustls::Certificate(cert.to_vec())).unwrap();

            rustls::ClientConfig::builder()
                .with_safe_defaults()
                .with_root_certificates(roots)
                .with_no_client_auth()
        } else {
            rustls::ClientConfig::builder()
                .with_safe_defaults()
                .with_custom_certificate_verifier(SkipServerVerification::new())
                .with_no_client_auth()
        };

        let client_cfg = ClientConfig::new(Arc::new(crypto));
        endpoint.set_default_client_config(client_cfg);

        self.outgoing_endpoint_connection = Some(endpoint);

        // Connecting to server
        self.quic_connection = Some(
            self.outgoing_endpoint_connection
                .as_ref()
                .unwrap()
                .connect(self.addr, &self.server_name)
                .map_err(|e| QuicClientError::ConnectErr(e))?
                .await
                .map_err(|e| QuicClientError::ConnectionErr(e))?,
        );

        Ok(())
    }

    /// Connects to the endpoint
    pub async fn connect(&mut self) -> Result<(), QuicClientError> {
        self.connection_attempt = 0;

        loop {
            let connection = self.connect_inner().await;

            let mut connection_attempt_errors: Vec<QuicClientError> = Vec::new();
            match connection {
                Ok(_) => {
                    self.connection_attempt = 0;

                    return Ok(());
                }
                Err(e) => {
                    if !connection_attempt_errors.contains(&e) {
                        connection_attempt_errors.push(e);
                    }
                }
            };

            if self.connection_attempt > self.max_connection_attempt {
                return Err(QuicClientError::ConnectionAttemptErr(
                    connection_attempt_errors,
                ));
            }

            self.connection_attempt += 1;
        }
    }

    async fn get_the_connection(&mut self) -> Result<&mut Connection, QuicClientError> {
        if self.quic_connection.is_none() {
            self.connect().await?;
        }

        let connection = &mut self.quic_connection.as_mut().unwrap().connection;

        Ok(connection)
    }
    async fn write_and_finish(
        mut p_stream: SendStream,
        p_content: Vec<u8>,
    ) -> Result<(), QuicClientError> {
        p_stream
            .write_all(&p_content[..])
            .await
            .map_err(|e| QuicClientError::WriteError(e))?;

        p_stream
            .finish()
            .await
            .map_err(|e| QuicClientError::TerminationErr(e))?;

        Ok(())
    }

    /// Sends a unidirectional message to the endpoint
    pub async fn uni_send(&mut self, p_content: Vec<u8>) -> Result<(), QuicClientError> {
        let connection = self.get_the_connection().await?;

        let stream = connection
            .open_uni()
            .await
            .map_err(|e| QuicClientError::ConnectionErr(e))?;

        Self::write_and_finish(stream, p_content).await?;

        Ok(())
    }

    /// Sends a biodirectional message to the endpoint
    pub async fn bio_send(&mut self, p_content: Vec<u8>) -> Result<Vec<u8>, QuicClientError> {
        let connection = self.get_the_connection().await?;

        let (stream, rescv_stream) = connection
            .open_bi()
            .await
            .map_err(|e| QuicClientError::ConnectionErr(e))?;

        Self::write_and_finish(stream, p_content).await?;

        let resp = rescv_stream
            .read_to_end(usize::max_value())
            .await
            .map_err(|e| QuicClientError::ReadToEndErr(e))?;

        Ok(resp)
    }

    async fn outgoing_endpoint_wait_idle(&self) {
        self.outgoing_endpoint_connection
            .as_ref()
            .unwrap()
            .wait_idle()
            .await;
    }

    /// disconnects from the endpoint
    pub async fn disconnect(&mut self) {
        if let Some(quic_connection) = &self.quic_connection {
            // Putting a close to connection
            quic_connection.connection.close(0u32.into(), b"done");

            // Giving the server a fair chance to receive the close packet
            self.outgoing_endpoint_wait_idle().await;

            drop(self.quic_connection.as_mut().unwrap());

            // Making sure the server has a chance to clean up
            self.outgoing_endpoint_wait_idle().await;

            drop(self.outgoing_endpoint_connection.as_mut().unwrap());
        }

        self.quic_connection = None;
        self.outgoing_endpoint_connection = None;
    }
}

/// Dummy certificate verifier that treats any certificate as valid.
/// NOTE, such verification is vulnerable to MITM attacks, but convenient for testing.
struct SkipServerVerification;
impl SkipServerVerification {
    fn new() -> Arc<Self> {
        Arc::new(Self)
    }
}
impl rustls::client::ServerCertVerifier for SkipServerVerification {
    fn verify_server_cert(
        &self,
        _end_entity: &rustls::Certificate,
        _intermediates: &[rustls::Certificate],
        _server_name: &rustls::ServerName,
        _scts: &mut dyn Iterator<Item = &[u8]>,
        _ocsp_response: &[u8],
        _now: std::time::SystemTime,
    ) -> Result<rustls::client::ServerCertVerified, rustls::Error> {
        Ok(rustls::client::ServerCertVerified::assertion())
    }
}
