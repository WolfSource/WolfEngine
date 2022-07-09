use crate::stream::common::{
    buffer::{Buffer, BufferType, MAX_MSG_SIZE},
    callback::{OnCloseSocketCallback, OnMessageCallback, OnSocketCallback},
    protocols::TcpProtocol,
    timeouts,
    tls::{self},
};
use anyhow::{bail, Result};
use futures::StreamExt;
use std::{net::SocketAddr, str::FromStr};
use tokio::{
    io::{split, AsyncReadExt, AsyncWriteExt},
    time::Instant,
};

#[derive(Debug)]
pub struct TcpServerConfig<'a> {
    protocol: TcpProtocol,
    address: &'a str,
    port: u16,
    ttl: u32,
    accept_timeout_in_secs: f64,
    io_timeout_in_secs: f64,
    tls: Option<std::sync::Arc<tls::Tls>>,
}

async fn handle_tcp_connection<S>(
    p_stream: S,
    p_peer_address: SocketAddr,
    p_io_timeout_in_secs: f64,
    p_on_msg_callback: OnMessageCallback,
) -> Result<String>
where
    S: tokio::io::AsyncRead + tokio::io::AsyncWrite + Unpin + Send,
{
    let close_msg: String;
    let mut msg = Buffer {
        type_: BufferType::BINARY,
        size: 0,
        buf: [0_u8; MAX_MSG_SIZE],
    };
    let conn_live_time = Instant::now();

    // split stream into reader and writer
    let (mut reader, mut writer) = split(p_stream);
    let mut res: std::io::Result<usize>;

    loop {
        if p_io_timeout_in_secs > 0.0 {
            //try for accept incoming session within specific timeout
            res = tokio::select! {
                res1 = timeouts::timeout_for_read(p_io_timeout_in_secs) => res1,
                res2 = async {reader.read(&mut msg.buf).await} => res2,
            };
        } else {
            res = reader.read(&mut msg.buf).await;
        }

        if res.is_err() {
            close_msg = format!("{:?}", res);
            break;
        }

        // read buffer
        msg.size = res.unwrap_or(0);
        let elapsed_secs = conn_live_time.elapsed().as_secs_f64();
        let want_to_close_conn = p_on_msg_callback.run(elapsed_secs, &p_peer_address, &mut msg);
        if want_to_close_conn.is_err() {
            close_msg = format!(
                        "tcp connection will be closed because of the p_on_msg_callback request. Reason: {:?}",
                        want_to_close_conn
                    );
            break;
        }
        if msg.size > 0 {
            let v = msg.buf[0..msg.size].to_vec();
            let w_ret = writer.write(&v).await;
            if w_ret.is_ok() {
                let _r = writer.flush().await;
            } else {
                close_msg = format!("{:?}", w_ret);
                break;
            }
        }
    }
    Ok(close_msg)
}

#[allow(clippy::too_many_arguments)]
async fn handle_ws_text_msg<S>(
    p_ws_msg: &Result<String, tokio_tungstenite::tungstenite::Error>,
    p_on_msg_callback: &OnMessageCallback,
    p_peer_address: &SocketAddr,
    p_elapsed_secs: f64,
    p_msg_buffer: &mut Buffer,
    p_ws_stream: &mut tokio_tungstenite::WebSocketStream<S>,
    p_close_code: &mut tokio_tungstenite::tungstenite::protocol::frame::coding::CloseCode,
    p_close_msg: &mut String,
) -> bool
where
    S: tokio::io::AsyncRead + tokio::io::AsyncWrite + Unpin + Send,
{
    use tokio_tungstenite::tungstenite::protocol::frame::coding::CloseCode;
    use tokio_tungstenite::tungstenite::Message;

    p_msg_buffer.type_ = BufferType::TEXT;
    match p_ws_msg {
        Ok(str) => {
            // memory copy
            unsafe {
                let src_len = str.len();
                let src_ptr = str.as_bytes().as_ptr();
                let dst_ptr = p_msg_buffer.buf.as_mut_ptr();

                std::ptr::copy_nonoverlapping(src_ptr, dst_ptr, src_len);
            }

            let close = p_on_msg_callback.run(p_elapsed_secs, p_peer_address, p_msg_buffer);

            // close the connection
            if close.is_err() {
                *p_close_msg = format!("websocket connection is going to close because of the p_on_msg_callback request. Reason: {:?}", close);
                *p_close_code = CloseCode::Normal;
                return false;
            }

            //fill other data to zero for better converting array to string
            p_msg_buffer.reset();

            let str_res = std::str::from_utf8(p_msg_buffer.buf.as_slice());
            if str_res.is_ok() {
                let r = futures::SinkExt::send(
                    p_ws_stream,
                    Message::Text((str_res.unwrap()).to_owned()),
                )
                .await;
                if r.is_err() {
                    *p_close_msg =  format!("websocket connection is going to close, because the TEXT message could not send. Reason: {:?}", r);
                    *p_close_code = CloseCode::Abnormal;
                    return false;
                }
            } else {
                *p_close_msg =  "websocket connection is going to close. Reason: for MessageType::TEXT, the data which was provided by p_on_msg_callback is not UTF8".to_string();
                *p_close_code = CloseCode::Invalid;
                return false;
            }
        }
        Err(e) => {
            *p_close_msg = format!(
                "websocket connection is going to close. Reason: Unsupported text message. {:?}",
                e
            );
            *p_close_code = CloseCode::Unsupported;
            return false;
        }
    };
    true
}

