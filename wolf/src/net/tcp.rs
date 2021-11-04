use super::{
    callback::{OnMessageCallback, OnSocketCallback},
    tls::{load_root_ca, TlsPrivateKeyType},
};
use crate::{chrono::gametime::GameTime, net::tls};
use anyhow::{anyhow, ensure};
use std::{net::SocketAddr, path::Path, str::FromStr, sync::Arc};
use tokio::{
    io::{split, AsyncReadExt, AsyncWriteExt},
    net::{TcpListener, TcpStream},
    time::Instant,
};
use tokio_rustls::{
    rustls::{self, ServerConfig},
    TlsAcceptor, TlsConnector,
};

const MAX_BUFFER_SIZE: usize = 1024; //1K
const SEND_RECEIVE_TIMEOUT_SECONDS: u64 = 3;

async fn accept_connection<T>(
    p_stream: T,
    p_peer_address: SocketAddr,
    p_on_accept_callback: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_callback: OnSocketCallback,
) -> anyhow::Result<()>
where
    T: tokio::io::AsyncRead + tokio::io::AsyncWrite,
{
    // check for accept this connection
    p_on_accept_callback.run(&p_peer_address)?;

    // split stream into reader and writer
    let (mut reader, mut writer) = split(p_stream);

    // don't read more than 1K
    let mut buf = [0u8; MAX_BUFFER_SIZE];
    let mut size: usize;
    let mut socket_io_timeout = Instant::now();
    let mut socket_live_time = GameTime::new();
    socket_live_time.set_fixed_time_step(false);

    // let's loop for read and writing to the socket
    loop {
        socket_live_time.tick();
        // read buffer
        size = reader.read(&mut buf).await?;
        if size > 0 {
            socket_io_timeout = Instant::now();
            p_on_message.run(&socket_live_time, &mut size, &mut buf)?;
            if size > 0 {
                let v = buf[0..size].to_vec();
                writer.write(&v).await?;
                writer.flush().await?;
            }
        } else {
            let dur = Instant::now() - socket_io_timeout;
            if dur.as_secs() > SEND_RECEIVE_TIMEOUT_SECONDS {
                break;
            }
        }
    }
    let ret = p_on_close_callback.run(&p_peer_address);
    ret
}

pub async fn server(
    p_address: &str,
    p_port: u16,
    p_tls: bool,
    p_tls_certificate_path: Option<&Path>,
    p_tls_private_key_path: Option<&Path>,
    p_tls_private_type: Option<&TlsPrivateKeyType>,
    p_on_bind_socket: OnSocketCallback,
    p_on_accept_connection: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_connection: OnSocketCallback,
    p_on_close_socket: OnSocketCallback,
) -> anyhow::Result<()> {
    let address = format!("{}:{}", p_address, p_port);
    let socket_addr = SocketAddr::from_str(&address)?;
    if p_tls {
        //tls-mode
        let err_msg = "Invalid Parameters for tcp::run_server";

        //load certificate
        let crt = p_tls_certificate_path.ok_or(
            anyhow!(err_msg).context("p_tls_certificate_path not provided for tcp server"),
        )?;
        let certs = tls::load_certs(crt).await?;
        ensure!(certs.len() == 0, "missing certificate for TLS tcp server");

        //load private key
        let key = p_tls_private_key_path.ok_or(
            anyhow!(err_msg).context("p_tls_private_key_path not provided for tcp server"),
        )?;
        //load private key type
        let key_type = p_tls_private_type
            .ok_or(anyhow!(err_msg).context("p_tls_private_type not provided for tcp server"))?;
        let mut keys = tls::load_private_keys(key, key_type).await?;
        ensure!(keys.len() == 0, "missing private key for TLS tcp server");

        //create tls config
        let tls_server_config = ServerConfig::builder()
            .with_safe_defaults()
            .with_no_client_auth()
            .with_single_cert(certs, keys.remove(0))
            .map_err(|err| std::io::Error::new(std::io::ErrorKind::InvalidInput, err))?;

        // run acceptor & listener
        let tls_acceptor = TlsAcceptor::from(Arc::new(tls_server_config));
        // bind to the socket
        let tcp_listener = TcpListener::bind(socket_addr).await?;
        p_on_bind_socket.run(&socket_addr)?;

        // start main loop
        while let Ok((stream, peer_addr)) = tcp_listener.accept().await {
            //clone necessary objects
            let tls = tls_acceptor.clone();
            let tls_on_message = p_on_message.clone();
            let tls_on_accept_con = p_on_accept_connection.clone();
            let tls_on_close_con = p_on_close_connection.clone();

            // accept a new tls connection
            tokio::spawn(async move {
                let res = tls.accept(stream).await;
                let ret = match res {
                    Ok(tls_stream) => {
                        accept_connection(
                            tls_stream,
                            peer_addr,
                            tls_on_accept_con,
                            tls_on_message,
                            tls_on_close_con,
                        )
                        .await
                    }
                    Err(e) => {
                        anyhow::bail!("cloud not accept tls. because {:?}", e)
                    }
                };
                ret
            });
        }
    } else {
        //no tls-mode
        let tcp_listener = TcpListener::bind(socket_addr).await?;
        // on bind
        p_on_bind_socket.run(&socket_addr)?;

        // start main loop
        while let Ok((stream, peer_addr)) = tcp_listener.accept().await {
            // accept a new connection
            tokio::spawn(accept_connection(
                stream,
                peer_addr,
                p_on_accept_connection.clone(),
                p_on_message.clone(),
                p_on_close_connection.clone(),
            ));
        }
    }

    // on close socket
    let ret = p_on_close_socket.run(&socket_addr);
    ret
}

