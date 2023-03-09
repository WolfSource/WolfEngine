use crate::{
    error::WError,
    system::{
        buffer::{Buffer, BufferType},
        socket::{
            callback::{OnMessageCallback, OnSocketCallback},
            timeouts::{timeout_for_connect, timeout_for_read_write, timeout_for_ssl_connect},
        },
    },
};
use boring::ssl::{ConnectConfiguration, SslConnector, SslMethod};
use instant::Instant;
use std::{net::SocketAddr, str::FromStr};
use tokio::io::{split, AsyncRead, AsyncReadExt, AsyncWrite, AsyncWriteExt, ReadHalf, WriteHalf};

#[derive(Debug)]
pub struct TcpClient {
    pub connect_number_of_retry: u32,
    pub connect_timeout_in_secs: f64,
    pub endpoint_address: String,
    pub io_number_of_retry: u32,
    pub io_timeout_in_secs: f64,
    pub port: u16,
    pub tls: bool,
}

impl TcpClient {
    #[must_use]
    pub fn new(p_endpoint_address: &str, p_port: u16) -> Self {
        Self {
            connect_number_of_retry: 3,
            connect_timeout_in_secs: 3.0,
            endpoint_address: p_endpoint_address.to_owned(),
            io_number_of_retry: 3,
            io_timeout_in_secs: 3.0,
            port: p_port,
            tls: false,
        }
    }

    /// # connect to a server
    /// # Errors
    ///
    /// returns `WError`
    pub async fn connect(
        &self,
        p_on_accept_connection: OnSocketCallback,
        p_on_message: OnMessageCallback,
        p_on_close_connection: OnSocketCallback,
    ) -> Result<(), WError> {
        //create an address
        let address = format!("{}:{}", self.endpoint_address, self.port);
        let socket_addr =
            SocketAddr::from_str(&address).map_err(WError::SystemSocketAddressParseFailed)?;

        // retry to connect for a number of times
        let mut retry = 0;
        let mut connected = false;
        while retry < self.connect_number_of_retry && !connected {
            // wait for a timeout
            match Self::connect_with_timeout(&socket_addr, self.connect_timeout_in_secs).await {
                Ok(stream) => {
                    // create tls
                    if self.tls {
                        //let ssl_method = boring::ssl::SslMethod::tls_client();
                        //let mut ssl_builder = boring::ssl::SslAcceptor::mozilla_modern(ssl_method)
                        //  .map_err(WError::SystemSocketSSLBuilderFailed)?;

                        // if let Some(cert) = self.tls_ca_path {
                        //     ssl_builder
                        //         .set_certificate_file(cert, boring::ssl::SslFiletype::PEM)
                        //         .map_err(WError::SystemSocketSSLBuilderFailed)?;
                        // }
                        // ssl_builder
                        //     .set_default_verify_paths()
                        //     .map_err(WError::SystemSocketSSLBuilderFailed)?;
                        // ssl_builder.set_verify(boring::ssl::SslVerifyMode::PEER);

                        // build ssl acceptor
                        //let acceptor = ssl_builder.build();

                        // let ssl_stream = tokio_boring::accept(&acceptor, stream)
                        //     .await
                        //     .map_err(WError::SystemSocketSSLAcceptFailed)?;

                        let ssl_builder = SslConnector::builder(SslMethod::tls_client())
                            .map_err(WError::SystemSocketSSLBuilderFailed)?;
                        let ssl_config = ssl_builder
                            .build()
                            .configure()
                            .map_err(WError::SystemSocketSSLBuilderFailed)?;

                        let ssl_stream = Self::ssl_connect_with_timeout(
                            ssl_config,
                            &self.endpoint_address,
                            stream,
                            self.connect_timeout_in_secs,
                        )
                        .await?;

                        connected = true;
                        p_on_accept_connection.run(&socket_addr)?;

                        Self::handle_client_stream(
                            ssl_stream,
                            &socket_addr,
                            self.io_timeout_in_secs,
                            self.io_number_of_retry,
                            p_on_message.clone(),
                        )
                        .await?;
                    } else {
                        connected = true;
                        // run accept callback
                        p_on_accept_connection.run(&socket_addr)?;

                        Self::handle_client_stream(
                            stream,
                            &socket_addr,
                            self.io_timeout_in_secs,
                            self.io_number_of_retry,
                            p_on_message.clone(),
                        )
                        .await?;
                    }
                }
                Err(e) => {
                    retry += 1;
                    if retry == self.connect_number_of_retry {
                        return Err(WError::SystemSocketConnectFailed(e));
                    }
                }
            }
        }
        p_on_close_connection.run(&socket_addr)
    }