// TODO: fix too_many_arguments
#[allow(clippy::too_many_lines)]
async fn handle_ws_connection<S>(
    p_stream: S,
    p_peer_address: SocketAddr,
    p_io_timeout_in_secs: f64,
    p_on_msg_callback: OnMessageCallback,
) -> Result<String>
where
    S: tokio::io::AsyncRead + tokio::io::AsyncWrite + Unpin + Send,
{
    use tokio_tungstenite::tungstenite::protocol::frame::coding::CloseCode;
    use tokio_tungstenite::tungstenite::protocol::CloseFrame;
    use tokio_tungstenite::tungstenite::Error;
    use tokio_tungstenite::tungstenite::Message;

    let mut close_msg = String::new();
    let mut msg_buffer = Buffer::new(BufferType::TEXT);
    let mut close_code = CloseCode::Unsupported;
    let mut res: Option<Result<Message, Error>>;
    let conn_live_time = Instant::now();

    let mut ws_stream = tokio_tungstenite::accept_async(p_stream).await?;
    // let's loop for reading and writing to the socket
    loop {
        if p_io_timeout_in_secs > 0.0 {
            //try for accept incoming session within specific timeout
            res = tokio::select! {
                res1 = timeouts::timeout_for_read_ws(p_io_timeout_in_secs) =>
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
                } else if !msg.is_empty() && (msg.is_text() || msg.is_binary()) {
                    msg_buffer.size = msg.len();
                    // don not allow process message more than 1K
                    if msg_buffer.size > MAX_MSG_SIZE {
                        close_msg = format!(
                                    "websocket connection is going to close. Reason: Received message size is greater than {}",
                                    MAX_MSG_SIZE
                                );
                        close_code = CloseCode::Size;
                        break;
                    }

                    let want_to_close_conn: Result<()>;
                    let elapsed_secs = conn_live_time.elapsed().as_secs_f64();
                    if msg.is_text() {
                        let msg_res = msg.into_text();
                        if !handle_ws_text_msg(
                            &msg_res,
                            &p_on_msg_callback,
                            &p_peer_address,
                            elapsed_secs,
                            &mut msg_buffer,
                            &mut ws_stream,
                            &mut close_code,
                            &mut close_msg,
                        )
                        .await
                        {
                            break;
                        }
                    } else if msg.is_binary() {
                        //TODO: This section should refactor
                        msg_buffer.type_ = BufferType::BINARY;
                        let msg_vec = msg.clone().into_data();
                        let msg_res: Result<&[u8; 1024], std::array::TryFromSliceError> =
                            msg_vec.as_slice().try_into();
                        match msg_res {
                            Ok(arr) => {
                                msg_buffer.buf.copy_from_slice(arr);

                                want_to_close_conn = p_on_msg_callback.run(
                                    elapsed_secs,
                                    &p_peer_address,
                                    &mut msg_buffer,
                                );

                                if want_to_close_conn.is_err() {
                                    close_msg = format!("websocket connection is going to close because of the p_on_msg_callback request. Reason: {:?}", want_to_close_conn);
                                    close_code = CloseCode::Normal;
                                    break;
                                }

                                let r = futures::SinkExt::send(
                                    &mut ws_stream,
                                    Message::Binary(msg_buffer.buf.to_vec()),
                                )
                                .await;
                                if r.is_err() {
                                    close_msg = format!("websocket connection is going to close, because the BINARY message could not send. Reason: {:?}", r);
                                    close_code = CloseCode::Abnormal;
                                    break;
                                }
                            }
                            Err(e) => {
                                close_msg = format!("websocket connection is going to close. Reason: Unsupported binary message. {:?}", e);
                                close_code = CloseCode::Unsupported;
                                break;
                            }
                        };
                    } else {
                        close_msg = "websocket connection is going to close. Reason: Unsupported message type".to_string();
                        close_code = CloseCode::Unsupported;
                        break;
                    }
                } else {
                    close_msg = "websocket connection is going to close. Reason: Received an empty or invalid message".to_string();
                    close_code = CloseCode::Invalid;
                    break;
                }
            }
        }
    }
    //send close message to endpoint connection
    let close_frame = CloseFrame {
        code: close_code,
        reason: close_msg.clone().into(),
    };
    futures::SinkExt::send(&mut ws_stream, Message::Close(Some(close_frame))).await?;
    Ok(close_msg)
}

