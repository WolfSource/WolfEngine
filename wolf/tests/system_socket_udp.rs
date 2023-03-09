//test it with this python code: ./udp_client.py
#[cfg(feature = "system_socket_server")]
#[tokio::main]
#[test]
async fn test() -> Result<(), wolf::error::WError> {
    use std::net::SocketAddr;
    use wolf::{
        error::WError,
        system::{
            buffer::Buffer,
            socket::{
                callback::{OnCloseSocketCallback, OnMessageCallback, OnSocketCallback},
                udp::UdpConnectionType,
            },
        },
    };

    // block main thread with udp server
    let on_bind_socket = OnSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr| -> Result<(), WError> {
            println!("udp socket {p_socket_address:?} just binded");
            Ok(())
        },
    ));
    let on_close_socket = OnCloseSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr, p_close_msg: &str| -> Result<(), WError> {
            println!("udp socket {p_socket_address:?} just closed, because: {p_close_msg}");
            Ok(())
        },
    ));

    let on_msg_callback = OnMessageCallback::new(Box::new(
        |p_socket_time_in_secs: f64,
         p_peer_address: &SocketAddr,
         p_msg: &mut Buffer|
         -> Result<(), WError> {
            println!(
                "client: number of received byte(s) from {p_peer_address:?} is {}. socket live time {p_socket_time_in_secs}",
                p_msg.size
            );

            if p_msg.size > 0 {
                let msg = std::str::from_utf8(p_msg.buf.as_slice())
                    .map_err(WError::SystemSocketUtf8Error);
                println!("client: received buffer is {msg:?}");
            }
            //now store new bytes for write
            let msg = "hello...world!\0"; //make sure append NULL terminate
            p_msg.buf[0..msg.len()].copy_from_slice(msg.as_bytes());
            p_msg.size = msg.len();

            if p_socket_time_in_secs > 10.0 {
                return Err(WError::Unknown);
            }
            Ok(())
        },
    ));

    let udp = wolf::system::socket::udp::UdpClient::new(
        "0.0.0.0".to_string(),
        8000,
        UdpConnectionType::Server,
    );
    udp.connect(on_bind_socket, on_msg_callback, on_close_socket)
        .await?;

    println!("udp tests done");
    Ok(())
}