    async fn handle_client_stream<T>(
        p_stream: T,
        p_peer_address: &SocketAddr,
        p_io_timeout_in_secs: f64,
        p_number_of_retry: u32,
        p_on_message: OnMessageCallback,
    ) -> Result<(), WError>
    where
        T: tokio::io::AsyncRead + tokio::io::AsyncWrite + Send,
    {
        let socket_live_time = Instant::now();
        let (mut reader, mut writer) = split(p_stream);
        let mut msg_buffer = Buffer::new(BufferType::BINARY);

        // let's loop for read and writing to the socket
        loop {
            let elapsed_secs = socket_live_time.elapsed().as_secs_f64();
            if p_on_message
                .run(elapsed_secs, p_peer_address, &mut msg_buffer)
                .is_err()
            {
                break;
            }

            // try to write to the socket for a number of times
            let mut retry = 0;
            let mut done = false;
            while retry < p_number_of_retry && !done {
                match Self::write_with_timeout(&mut writer, &msg_buffer, p_io_timeout_in_secs).await
                {
                    Ok(size) => {
                        if size > 0 {
                            done = true;
                            break;
                        } else if writer.flush().await.is_err() {
                            retry += 1;
                        }
                    }
                    Err(_e) => {
                        retry += 1;
                    }
                }
            }

            if !done {
                return Err(WError::SystemTCPSocketWriteFailed);
            }

            retry = 0;
            while retry < p_number_of_retry {
                match Self::read_with_timeout(
                    &mut reader,
                    &mut msg_buffer.buf,
                    p_io_timeout_in_secs,
                )
                .await
                {
                    Ok(size) => {
                        if size > 0 {
                            break;
                        } else if writer.flush().await.is_err() {
                            retry += 1;
                        }
                    }
                    Err(_e) => {
                        retry += 1;
                    }
                }
            }
        }

        Ok(())
    }

    async fn connect_with_timeout(
        p_socket_addr: &SocketAddr,
        p_timeout_in_secs: f64,
    ) -> std::io::Result<tokio::net::TcpStream> {
        if p_timeout_in_secs > 0.0 {
            tokio::select! {
                res1 = timeout_for_connect(p_timeout_in_secs) => res1,
                res2 = async {tokio::net::TcpStream::connect(p_socket_addr).await} => res2,
            }
        } else {
            tokio::net::TcpStream::connect(p_socket_addr).await
        }
    }

    async fn ssl_connect_with_timeout<S>(
        p_config: ConnectConfiguration,
        p_domain: &str,
        p_stream: S,
        p_timeout_in_secs: f64,
    ) -> Result<tokio_boring::SslStream<S>, WError>
    where
        S: AsyncRead + AsyncWrite + Unpin + Send,
    {
        if p_timeout_in_secs > 0.0 {
            tokio::select! {
                res1 = timeout_for_ssl_connect(p_timeout_in_secs) => res1,
                res2 = async {
                    let stream_res = tokio_boring::connect(p_config, p_domain, p_stream).await;
                    match stream_res {
                        Ok(s) => Ok(s),
                        Err(_e) => Err(WError::SystemSocketSSLConnectFailed),
                    }
                } => res2,
            }
        } else {
            let stream_res = tokio_boring::connect(p_config, p_domain, p_stream).await;
            match stream_res {
                Ok(s) => Ok(s),
                Err(_e) => Err(WError::SystemSocketSSLConnectFailed),
            }
        }
    }

    async fn write_with_timeout<T>(
        p_writer: &mut WriteHalf<T>,
        p_msg_buffer: &Buffer,
        p_timeout_in_secs: f64,
    ) -> std::io::Result<usize>
    where
        T: AsyncWrite + Send,
    {
        if p_timeout_in_secs > 0.0 {
            tokio::select! {
                res1 = timeout_for_read_write(p_timeout_in_secs) => res1,
                res2 = async {
                    p_writer
                    .write(&p_msg_buffer.buf[0..p_msg_buffer.size])
                    .await} => res2,
            }
        } else {
            p_writer
                .write(&p_msg_buffer.buf[0..p_msg_buffer.size])
                .await
        }
    }

    async fn read_with_timeout<T>(
        p_reader: &mut ReadHalf<T>,
        p_msg_buffer: &mut [u8],
        p_timeout_in_secs: f64,
    ) -> std::io::Result<usize>
    where
        T: AsyncRead + Send,
    {
        if p_timeout_in_secs > 0.0 {
            tokio::select! {
                res1 = timeout_for_read_write(p_timeout_in_secs) => res1,
                res2 = async {
                    p_reader
                    .read(p_msg_buffer)
                    .await} => res2,
            }
        } else {
            p_reader.read(p_msg_buffer).await
        }
    }
}