async fn accept_connection<S>(
    p_protocol: TcpProtocol,
    p_stream: S,
    p_peer_address: SocketAddr,
    p_io_timeout_in_secs: f64,
    p_on_accept_connection: OnSocketCallback,
    p_on_msg_callback: OnMessageCallback,
    p_on_close_connection: OnCloseSocketCallback,
) -> Result<()>
where
    S: tokio::io::AsyncRead + tokio::io::AsyncWrite + Unpin + Send,
{
    // check for accept this connection
    p_on_accept_connection.run(&p_peer_address)?;

    let close_msg_res = match p_protocol {
        TcpProtocol::TcpNative => {
            handle_tcp_connection(
                p_stream,
                p_peer_address,
                p_io_timeout_in_secs,
                p_on_msg_callback,
            )
            .await
        }
        TcpProtocol::TcpWebsocket => {
            handle_ws_connection(
                p_stream,
                p_peer_address,
                p_io_timeout_in_secs,
                p_on_msg_callback,
            )
            .await
        }
    };

    let close_msg = close_msg_res?;
    p_on_close_connection.run(&p_peer_address, &close_msg)
}

// TODO: fix clippy::future_not_send
#[allow(clippy::future_not_send)]
async fn server_main_loop(
    p_config: &'static TcpServerConfig<'static>,
    p_tcp_listener: tokio::net::TcpListener,
    p_on_accept_connection: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_connection: OnCloseSocketCallback,
    p_shutdown_signal: &parking_lot::Mutex<(
        std::sync::mpsc::Sender<bool>,
        std::sync::mpsc::Receiver<bool>,
    )>,
) {
    //start loop
    loop {
        //check for shutting down the tcp server
        let close_res = p_shutdown_signal.try_lock();
        let close = match close_res {
            Some(chan) => {
                if let Ok(b) = chan.1.try_recv() {
                    b
                } else {
                    false
                }
            }
            None => false,
        };
        if close {
            break;
        }

        //try for accept incoming session within specific timeout
        let res = if p_config.accept_timeout_in_secs > 0.0 {
            tokio::select! {
                res1 = timeouts::timeout_for_accept(p_config.accept_timeout_in_secs) => res1,
                res2 = async {p_tcp_listener.accept().await} => res2,
            }
        } else {
            p_tcp_listener.accept().await
        };

        if res.is_err() {
            //timeout reached or could not accept incoming connection successfully
            continue;
        }

        //copy from necessary objects
        let protocol = p_config.protocol;
        let io_timeout_in_secs = p_config.io_timeout_in_secs;

        //clone from necessary objects
        let on_message_callback = p_on_message.clone();
        let on_accept_connection = p_on_accept_connection.clone();
        let on_close_connection_callback = p_on_close_connection.clone();

        if let Some(tls) = p_config.tls.clone() {
            // start main loop
            if let Ok((stream, peer_addr)) = res {
                // accept a new tls connection
                let _j = tokio::spawn(async move {
                    let ret_accept = match tls.acceptor.accept(stream).await {
                        Ok(tls_stream) => {
                            accept_connection(
                                protocol,
                                tls_stream,
                                peer_addr,
                                io_timeout_in_secs,
                                on_accept_connection,
                                on_message_callback,
                                on_close_connection_callback,
                            )
                            .await
                        }
                        Err(e) => {
                            bail!("could not accept tls connection. because {:?}", e)
                        }
                    };
                    if ret_accept.is_err() {
                        tracing::error!("tls accept_connection failed because {:?}", ret_accept);
                    }
                    ret_accept
                });
            }
        } else {
            //no tls-mode
            if let Ok((stream, peer_addr)) = res {
                // accept a new connection
                let _r = tokio::spawn(async move {
                    if let Err(e) = accept_connection(
                        protocol,
                        stream,
                        peer_addr,
                        io_timeout_in_secs,
                        on_accept_connection,
                        on_message_callback,
                        on_close_connection_callback,
                    )
                    .await
                    {
                        tracing::error!("accept_connection failed because {}", e);
                    }
                });
            }
        }
    }
}

