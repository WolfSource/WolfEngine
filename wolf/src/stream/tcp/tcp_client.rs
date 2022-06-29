use crate::stream::common::{
    callback::{MessageType, OnMessageCallback, OnSocketCallback},
    protocols::MAX_MSG_SIZE,
    timeouts,
};
use anyhow::{anyhow, Result};
use std::{fs::File, io::BufReader, path::Path};
use std::{net::SocketAddr, str::FromStr};
use tokio::{
    io::{split, AsyncReadExt, AsyncWriteExt},
    time::Instant,
};
use tokio_rustls::{rustls::RootCertStore, webpki};
use tokio_rustls::{
    rustls::{self, OwnedTrustAnchor},
    TlsConnector,
};

#[derive(Debug)]
pub struct TcpClientConfig<'a> {
    //protocol: TcpProtocol,
    pub endpoint_address: &'a str,
    pub port: u16,
    pub io_timeout_in_secs: f64,
    pub tls: bool,
    pub tls_ca_path: Option<&'a Path>,
}

/// # Errors
///
/// TODO: add error description
pub async fn client(
    p_config: &TcpClientConfig<'_>,
    p_on_accept_connection: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_connection: OnSocketCallback,
) -> Result<()> {
    const TRACE: &str = "tcp_client::client";

    //create address
    let address = format!("{}:{}", p_config.endpoint_address, p_config.port);
    let socket_addr =
        SocketAddr::from_str(&address).map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;

    p_on_accept_connection.run(&socket_addr)?;

    if p_config.tls {
        // create root cert store
        let root_cert_store = load_root_ca(p_config.tls_ca_path)?;
        // load tls config
        let config = rustls::ClientConfig::builder()
            .with_safe_defaults()
            .with_root_certificates(root_cert_store)
            .with_no_client_auth();
        // create tls connector
        let connector = TlsConnector::from(std::sync::Arc::new(config));
        // create tcp stream
        let stream = tokio::net::TcpStream::connect(&socket_addr)
            .await
            .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;

        // domain
        let domain = rustls::ServerName::try_from(p_config.endpoint_address)
            .map_err(|e| anyhow!("invalid DNS name {:?}", e).context("tcp::client"))?;
        let peer_address = stream
            .peer_addr()
            .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;

        // create tls tcp stream
        let stream = connector
            .connect(domain, stream)
            .await
            .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;
        handle_client_stream(
            stream,
            &peer_address,
            p_config.io_timeout_in_secs,
            p_on_message,
        )
        .await?;
    } else {
        // create tcp stream
        let stream = tokio::net::TcpStream::connect(&socket_addr)
            .await
            .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;
        let peer_address = stream
            .peer_addr()
            .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;

        handle_client_stream(
            stream,
            &peer_address,
            p_config.io_timeout_in_secs,
            p_on_message,
        )
        .await?;
    }
    p_on_close_connection.run(&socket_addr)
}

/// Load root certificate from specific path
/// # Panics
///
/// Will panic if cert is not valid
fn load_root_ca(p_root_ca_path: Option<&Path>) -> Result<RootCertStore> {
    const TRACE: &str = "tcp_client::load_root_ca";

    let mut root_cert_store = rustls::RootCertStore::empty();

    if let Some(ca_path) = p_root_ca_path {
        let mut pem = BufReader::new(
            File::open(ca_path).map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?,
        );
        let certs = rustls_pemfile::certs(&mut pem)
            .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;

        let trust_anchors = certs.iter().map(|cert| {
            let ta = webpki::TrustAnchor::try_from_cert_der(&cert[..]).unwrap();
            OwnedTrustAnchor::from_subject_spki_name_constraints(
                ta.subject,
                ta.spki,
                ta.name_constraints,
            )
        });
        root_cert_store.add_server_trust_anchors(trust_anchors);
    } else {
        root_cert_store.add_server_trust_anchors(webpki_roots::TLS_SERVER_ROOTS.0.iter().map(
            |ta| {
                OwnedTrustAnchor::from_subject_spki_name_constraints(
                    ta.subject,
                    ta.spki,
                    ta.name_constraints,
                )
            },
        ));
    }

    Ok(root_cert_store)
}

async fn handle_client_stream<T>(
    p_stream: T,
    p_peer_address: &SocketAddr,
    p_io_timeout_in_secs: f64,
    p_on_message: OnMessageCallback,
) -> Result<()>
where
    T: tokio::io::AsyncRead + tokio::io::AsyncWrite + Send,
{
    const TRACE: &str = "tcp_client::handle_client_stream";

    // don't read more than 1K
    let mut msg_type = MessageType::BINARY;
    let mut msg_buf = [0_u8; MAX_MSG_SIZE];
    let mut msg_size = 0;
    let socket_live_time = Instant::now();

    let (mut reader, mut writer) = split(p_stream);
    let mut res: std::io::Result<usize>;

    // let's loop for read and writing to the socket
    loop {
        let elapsed_secs = socket_live_time.elapsed().as_secs_f64();
        if p_on_message
            .run(
                elapsed_secs,
                p_peer_address,
                &mut msg_type,
                &mut msg_size,
                &mut msg_buf,
            )
            .is_err()
        {
            break;
        }
        if msg_size > 0 {
            let v = msg_buf[0..msg_size].to_vec();
            let s = writer
                .write(&v)
                .await
                .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;
            if s > 0 {
                writer
                    .flush()
                    .await
                    .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;
            }
        }

        if p_io_timeout_in_secs > 0.0 {
            //try for accept incoming session within specific timeout
            res = tokio::select! {
                res1 = timeouts::timeout_for_read(p_io_timeout_in_secs) => res1,
                res2 = async {reader.read(&mut msg_buf).await} => res2,
            };
        } else {
            res = reader.read(&mut msg_buf).await;
        }

        // read from buffer
        msg_size = res.map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;
    }

    Ok(())
}
