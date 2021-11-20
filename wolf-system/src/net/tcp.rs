use super::{
    callback::{MessageType, OnCloseSocketCallback, OnMessageCallback, OnSocketCallback},
    tls::{load_root_ca, TlsPrivateKeyType},
};
use crate::net::tls;
use anyhow::{anyhow, ensure};
use futures::StreamExt;
use std::{net::SocketAddr, path::Path, str::FromStr, sync::Arc};
use std::{
    sync::{
        mpsc::{Receiver, Sender},
        Mutex,
    },
    time::Duration,
};
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

#[derive(Debug, Copy, Clone)]
pub enum TcpProtocol {
    TcpNative = 0,
    TcpWebsocket,
}

async fn timeout_for_accept(p_timeout_in_secs: f64) -> std::io::Result<(TcpStream, SocketAddr)> {
    use std::io::{Error, ErrorKind};

    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    //return Error
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout for accept tcp connection reached",
    ))
}

async fn timeout_for_read(p_timeout_in_secs: f64) -> std::io::Result<usize> {
    use std::io::{Error, ErrorKind};

    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout for read from tcp socket reached",
    ))
}

async fn timeout_for_read_ws(
    p_timeout_in_secs: f64,
) -> Option<
    core::result::Result<
        tokio_tungstenite::tungstenite::Message,
        tokio_tungstenite::tungstenite::Error,
    >,
> {
    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    None
}