/// Run tcp server
/// # Panics
///
/// Might panic
#[allow(clippy::future_not_send)]
#[tracing::instrument]
pub async fn server(
    p_config: &'static TcpServerConfig<'static>,
    p_on_bind_socket: OnSocketCallback,
    p_on_accept_connection: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_connection: OnCloseSocketCallback,
    p_on_close_socket: OnSocketCallback,
    p_shutdown_signal: &'static parking_lot::Mutex<(
        std::sync::mpsc::Sender<bool>,
        std::sync::mpsc::Receiver<bool>,
    )>,
) -> Result<()> {
    //create address
    let address = format!("{}:{}", p_config.address, p_config.port);
    let socket_addr = SocketAddr::from_str(&address)?;
    //bind to the tcp listener
    let tcp_listener = tokio::net::TcpListener::bind(socket_addr).await?;

    //set ttl
    tcp_listener.set_ttl(p_config.ttl)?;

    //call bind callback
    p_on_bind_socket.run(&socket_addr)?;

    //start loop
    server_main_loop(
        p_config,
        tcp_listener,
        p_on_accept_connection,
        p_on_message,
        p_on_close_connection,
        p_shutdown_signal,
    )
    .await;

    // on close socket
    p_on_close_socket.run(&socket_addr)
}

// #[allow(clippy::too_many_lines)]
// #[tokio::main]
// #[test]
// async fn test_native() {
//     use std::sync::mpsc::{channel, Receiver, Sender};
//     use std::time::Duration;

//     static TCP_SERVER_CONFIG: TcpServerConfig = TcpServerConfig {
//         protocol: TcpProtocol::TcpNative,
//         address: "0.0.0.0",
//         port: 8000,
//         ttl: 100,
//         accept_timeout_in_secs: 5.0, //5 seconds
//         io_timeout_in_secs: 3.0,     // 3 seconds
//         tls: false,
//         tls_certificate_path: None,
//         tls_private_key_path: None,
//         tls_private_type: None,
//     };

//     lazy_static::lazy_static! {
//         static ref CHANNEL_MUTEX: parking_lot::Mutex<(Sender<bool>, Receiver<bool>)> = parking_lot::Mutex::new(channel::<bool>());
//     }

//     let _r = tokio::spawn(async move {
//         // wait for server to be created
//         tokio::time::sleep(Duration::from_secs(2)).await;

//         let on_accept_connection = OnSocketCallback::new(Box::new(
//             |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
//                 println!("client {:?} just connected to the server", p_socket_address);
//                 Ok(())
//             },
//         ));
//         let on_close_connection = OnSocketCallback::new(Box::new(
//             |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
//                 println!("client {:?} just closed", p_socket_address);

//                 //send request to close the server socket
//                 let _r = CHANNEL_MUTEX.lock().0.send(true).map_err(|e| {
//                     println!("could not send data to close_sig_channel. error: {:?}", e);
//                     e
//                 });
//                 Ok(())
//             },
//         ));

