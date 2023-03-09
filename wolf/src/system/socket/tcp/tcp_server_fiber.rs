// use crate::stream::common::{
//     buffer::{Buffer, BufferType},
//     callback::{OnCloseSocketCallback, OnMessageCallback, OnSocketCallback},
//     protocols::TcpProtocol,
// };
// use std::{
//     net::SocketAddr,
//     str::FromStr,
//     time::{Duration, Instant},
// };

// #[derive(Debug)]
// pub struct TcpFiberServerConfig<'a> {
//     pub protocol: TcpProtocol,
//     pub address: &'a str,
//     pub port: u16,
//     pub fiber_num_of_workers: usize, //the minimum worker thread is 1, if you pass 0 to it, will use internal default
//     pub accept_timeout_in_secs: f64,
//     pub io_timeout_in_secs: f64,
// }

// #[inline]
// fn handle_tcp_connection(
//     mut p_stream: may::net::TcpStream,
//     p_peer_address: SocketAddr,
//     p_on_msg_callback: &OnMessageCallback,
// ) -> String {
//     let mut close_msg: String = String::new();
//     let mut msg = Buffer::new(BufferType::BINARY);
//     let conn_live_time = Instant::now();

//     loop {
//         let _r = std::io::Read::read(&mut p_stream, &mut msg.buf)
//             .map(|n| {
//                 msg.size  = n;
//                 let elapsed_secs = conn_live_time.elapsed().as_secs_f64();
//                 let close_conn = p_on_msg_callback.run(
//                     elapsed_secs,
//                     &p_peer_address,
//                     &mut msg,
//                 );
//                 if close_conn.is_err()
//                 {
//                     close_msg = format!("{close_conn:?}. ");
//                 }

//                 if msg.size > 0
//                 {
//                     let _r = std::io::Write::write(&mut p_stream, &msg.buf).map(|sent|
//                     {
//                         if sent > 0
//                         {
//                             //don't care about result of flush
//                             let _flushed = std::io::Write::flush(&mut p_stream);
//                         }
//                     }).map_err(|e|
//                     {
//                         let err_str = format!("{e:?}");
//                         close_msg += &err_str;
//                     });
//                 }
//             })
//             .map_err(|e| {
//                 close_msg = format!("tcp connection will be closed because of the p_on_msg_callback request. Reason: {e:?}");
//             });

//         if !close_msg.is_empty() {
//             close_msg = format!("tcp connection will be closed because of the p_on_msg_callback request. Reason: {close_msg}");
//             break;
//         }
//     }

//     close_msg
// }

// #[inline]
// fn handle_ws_connection(
//     mut _p_stream: may::net::TcpStream,
//     _p_peer_address: SocketAddr,
//     _p_on_msg_callback: &OnMessageCallback,
// ) -> String {
//     "close_msg_from_ws".to_string()
// }

// #[inline]
// fn accept_connection(
//     p_protocol: TcpProtocol,
//     p_stream: may::net::TcpStream,
//     p_peer_address: SocketAddr,
//     p_io_timeout_in_secs: f64,
//     p_on_accept_connection: &OnSocketCallback,
//     p_on_msg: &OnMessageCallback,
//     p_on_close_connection: &OnCloseSocketCallback,
// ) -> anyhow::Result<()> {
//     // check for accept this connection
//     p_on_accept_connection.run(&p_peer_address)?;

//     //set timeout
//     p_stream.set_read_timeout(Some(Duration::from_secs_f64(p_io_timeout_in_secs)))?;
//     p_stream.set_write_timeout(Some(Duration::from_secs_f64(p_io_timeout_in_secs)))?;

//     let close_msg = match p_protocol {
//         TcpProtocol::Native => handle_tcp_connection(p_stream, p_peer_address, p_on_msg),
//         TcpProtocol::Websocket => handle_ws_connection(p_stream, p_peer_address, p_on_msg),
//     };

//     p_on_close_connection.run(&p_peer_address, &close_msg)
// }

// #[tracing::instrument]
// fn server_main_loop(
//     p_worker_id: usize,
//     p_config: &'static TcpFiberServerConfig<'static>,
//     p_tcp_listener: &may::net::TcpListener,
//     p_on_accept_connection: OnSocketCallback,
//     p_on_message: OnMessageCallback,
//     p_on_close_connection: OnCloseSocketCallback,
//     p_shutdown_signal: &parking_lot::Mutex<(
//         std::sync::mpsc::Sender<bool>,
//         std::sync::mpsc::Receiver<bool>,
//     )>,
// ) {
//     loop {
//         //check for shutting down the tcp server
//         let close_res = p_shutdown_signal.try_lock();
//         let close = close_res.map_or(false, |chan| chan.1.try_recv().map_or(false, |b| b));
//         if close {
//             break;
//         }

//         let protocol = p_config.protocol;
//         let accept_timeout_in_secs = p_config.accept_timeout_in_secs;
//         let io_timeout_in_secs = p_config.io_timeout_in_secs;

