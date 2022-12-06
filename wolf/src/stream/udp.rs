use crate::stream::common::{
    buffer::{Buffer, BufferType},
    callback::{OnCloseSocketCallback, OnMessageCallback, OnSocketCallback},
};
use anyhow::{anyhow, Result};
use std::{net::SocketAddr, str::FromStr};
use tokio::{net::UdpSocket, time::Instant};

use super::common::buffer::MAX_MSG_SIZE;

#[derive(Debug)]
pub enum UdpConnectionType {
    UDPServer = 0, // read then write
    UDPClient,     // write then read
}

async fn timeout_for_read(p_timeout_in_secs: f64) -> std::io::Result<(usize, SocketAddr)> {
    use std::io::{Error, ErrorKind};

    tokio::time::sleep(std::time::Duration::from_secs_f64(p_timeout_in_secs)).await;
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout reached for udp read data from socket",
    ))
}

async fn timeout_for_send(p_timeout_in_secs: f64) -> std::io::Result<usize> {
    use std::io::{Error, ErrorKind};

    tokio::time::sleep(std::time::Duration::from_secs_f64(p_timeout_in_secs)).await;
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout reached for udp send data to socket",
    ))
}

async fn read(
    p_socket: &UdpSocket,
    p_buffer: &mut [u8; MAX_MSG_SIZE],
    p_read_write_timeout_in_secs: f64,
) -> std::io::Result<(usize, SocketAddr)> {
    if p_read_write_timeout_in_secs > 0.0 {
        //try to read data from UDP socket
        tokio::select! {
            res1 = timeout_for_read(p_read_write_timeout_in_secs) =>
            {
                res1
            },
            res2 = async { p_socket.recv_from(p_buffer).await } =>
            {
                res2
            },
        }
    } else {
        p_socket.recv_from(p_buffer).await
    }
}

async fn send(
    p_socket: &UdpSocket,
    peer_addr: &SocketAddr,
    p_buffer: &mut [u8; MAX_MSG_SIZE],
    p_read_write_timeout_in_secs: f64,
) -> std::io::Result<usize> {
    if p_read_write_timeout_in_secs > 0.0 {
        //try to send data via UDP
        tokio::select! {
            res1 = timeout_for_send(p_read_write_timeout_in_secs) =>
            {
                res1
            },
            res2 = async { p_socket.send_to(p_buffer, peer_addr).await } =>
            {
                res2
            },
        }
    } else {
        p_socket.send_to(p_buffer, peer_addr).await
    }
}