//         let on_msg_callback = OnMessageCallback::new(Box::new(
//             |p_socket_time_in_secs: f64,
//              p_peer_address: &SocketAddr,
//              _p_msg_type: &mut MessageType,
//              p_msg_size: &mut usize,
//              p_msg_buf: &mut [u8]|
//              -> anyhow::Result<()> {
//                 println!(
//                     "client: number of received byte(s) from {:?} is {}. socket live time {}",
//                     p_peer_address, *p_msg_size, p_socket_time_in_secs
//                 );

//                 if *p_msg_size > 0 {
//                     let msg = std::str::from_utf8(p_msg_buf)?;
//                     println!("client: received buffer is {}", msg);
//                 }
//                 //now store new bytes for write
//                 let msg = "hello...world!"; //make sure append NULL terminate
//                 p_msg_buf[0..msg.len()].copy_from_slice(msg.as_bytes());
//                 *p_msg_size = msg.len();

//                 if p_socket_time_in_secs > 5.0 {
//                     anyhow::bail!("closing socket");
//                 }
//                 Ok(())
//             },
//         ));

//         let tcp_client_config = TcpClientConfig {
//             //protocol: TcpProtocol::TcpNative, //we need to provide ws client code from rust
//             endpoint_address: "0.0.0.0",
//             port: 8000,
//             io_timeout_in_secs: 3.0, // 3 seconds
//             tls: false,
//             tls_ca_path: None,
//         };

//         let ret = client(
//             &tcp_client_config,
//             on_accept_connection,
//             on_msg_callback,
//             on_close_connection,
//         )
//         .await;
//         assert!(ret.is_ok(), "{:?}", ret);
//     });

//     // block main thread with tcp server
//     let on_bind_socket = OnSocketCallback::new(Box::new(
//         |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
//             println!("server: socket {:?} just bound", p_socket_address);
//             Ok(())
//         },
//     ));

//     let on_accept_connection = OnSocketCallback::new(Box::new(
//         |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
//             println!(
//                 "server: remote address with peer id {:?} just connected",
//                 p_socket_address
//             );
//             Ok(())
//         },
//     ));

//     let on_msg_callback = OnMessageCallback::new(Box::new(
//         |p_socket_time_in_secs: f64,
//          p_peer_address: &SocketAddr,
//          _p_msg_type: &mut MessageType,
//          p_msg_size: &mut usize,
//          p_msg_buf: &mut [u8]|
//          -> anyhow::Result<()> {
//             println!(
//                 "server: number of received byte(s) from {:?} is {}. socket live time {}",
//                 p_peer_address, *p_msg_size, p_socket_time_in_secs
//             );
//             if *p_msg_size > 0 {
//                 let msg = std::str::from_utf8(p_msg_buf)?;
//                 println!("server: received buffer is {}", msg);

//                 //now store new bytes for write
//                 let msg = "hello client!"; //make sure append NULL terminate
//                 p_msg_buf[0..msg.len()].copy_from_slice(msg.as_bytes());
//                 *p_msg_size = msg.len();
//             }
//             Ok(())
//         },
//     ));

//     let on_close_connection = OnCloseSocketCallback::new(Box::new(
//         |p_socket_address: &SocketAddr, p_close_msg: &str| -> anyhow::Result<()> {
//             println!(
//                 "server: remote address with peer id {:?} just disconnected. close message is {}",
//                 p_socket_address, p_close_msg
//             );
//             Ok(())
//         },
//     ));

//     let on_close_socket = OnSocketCallback::new(Box::new(
//         |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
//             println!("server: socket {:?} just closed", p_socket_address);
//             Ok(())
//         },
//     ));

//     let ret = server(
//         &TCP_SERVER_CONFIG,
//         on_bind_socket,
//         on_accept_connection,
//         on_msg_callback,
//         on_close_connection,
//         on_close_socket,
//         &CHANNEL_MUTEX,
//     )
//     .await;
//     assert!(ret.is_ok(), "{:?}", ret);

//     println!("native tcp tests were done");
// }