//         //try for accept incoming session within specific timeout with a fiber
//         let mut res: std::io::Result<(may::net::TcpStream, SocketAddr)> = Err(std::io::Error::new(
//             std::io::ErrorKind::Unsupported,
//             "Initialized with ErrorKind::Unsupported",
//         ));
//         if accept_timeout_in_secs > 0.0 {
//             use may::{cqueue_add_oneshot, go};

//             let _id = may::select!(
//                 res1 = p_tcp_listener.accept() => { res = res1; },
//                 _ = may::coroutine::sleep(std::time::Duration::from_secs_f64(accept_timeout_in_secs)) =>
//                 {
//                     let msg = format!("timeout {accept_timeout_in_secs} seconds reached for accepting fiber based tcp listener");
//                     res = Err(std::io::Error::new( std::io::ErrorKind::TimedOut, msg));
//                 }
//             );
//         } else {
//             res = p_tcp_listener.accept();
//         }

//         //check stream result and handle the connection
//         if let Ok((stream, peer_addr)) = res {
//             //clone necessary objects for fiber
//             let on_accept_connection = p_on_accept_connection.clone();
//             let on_message = p_on_message.clone();
//             let on_close_connection = p_on_close_connection.clone();

//             may::go!(move || {
//                 if let Err(e) = accept_connection(
//                     protocol,
//                     stream,
//                     peer_addr,
//                     io_timeout_in_secs,
//                     &on_accept_connection,
//                     &on_message,
//                     &on_close_connection,
//                 ) {
//                     tracing::error!(
//                         "could not handle tcp fiber connection on worker #{}. because {:?}",
//                         p_worker_id,
//                         e
//                     );
//                 }
//             });
//         } else {
//             tracing::error!(
//                 "could not accept tcp fiber connection on worker #{}. because {:?}",
//                 p_worker_id,
//                 res
//             );
//         }
//     }
// }

// /// Run tcp server
// /// # Panics
// ///
// /// Might panic
// #[allow(clippy::future_not_send)]
// #[tracing::instrument]
// pub fn server(
//     p_config: &'static TcpFiberServerConfig<'static>,
//     p_on_bind_socket: OnSocketCallback,
//     p_on_accept_connection: OnSocketCallback,
//     p_on_message: OnMessageCallback,
//     p_on_close_connection: OnCloseSocketCallback,
//     p_shutdown_signal: &'static parking_lot::Mutex<(
//         std::sync::mpsc::Sender<bool>,
//         std::sync::mpsc::Receiver<bool>,
//     )>,
// ) {
//     //create address
//     let address = format!("{}:{}", p_config.address, p_config.port);
//     let socket_addr = SocketAddr::from_str(&address).unwrap();

//     //set fiber workers
//     let workers = p_config.fiber_num_of_workers;

//     let _may_config = may::config().set_workers(workers); //.set_stack_size(0);

//     //call bind callback
//     p_on_bind_socket.run(&socket_addr).unwrap();

//     //run fibers
//     may::coroutine::scope(|_s| {
//         for i in 0..workers {
//             //clone necessary objects for each worker thread
//             let on_accept_connection_for_each = p_on_accept_connection.clone();
//             let on_message_for_each = p_on_message.clone();
//             let on_close_connection_for_each = p_on_close_connection.clone();

//             may::go!(move || {
//                 //bind to the tcp listener
//                 let tcp_listener_res = may::net::TcpListener::bind(socket_addr);
//                 match tcp_listener_res {
//                     Ok(tcp_listener) => {
//                         //start loop
//                         server_main_loop(
//                             i,
//                             p_config,
//                             &tcp_listener,
//                             on_accept_connection_for_each,
//                             on_message_for_each,
//                             on_close_connection_for_each,
//                             p_shutdown_signal,
//                         );
//                     }
//                     Err(e) => {
//                         tracing::error!("could not bind to tcp fiber socket. because {:?}", e);
//                     }
//                 }
//             });
//         }
//     });
// }

// // #[allow(clippy::too_many_lines)]
// // #[tokio::main]
// // #[test]
// // async fn test_native() {
// //     use super::callback::MessageType;
// //     use std::sync::mpsc::{channel, Receiver, Sender};

// //     static TCP_FIBER_SERVER_CONFIG: TcpFiberServerConfig = TcpFiberServerConfig {
// //         protocol: TcpProtocol::TcpNative,
// //         address: "0.0.0.0",
// //         port: 8000,
// //         fiber_num_of_workers: 4,
// //         accept_timeout_in_secs: 15.0,
// //         io_timeout_in_secs: 3.0,
// //     };

// //     lazy_static::lazy_static! {
// //         static ref CHANNEL_MUTEX: parking_lot::Mutex<(Sender<bool>, Receiver<bool>)> = parking_lot::Mutex::new(channel::<bool>());
// //         static ref MUTEX_CV_PAIR : std::sync::Arc<(parking_lot::Mutex<bool>, parking_lot::Condvar)> = std::sync::Arc::new((parking_lot::Mutex::new(false), parking_lot::Condvar::new()));
// //     }

// //     let _r = tokio::spawn(async move {
// //         // wait for server to be created
// //         tokio::time::sleep(Duration::from_secs(2)).await;