/// # Errors
///
/// TODO: add error description
pub async fn connect(
    p_address: &str,
    p_port: u16,
    p_connection_type: UdpConnectionType,
    p_read_write_timeout_in_secs: f64,
    p_on_bind_socket: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_socket: OnCloseSocketCallback,
) -> Result<()> {
    const TRACE: &str = "udp::connect";

    let address = format!("{p_address}:{p_port}");
    let socket_addr =
        SocketAddr::from_str(&address).map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;

    //no tls-mode
    let socket = UdpSocket::bind(socket_addr)
        .await
        .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;
    // on bind
    p_on_bind_socket.run(&socket_addr)?;

    // don't read more than 1K
    let mut msg = Buffer::new(BufferType::BINARY);
    let mut r_res: std::io::Result<(usize, SocketAddr)>;
    let mut s_res: std::io::Result<usize>;
    let close_msg: String;

    let socket_live_time = Instant::now();

    match p_connection_type {
        UdpConnectionType::UDPServer => {
            // let's loop for read and writing to the socket
            loop {
                let elpased_secs = socket_live_time.elapsed().as_secs_f64();

                r_res = read(&socket, &mut msg.buf, p_read_write_timeout_in_secs).await;
                if r_res.is_err() {
                    close_msg = format!("{r_res:?}");
                    break;
                }

                let (msg_size, peer_addr) =
                    r_res.map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;

                // copy msg size
                msg.size = msg_size;

                let want_to_close_conn = p_on_message.run(elpased_secs, &peer_addr, &mut msg);
                if want_to_close_conn.is_err() {
                    close_msg = format!("udp connection will be closed because of the p_on_msg_callback request. Reason: {want_to_close_conn:?}");
                    break;
                }

                if msg_size > 0 {
                    let s_res = send(
                        &socket,
                        &peer_addr,
                        &mut msg.buf,
                        p_read_write_timeout_in_secs,
                    )
                    .await;
                    if s_res.is_err() {
                        close_msg = format!("{s_res:?}");
                        break;
                    }
                }
            }
        }
        UdpConnectionType::UDPClient => {
            let mut msg_size: usize;
            let peer_addr = socket
                .local_addr()
                .map_err(|e| anyhow!("{:?}. trace info:{}", e, TRACE))?;

            // let's loop for write and reading to the socket
            loop {
                let elpased_secs = socket_live_time.elapsed().as_secs_f64();
                // write buffer
                let want_to_close_conn = p_on_message.run(elpased_secs, &peer_addr, &mut msg);
                if want_to_close_conn.is_err() {
                    close_msg = format!("udp client will be closed because of the p_on_msg_callback request. Reason: {want_to_close_conn:?}");
                    break;
                }
                msg_size = msg.size;

                if msg_size > 0 {
                    s_res = send(
                        &socket,
                        &peer_addr,
                        &mut msg.buf,
                        p_read_write_timeout_in_secs,
                    )
                    .await;
                    if s_res.is_err() {
                        close_msg = format!("{s_res:?}");
                        break;
                    }
                    r_res = read(&socket, &mut msg.buf, p_read_write_timeout_in_secs).await;
                    if r_res.is_err() {
                        close_msg = format!("{r_res:?}");
                        break;
                    }
                }
            }
        }
    };

    // on close socket
    p_on_close_socket.run(&socket_addr, &close_msg)
}

#[tokio::main]
#[test]
async fn test() {
    /*
    #test it with this python code
    import socket

    HOST = '127.0.0.1'  # The server's hostname or IP address
    PORT = 8000        # The port used by the server

    # Create a UDP socket at client side
    udp = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    # Send to server using created UDP socket
    udp.sendto(str.encode("Hello UDP Server"), (HOST, PORT))
    msg = udp.recvfrom(1024)
    print('Received', msg.decode("utf-8"))
    */

    // block main thread with udp server
    let on_bind_socket = OnSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
            println!("udp socket {p_socket_address:?} just binded");
            Ok(())
        },
    ));
    let on_close_socket = OnCloseSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr, p_close_msg: &str| -> anyhow::Result<()> {
            println!("udp socket {p_socket_address:?} just closed, because: {p_close_msg}");
            Ok(())
        },
    ));

    let on_msg_callback = OnMessageCallback::new(Box::new(
        |p_socket_time_in_secs: f64,
         p_peer_address: &SocketAddr,
         p_msg: &mut crate::stream::common::buffer::Buffer|
         -> anyhow::Result<()> {
            println!(
                "client: number of received byte(s) from {p_peer_address:?} is {}. socket live time {p_socket_time_in_secs}",
                p_msg.size
            );

            if p_msg.size > 0 {
                let msg = std::str::from_utf8(p_msg.buf.as_slice())?;
                println!("client: received buffer is {msg}");
            }
            //now store new bytes for write
            let msg = "hello...world!\0"; //make sure append NULL terminate
            p_msg.buf[0..msg.len()].copy_from_slice(msg.as_bytes());
            p_msg.size = msg.len();

            if p_socket_time_in_secs > 10.0 {
                anyhow::bail!("closing socket");
            }
            Ok(())
        },
    ));
    let ret = connect(
        "0.0.0.0",
        8000,
        UdpConnectionType::UDPServer,
        3.0,
        on_bind_socket,
        on_msg_callback,
        on_close_socket,
    )
    .await;
    assert!(ret.is_ok(), "{ret:?}");

    println!("udp tests done");
}