//#[allow(clippy::too_many_lines)]
//#[tokio::main]
//#[test]
//async fn test_ws() {
//    //run client code for this test
//    /*
//    <!DOCTYPE html>
//    <html>
//    <body>
//        <h1>Hello Wolf</h1>
//    </body>
//    <script>
//        let socket = new WebSocket("ws://127.0.0.1:8000");
//        socket.onopen = function (e) {
//            alert("[open] Connection established");
//            alert("Sending to server");
//            socket.send("Hello Wolf from WS");
//        };
//        socket.onmessage = function (event) {
//            console.log(`[message] Data received from server: ${event.data}`);
//        };
//        socket.onclose = function (event) {
//            if (event.wasClean) {
//                alert(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
//            } else {
//                // e.g. server process killed or network down
//                // event.code is usually 1006 in this case
//                alert('[close] Connection died');
//            }
//        };
//        socket.onerror = function (error) {
//            alert(`[error] ${error.message}`);
//        };
//    </script>
//    </html>
//        */
//
//    use std::sync::mpsc::{channel, Receiver, Sender};
//    lazy_static::lazy_static! {
//        static ref CHANNEL_MUTEX: parking_lot::Mutex<(Sender<bool>, Receiver<bool>)> = parking_lot::Mutex::new(channel::<bool>());
//    }
//
//    // block main thread with tcp server
//    let on_bind_socket = OnSocketCallback::new(Box::new(
//        |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
//            println!("server: socket {:?} just binded", p_socket_address);
//            Ok(())
//        },
//    ));
//
//    let on_accept_connection = OnSocketCallback::new(Box::new(
//        |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
//            println!(
//                "server: remote address with peer id {:?} just connected",
//                p_socket_address
//            );
//            Ok(())
//        },
//    ));
//
//    let on_msg_callback = OnMessageCallback::new(Box::new(
//        |p_socket_time_in_secs: &f64,
//         p_peer_address: &SocketAddr,
//         _p_msg_type: &mut MessageType,
//         p_msg_size: &mut usize,
//         p_msg_buf: &mut [u8]|
//         -> anyhow::Result<()> {
//            println!(
//                "server: number of received byte(s) from {:?} is {}. socket live time {}",
//                p_peer_address, *p_msg_size, p_socket_time_in_secs
//            );
//            if *p_msg_size > 0 {
//                let msg = std::str::from_utf8(p_msg_buf)?;
//                println!("server: received buffer is {}", msg);
//
//                //now store new bytes for write
//                let msg = "hello websocket client!";
//                p_msg_buf[0..msg.len()].copy_from_slice(msg.as_bytes());
//                *p_msg_size = msg.len();
//            }
//            Err(anyhow!("close"))
//        },
//    ));
//
//    let on_close_connection = OnCloseSocketCallback::new(Box::new(
//        |p_socket_address: &SocketAddr, p_close_msg: &str| -> anyhow::Result<()> {
//            println!(
//                "server: remote address with peer id {:?} just disconnected. close message is {}",
//                p_socket_address, p_close_msg
//            );
//            //send request to close the server socket
//            let _r = CHANNEL_MUTEX.lock().0.send(true).map_err(|e| {
//                println!("could not send data to close_sig_channel. error: {:?}", e);
//                e
//            });
//            Ok(())
//        },
//    ));
//
//    let on_close_socket = OnSocketCallback::new(Box::new(
//        |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
//            println!("server: socket {:?} just closed", p_socket_address);
//            Ok(())
//        },
//    ));
//
//    let ws_server_config = TcpServerConfig {
//        protocol: TcpProtocol::TcpWebsocket,
//        address: "0.0.0.0",
//        port: 8001,
//        ttl: 100,
//        accept_timeout_in_secs: 5.0, //5 seconds
//        io_timeout_in_secs: 3.0,   // 3 seconds
//        tls: false,
//        tls_certificate_path: None,
//        tls_private_key_path: None,
//        tls_private_type: None,
//    };
//
//    let ret = server(
//        &ws_server_config,
//        on_bind_socket,
//        on_accept_connection,
//        on_msg_callback,
//        on_close_connection,
//        on_close_socket,
//        &CHANNEL_MUTEX,
//    )
//    .await;
//    assert!(ret.is_ok(), "{:?}", ret);
//
//    println!("websocket tests done");
//}