// //         let on_accept_connection = OnSocketCallback::new(Box::new(
// //             |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
// //                 println!("client {:?} just connected to the server", p_socket_address);
// //                 Ok(())
// //             },
// //         ));
// //         let on_close_connection = OnSocketCallback::new(Box::new(
// //             |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
// //                 println!("client {:?} just closed", p_socket_address);

// //                 //release current condition variable
// //                 let mut started = MUTEX_CV_PAIR.0.lock();
// //                 *started = true;
// //                 let _notified = MUTEX_CV_PAIR.1.notify_one();

// //                 //send request to close the server socket
// //                 let _r = CHANNEL_MUTEX.lock().0.send(true).map_err(|e| {
// //                     println!("could not send data to close_sig_channel. error: {:?}", e);
// //                     e
// //                 });
// //                 Ok(())
// //             },
// //         ));

// //         let on_msg_callback = OnMessageCallback::new(Box::new(
// //             |p_socket_time_in_secs: f64,
// //              p_peer_address: &SocketAddr,
// //              _p_msg_type: &mut MessageType,
// //              p_msg_size: &mut usize,
// //              p_msg_buf: &mut [u8]|
// //              -> anyhow::Result<()> {
// //                 println!(
// //                     "client: number of received byte(s) from {:?} is {}. socket live time {}",
// //                     p_peer_address, *p_msg_size, p_socket_time_in_secs
// //                 );

// //                 if *p_msg_size > 0 {
// //                     let msg = std::str::from_utf8(p_msg_buf)?;
// //                     println!("client: received buffer is {}", msg);
// //                 }
// //                 //now store new bytes for write
// //                 let msg = "hello...world!"; //make sure append NULL terminate
// //                 p_msg_buf[0..msg.len()].copy_from_slice(msg.as_bytes());
// //                 *p_msg_size = msg.len();

// //                 if p_socket_time_in_secs > 5.0 {
// //                     anyhow::bail!("closing socket");
// //                 }
// //                 Ok(())
// //             },
// //         ));

// //         use crate::system::net::tcp_client::TcpClientConfig;
// //         let tcp_client_config = TcpClientConfig {
// //             //protocol: TcpProtocol::TcpNative, //we need to provide ws client code from rust
// //             endpoint_address: "0.0.0.0",
// //             port: 8000,
// //             io_timeout_in_secs: 3.0, // 3 seconds
// //             tls: false,
// //             tls_ca_path: None,
// //         };

// //         let ret = super::tcp_client::client(
// //             &tcp_client_config,
// //             on_accept_connection,
// //             on_msg_callback,
// //             on_close_connection,
// //         )
// //         .await;
// //         assert!(ret.is_ok(), "{:?}", ret);
// //     });

// //     // block main thread with tcp server
// //     let on_bind_socket = OnSocketCallback::new(Box::new(
// //         |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
// //             println!("fiber server: socket {:?} just bound", p_socket_address);
// //             Ok(())
// //         },
// //     ));

// //     let on_accept_connection = OnSocketCallback::new(Box::new(
// //         |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
// //             println!(
// //                 "fiber server: remote address with peer id {:?} just connected",
// //                 p_socket_address
// //             );
// //             Ok(())
// //         },
// //     ));

// //     let on_msg_callback = OnMessageCallback::new(Box::new(
// //         |p_socket_time_in_secs: f64,
// //          p_peer_address: &SocketAddr,
// //          _p_msg_type: &mut MessageType,
// //          p_msg_size: &mut usize,
// //          p_msg_buf: &mut [u8]|
// //          -> anyhow::Result<()> {
// //             println!(
// //                 "fiber server: number of received byte(s) from {:?} is {}. socket live time {}",
// //                 p_peer_address, *p_msg_size, p_socket_time_in_secs
// //             );
// //             if *p_msg_size > 0 {
// //                 let msg = std::str::from_utf8(p_msg_buf)?;
// //                 println!("server: received buffer is {}", msg);

// //                 //now store new bytes for write
// //                 let msg = "hello client!"; //make sure append NULL terminate
// //                 p_msg_buf[0..msg.len()].copy_from_slice(msg.as_bytes());
// //                 *p_msg_size = msg.len();
// //             }
// //             Ok(())
// //         },
// //     ));

// //     let on_close_connection = OnCloseSocketCallback::new(Box::new(
// //         |p_socket_address: &SocketAddr, p_close_msg: &str| -> anyhow::Result<()> {
// //             println!(
// //                 "fiber server: remote address with peer id {:?} just disconnected. close message is {}",
// //                 p_socket_address, p_close_msg
// //             );
// //             Ok(())
// //         },
// //     ));

// //     server(
// //         &TCP_FIBER_SERVER_CONFIG,
// //         on_bind_socket,
// //         on_accept_connection,
// //         on_msg_callback,
// //         on_close_connection,
// //         &CHANNEL_MUTEX,
// //     );

// //     //wait for fiber
// //     let mut started = MUTEX_CV_PAIR.0.lock();
// //     if !*started {
// //         MUTEX_CV_PAIR.1.wait(&mut started);
// //     }

// //     println!("native tcp_fiber tests were done");
// // }