async fn handle_client_stream<T>(p_stream: T, p_on_message: OnMessageCallback) -> anyhow::Result<()>
where
    T: tokio::io::AsyncRead + tokio::io::AsyncWrite,
{
    // don't read more than 1K
    let mut buf = [0u8; MAX_BUFFER_SIZE];
    let mut size = 0;
    let mut socket_live_time = GameTime::new();
    socket_live_time.set_fixed_time_step(false);

    let (mut reader, mut writer) = split(p_stream);

    // let's loop for read and writing to the socket
    loop {
        socket_live_time.tick();
        if p_on_message
            .run(&socket_live_time, &mut size, &mut buf)
            .is_err()
        {
            break;
        }
        if size > 0 {
            let v = buf[0..size].to_vec();
            writer.write(&v).await?;
            writer.flush().await?;
        }
        // read buffer
        size = reader.read(&mut buf).await?;
    }

    Ok(())
}

pub async fn client(
    p_domain_address: &str,
    p_port: u16,
    p_tls: bool,
    p_tls_ca_path: Option<&Path>,
    p_on_accept_connection: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_connection: OnSocketCallback,
) -> anyhow::Result<()> {
    //create address
    let address = format!("{}:{}", p_domain_address, p_port);
    let socket_addr = SocketAddr::from_str(&address)?;

    p_on_accept_connection.run(&socket_addr)?;

    if p_tls {
        // create root cert store
        let root_cert_store = load_root_ca(p_tls_ca_path).await?;
        // load tls config
        let config = rustls::ClientConfig::builder()
            .with_safe_defaults()
            .with_root_certificates(root_cert_store)
            .with_no_client_auth();
        // create tls connector
        let connector = TlsConnector::from(Arc::new(config));
        // create tcp stream
        let stream = TcpStream::connect(&socket_addr).await?;
        // domain
        let domain = rustls::ServerName::try_from(p_domain_address).map_err(|e| {
            anyhow!("invalid dnsname {:?}", e).context("invalid dnsname tcp::client")
        })?;

        // create tls tcp stream
        let stream = connector.connect(domain, stream).await?;
        handle_client_stream(stream, p_on_message).await?;
    } else {
        // create tcp stream
        let stream = TcpStream::connect(&socket_addr).await?;
        handle_client_stream(stream, p_on_message).await?;
    }
    let ret = p_on_close_connection.run(&socket_addr);
    ret
}

#[tokio::test]
async fn test() -> () {
    tokio::spawn(async move {
        // wait for server to be created
        tokio::time::sleep(std::time::Duration::from_secs(3)).await;

        let on_accept_connection = OnSocketCallback::new(Box::new(
            |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
                println!("client {:?} just connected to the server", p_socket_address);
                Ok(())
            },
        ));
        let on_close_connection = OnSocketCallback::new(Box::new(
            |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
                println!("client {:?} just closed", p_socket_address);
                Ok(())
            },
        ));

        let on_msg_callback = OnMessageCallback::new(Box::new(
            |p_socket_live_time: &GameTime,
             p_size: &mut usize,
             p_buffer: &mut [u8]|
             -> anyhow::Result<()> {
                println!(
                    "client: number of received byte(s) is {} at socket live time {}",
                    *p_size,
                    p_socket_live_time.get_total_elapsed_seconds()
                );

                if *p_size > 0 {
                    let msg = std::str::from_utf8(&p_buffer)?;
                    println!("client: received buffer is {}", msg);
                }
                //now store new bytes for write
                let msg = "hello...world!\0"; //make sure append NULL terminate
                p_buffer[0..msg.len()].copy_from_slice(msg.as_bytes());
                *p_size = msg.len();

                if p_socket_live_time.get_total_elapsed_seconds() > 10.0 {
                    anyhow::bail!("closing socket");
                }
                Ok(())
            },
        ));
        let ret = client(
            "0.0.0.0",
            8000,
            false,
            None,
            on_accept_connection,
            on_msg_callback,
            on_close_connection,
        )
        .await;
        assert!(ret.is_ok());
    });

    // block main thread with tcp server
    let on_bind_socket = OnSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
            println!("server: socket {:?} just binded", p_socket_address);
            Ok(())
        },
    ));

    let on_accept_connection = OnSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
            println!(
                "server: remote address with peer id {:?} just connected",
                p_socket_address
            );
            Ok(())
        },
    ));

    let on_msg_callback = OnMessageCallback::new(Box::new(
        |p_socket_live_time: &GameTime,
         p_size: &mut usize,
         p_buffer: &mut [u8]|
         -> anyhow::Result<()> {
            println!(
                "server: number of received byte(s) is {} at socket live time {}",
                *p_size,
                p_socket_live_time.get_total_elapsed_seconds()
            );
            if *p_size > 0 {
                let msg = std::str::from_utf8(&p_buffer)?;
                println!("server: received buffer is {}", msg);

                //now store new bytes for write
                let msg = "hello client!\0"; //make sure append NULL terminate
                p_buffer[0..msg.len()].copy_from_slice(msg.as_bytes());
                *p_size = msg.len();
            }
            Ok(())
        },
    ));

    let on_close_connection = OnSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
            println!(
                "server: remote address with peer id {:?} just disconnected",
                p_socket_address
            );
            Ok(())
        },
    ));

    let on_close_socket = OnSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
            println!("server: socket {:?} just closed", p_socket_address);
            Ok(())
        },
    ));

    let ret = server(
        "0.0.0.0",
        8000,
        false,
        None,
        None,
        None,
        on_bind_socket,
        on_accept_connection,
        on_msg_callback,
        on_close_connection,
        on_close_socket,
    )
    .await;

    println!("tcp tests done {:?}", ret);
}