async fn accept_connection<S>(
    p_protocol: TcpProtocol,
    p_stream: S,
    p_peer_address: SocketAddr,
    p_read_timeout_in_secs: f64,
    p_on_accept_callback: OnSocketCallback,
    p_on_msg_callback: OnMessageCallback,
    p_on_close_callback: OnCloseSocketCallback,
) -> anyhow::Result<()>
where
    S: tokio::io::AsyncRead + tokio::io::AsyncWrite + Unpin,
{
    let socket_live_time = Instant::now();

    // check for accept this connection
    p_on_accept_callback.run(&p_peer_address)?;

    // don't read more than 1K
    let mut close_msg = String::new();
    let mut msg_type = MessageType::BINARY;
    let mut msg_buf = [0u8; MAX_BUFFER_SIZE];
    let mut msg_size: usize;

    match p_protocol {
        TcpProtocol::TcpNative => {
            // split stream into reader and writer
            let (mut reader, mut writer) = split(p_stream);
            let mut res: std::io::Result<usize>;
            // let's loop for read and writing to the socket
            loop {
                if p_read_timeout_in_secs > 0.0 {
                    //try for accept incomming session within specific timout
                    res = tokio::select! {
                        res1 = timeout_for_read(p_read_timeout_in_secs) =>
                        {
                            res1
                        },
                        res2 = async {reader.read(&mut msg_buf).await} =>
                        {
                            res2
                        },
                    };
                } else {
                    res = reader.read(&mut msg_buf).await;
                }

                if res.is_err() {
                    close_msg = format!("{:?}", res);
                    break;
                }

                // read buffer
                msg_size = res.unwrap_or(0);
                let elapsed_secs = socket_live_time.elapsed().as_secs_f64();
                let want_to_close_conn = p_on_msg_callback.run(
                    &elapsed_secs,
                    &p_peer_address,
                    &mut msg_type,
                    &mut msg_size,
                    &mut msg_buf,
                );
                if want_to_close_conn.is_err() {
                    close_msg = format!(
                        "tcp connection will be closed because of the p_on_msg_callback request. Reason: {:?}",
                        want_to_close_conn
                    );
                    break;
                }
                if msg_size > 0 {
                    let v = msg_buf[0..msg_size].to_vec();
                    let wret = writer.write(&v).await;
                    if wret.is_ok() {
                        let _ = writer.flush().await;
                    } else {
                        close_msg = format!("{:?}", wret);
                        break;
                    }
                }
            }
        }
        TcpProtocol::TcpWebsocket => {
            use tokio_tungstenite::tungstenite::protocol::frame::coding::CloseCode;
            use tokio_tungstenite::tungstenite::Message;

            let mut close_code = CloseCode::Unsupported;
            let mut res: Option<
                core::result::Result<
                    tokio_tungstenite::tungstenite::Message,
                    tokio_tungstenite::tungstenite::Error,
                >,
            >;
            let mut ws_stream = tokio_tungstenite::accept_async(p_stream).await?;
            // let's loop for reading and writing to the socket
            loop {
                if p_read_timeout_in_secs > 0.0 {
                    //try for accept incomming session within specific timout
                    res = tokio::select! {
                        res1 = timeout_for_read_ws(p_read_timeout_in_secs) =>
                        {
                            res1
                        },
                        res2 = async {ws_stream.next().await} =>
                        {
                            res2
                        },
                    };
                } else {
                    res = ws_stream.next().await;
                }

                if let Some(msg_res) = res {
                    if msg_res.is_ok() {
                        let msg = msg_res.unwrap();
                        //check msg
                        if msg.is_close() {
                            break;
                        } else if !msg.is_empty() {
                            if msg.is_text() || msg.is_binary() {
                                msg_size = msg.len();
                                // don not allow process message more than 1K
                                if msg_size > MAX_BUFFER_SIZE {
                                    close_msg = format!(
                                        "websocket connection is going to close. Reason: Received message size is greater than {}",
                                        MAX_BUFFER_SIZE
                                    );
                                    close_code = CloseCode::Size;
                                    break;
                                }

                                let elapsed_secs = socket_live_time.elapsed().as_secs_f64();
                                let msg_vec = msg.clone().into_data();
                                msg_size = msg_vec.len();

                                msg_buf = msg_vec.as_slice().try_into()?;
                                let want_to_close_conn = p_on_msg_callback.run(
                                    &elapsed_secs,
                                    &p_peer_address,
                                    &mut msg_type,
                                    &mut msg_size,
                                    &mut msg_buf,
                                );
                                if want_to_close_conn.is_err() {
                                    close_msg = format!("websocket connection is going to close because of the p_on_msg_callback request. Reason: {:?}", want_to_close_conn);
                                    close_code = CloseCode::Normal;
                                    break;
                                }

                                match msg_type {
                                    MessageType::TEXT => {
                                        let str_res = std::str::from_utf8(&msg_buf);
                                        if str_res.is_ok() {
                                            let r = futures::SinkExt::send(
                                                &mut ws_stream,
                                                Message::Text(str_res.unwrap().to_owned()),
                                            )
                                            .await;
                                            if r.is_err() {
                                                close_msg = format!("websocket connection is going to close, because the TEXT message could not send. Reason: {:?}", r);
                                                close_code = CloseCode::Abnormal;
                                                break;
                                            }
                                        } else {
                                            close_msg = "websocket connection is going to close. Reason: for MessageType::TEXT, the data which was provided by p_on_msg_callback is not UTF8".to_string();
                                            close_code = CloseCode::Invalid;
                                            break;
                                        }
                                    }
                                    MessageType::BINARY => {
                                        let r = futures::SinkExt::send(
                                            &mut ws_stream,
                                            Message::Binary(msg_buf.to_vec()),
                                        )
                                        .await;
                                        if r.is_err() {
                                            close_msg = format!("websocket connection is going to close, because the BINARY message could not send. Reason: {:?}", r);
                                            close_code = CloseCode::Abnormal;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            //send close message to endpoint connection
            use tokio_tungstenite::tungstenite::protocol::CloseFrame;
            let close_frame = CloseFrame {
                code: close_code,
                reason: close_msg.clone().into(),
            };
            let _ = futures::SinkExt::send(&mut ws_stream, Message::Close(Some(close_frame))).await;
        }
    }

    let ret = p_on_close_callback.run(&p_peer_address, &close_msg);
    ret
}

async fn tls_acceptor(
    p_tls_certificate_path: Option<&Path>,
    p_tls_private_key_path: Option<&Path>,
    p_tls_private_type: Option<&TlsPrivateKeyType>,
) -> anyhow::Result<TlsAcceptor> {
    //tls-mode
    let err_msg = "Invalid Parameters for tcp::run_server";

    //load certificate
    let crt = p_tls_certificate_path
        .ok_or(anyhow!(err_msg).context("p_tls_certificate_path not provided for tcp server"))?;
    let certs = tls::load_certs(crt).await?;
    ensure!(certs.len() == 0, "missing certificate for TLS tcp server");

    //load private key
    let key = p_tls_private_key_path
        .ok_or(anyhow!(err_msg).context("p_tls_private_key_path not provided for tcp server"))?;
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
    Ok(tls_acceptor)
}

pub async fn server(
    p_protocol: TcpProtocol,
    p_address: &str,
    p_port: u16,
    p_ttl: u32,
    p_accept_timeout_in_secs: f64,
    p_read_timeout_in_secs: f64,
    p_tls: bool,
    p_tls_certificate_path: Option<&Path>,
    p_tls_private_key_path: Option<&Path>,
    p_tls_private_type: Option<&TlsPrivateKeyType>,
    p_close_sig_channel: &Mutex<(Sender<bool>, Receiver<bool>)>,
    p_on_bind_socket: OnSocketCallback,
    p_on_accept_connection: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_connection: OnCloseSocketCallback,
    p_on_close_socket: OnSocketCallback,
) -> anyhow::Result<()> {
    let address = format!("{}:{}", p_address, p_port);
    let socket_addr = SocketAddr::from_str(&address)?;

    // bind to the socket
    let tcp_listener = TcpListener::bind(socket_addr).await?;
    tcp_listener.set_ttl(p_ttl)?;

    p_on_bind_socket.run(&socket_addr)?;

    let mut tls_acc: Option<TlsAcceptor> = None;

    if p_tls {
        //create tls acceptor
        let tls = tls_acceptor(
            p_tls_certificate_path,
            p_tls_private_key_path,
            p_tls_private_type,
        )
        .await?;
        tls_acc = Option::from(tls);
    }

    loop {
        //close current socket, if it was requested by close signal channel
        let close_res = p_close_sig_channel.try_lock();
        let close = match close_res {
            Ok(chan) => {
                if let Ok(b) = chan.1.try_recv() {
                    b
                } else {
                    false
                }
            }
            Err(_) => false,
        };
        if close {
            break;
        }

        let _res: std::io::Result<(TcpStream, SocketAddr)>;
        if p_accept_timeout_in_secs > 0.0 {
            //try for accept incomming session within specific timout
            _res = tokio::select! {
                res1 = timeout_for_accept(p_accept_timeout_in_secs) =>
                {
                    res1
                },
                res2 = async {tcp_listener.accept().await} =>
                {
                    res2
                },
            };
        } else {
            _res = tcp_listener.accept().await;
        }

        if _res.is_err() {
            //timeout reached or could not accept incomming connectoin successfully
            continue;
        }

        if p_tls {
            // start main loop
            if let Ok((stream, peer_addr)) = _res {
                //clone necessary objects
                let tls = tls_acc.clone().unwrap();
                let tls_on_message = p_on_message.clone();
                let tls_on_accept_con = p_on_accept_connection.clone();
                let tls_on_close_con = p_on_close_connection.clone();

                // accept a new tls connection
                tokio::spawn(async move {
                    let res = tls.accept(stream).await;
                    let ret = match res {
                        Ok(tls_stream) => {
                            accept_connection(
                                p_protocol,
                                tls_stream,
                                peer_addr,
                                p_read_timeout_in_secs,
                                tls_on_accept_con,
                                tls_on_message,
                                tls_on_close_con,
                            )
                            .await
                        }
                        Err(e) => {
                            anyhow::bail!("could not accept tls connection. because {:?}", e)
                        }
                    };
                    if ret.is_err() {
                        println!("tls accept_connection failed because {:?}", ret);
                    }
                    ret
                });
            }
        } else {
            //no tls-mode
            if let Ok((stream, peer_addr)) = _res {
                let on_message_callback = p_on_message.clone();
                let on_accept_connection = p_on_accept_connection.clone();
                let on_close_connection_callback = p_on_close_connection.clone();
                // accept a new connection
                tokio::spawn(async move {
                    if let Err(e) = accept_connection(
                        p_protocol,
                        stream,
                        peer_addr,
                        p_read_timeout_in_secs,
                        on_accept_connection,
                        on_message_callback,
                        on_close_connection_callback,
                    )
                    .await
                    {
                        println!("accept_connection failed because {:?}", e);
                    }
                });
            }
        }
    }

    // on close socket
    let ret = p_on_close_socket.run(&socket_addr);
    ret
}

async fn handle_client_stream<T>(
    p_stream: T,
    p_peer_address: &SocketAddr,
    p_read_timeout_in_secs: &f64,
    p_on_message: OnMessageCallback,
) -> anyhow::Result<()>
where
    T: tokio::io::AsyncRead + tokio::io::AsyncWrite,
{
    // don't read more than 1K
    let mut msg_type = MessageType::BINARY;
    let mut msg_buf = [0u8; MAX_BUFFER_SIZE];
    let mut msg_size = 0;
    let socket_live_time = Instant::now();

    let (mut reader, mut writer) = split(p_stream);
    let mut res: std::io::Result<usize>;

    // let's loop for read and writing to the socket
    loop {
        let elapsed_secs = socket_live_time.elapsed().as_secs_f64();
        if p_on_message
            .run(
                &elapsed_secs,
                &p_peer_address,
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
            writer.write(&v).await?;
            writer.flush().await?;
        }

        if p_read_timeout_in_secs > &0.0 {
            //try for accept incomming session within specific timout
            res = tokio::select! {
                res1 = timeout_for_read(*p_read_timeout_in_secs) =>
                {
                    res1
                },
                res2 = async {reader.read(&mut msg_buf).await} =>
                {
                    res2
                },
            };
        } else {
            res = reader.read(&mut msg_buf).await;
        }

        // read from buffer
        msg_size = res?;
    }

    Ok(())
}

pub async fn client(
    p_domain_address: &str,
    p_port: u16,
    p_read_timeout_in_secs: f64,
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
        let peer_address = stream.peer_addr()?;

        // create tls tcp stream
        let stream = connector.connect(domain, stream).await?;
        handle_client_stream(stream, &peer_address, &p_read_timeout_in_secs, p_on_message).await?;
    } else {
        // create tcp stream
        let stream = TcpStream::connect(&socket_addr).await?;
        let peer_address = stream.peer_addr()?;

        handle_client_stream(stream, &peer_address, &p_read_timeout_in_secs, p_on_message).await?;
    }
    let ret = p_on_close_connection.run(&socket_addr);
    ret
}

#[tokio::main]
#[test]
async fn test() -> () {
    use std::sync::mpsc::channel;
    use std::time::Duration;

    lazy_static::lazy_static! {
        static ref CHANNEL_MUTEX: Mutex<(Sender<bool>, Receiver<bool>)> = Mutex::new(channel::<bool>());
    }

    tokio::spawn(async move {
        // wait for server to be created
        tokio::time::sleep(Duration::from_secs(2)).await;

        let on_accept_connection = OnSocketCallback::new(Box::new(
            |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
                println!("client {:?} just connected to the server", p_socket_address);
                Ok(())
            },
        ));
        let on_close_connection = OnSocketCallback::new(Box::new(
            |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
                println!("client {:?} just closed", p_socket_address);
                //send request to close the server socket
                let _ = CHANNEL_MUTEX.lock().and_then(|channel| {
                    let _ = channel.0.send(true).and_then(|_| Ok(())).or_else(|e| {
                        println!("could not send data to close_sig_channel. error: {:?}", e);
                        Err(e)
                    });
                    Ok(())
                });
                Ok(())
            },
        ));

        let on_msg_callback = OnMessageCallback::new(Box::new(
            |p_socket_time_in_secs: &f64,
             p_peer_address: &SocketAddr,
             _p_msg_type: &mut MessageType,
             p_msg_size: &mut usize,
             p_msg_buf: &mut [u8]|
             -> anyhow::Result<()> {
                println!(
                    "client: number of received byte(s) from {:?} is {}. socket live time {}",
                    p_peer_address, *p_msg_size, p_socket_time_in_secs
                );

                if *p_msg_size > 0 {
                    let msg = std::str::from_utf8(&p_msg_buf)?;
                    println!("client: received buffer is {}", msg);
                }
                //now store new bytes for write
                let msg = "hello...world!\0"; //make sure append NULL terminate
                p_msg_buf[0..msg.len()].copy_from_slice(msg.as_bytes());
                *p_msg_size = msg.len();

                if p_socket_time_in_secs > &10.0 {
                    anyhow::bail!("closing socket");
                }
                Ok(())
            },
        ));
        let ret = client(
            "0.0.0.0",
            8000,
            3.0,
            false,
            None,
            on_accept_connection,
            on_msg_callback,
            on_close_connection,
        )
        .await;
        assert!(ret.is_ok(), "{:?}", ret);
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
        |p_socket_time_in_secs: &f64,
         p_peer_address: &SocketAddr,
         _p_msg_type: &mut MessageType,
         p_msg_size: &mut usize,
         p_msg_buf: &mut [u8]|
         -> anyhow::Result<()> {
            println!(
                "server: number of received byte(s) from {:?} is {}. socket live time {}",
                p_peer_address, *p_msg_size, p_socket_time_in_secs
            );
            if *p_msg_size > 0 {
                let msg = std::str::from_utf8(&p_msg_buf)?;
                println!("server: received buffer is {}", msg);

                //now store new bytes for write
                let msg = "hello client!\0"; //make sure append NULL terminate
                p_msg_buf[0..msg.len()].copy_from_slice(msg.as_bytes());
                *p_msg_size = msg.len();
            }
            Ok(())
        },
    ));

    let on_close_connection = OnCloseSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr, p_close_msg: &str| -> anyhow::Result<()> {
            println!(
                "server: remote address with peer id {:?} just disconnected because of {}",
                p_socket_address, p_close_msg
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
        TcpProtocol::TcpNative,
        "0.0.0.0",
        8000,
        100,
        5.0, //5 seconds
        3.0, // 3 seconds
        false,
        None,
        None,
        None,
        &CHANNEL_MUTEX,
        on_bind_socket,
        on_accept_connection,
        on_msg_callback,
        on_close_connection,
        on_close_socket,
    )
    .await;

    println!("tcp tests done {:?}", ret);
}
