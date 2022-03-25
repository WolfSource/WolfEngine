use crate::net::callback::MessageType;

use super::callback::{OnCloseSocketCallback, OnMessageCallback, OnSocketCallback};
use std::{net::SocketAddr, str::FromStr};
use tokio::{net::UdpSocket, time::Instant};

const MAX_BUFFER_SIZE: usize = 1024; //1K

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
    p_buffer: &mut [u8; MAX_BUFFER_SIZE],
    p_read_write_timeout_in_secs: f64,
) -> std::io::Result<(usize, SocketAddr)> {
    let res = if p_read_write_timeout_in_secs > 0.0 {
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
    };
    res
}

async fn send(
    p_socket: &UdpSocket,
    peer_addr: &SocketAddr,
    p_buffer: &mut [u8; MAX_BUFFER_SIZE],
    p_read_write_timeout_in_secs: f64,
) -> std::io::Result<usize> {
    let res = if p_read_write_timeout_in_secs > 0.0 {
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
    };
    res
}

pub async fn connect(
    p_address: &str,
    p_port: u16,
    p_connection_type: UdpConnectionType,
    p_read_write_timeout_in_secs: f64,
    p_on_bind_socket: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_socket: OnCloseSocketCallback,
) -> anyhow::Result<()> {
    let address = format!("{}:{}", p_address, p_port);
    let socket_addr = SocketAddr::from_str(&address)?;

    //no tls-mode
    let socket = UdpSocket::bind(socket_addr).await?;
    // on bind
    p_on_bind_socket.run(&socket_addr)?;

    // don't read more than 1K
    let mut msg_type = MessageType::BINARY;
    let mut msg_buf = [0_u8; MAX_BUFFER_SIZE];
    let mut r_res: std::io::Result<(usize, SocketAddr)>;
    let mut s_res: std::io::Result<usize>;
    let close_msg: String;

    let socket_live_time = Instant::now();

    match p_connection_type {
        UdpConnectionType::UDPServer => {
            // let's loop for read and writing to the socket
            loop {
                let elpased_secs = socket_live_time.elapsed().as_secs_f64();

                r_res = read(&socket, &mut msg_buf, p_read_write_timeout_in_secs).await;
                if r_res.is_err() {
                    close_msg = format!("{:?}", r_res);
                    break;
                }
                let (mut msg_size, peer_addr) = r_res?;
                let want_to_close_conn = p_on_message.run(
                    elpased_secs,
                    &peer_addr,
                    &mut msg_type,
                    &mut msg_size,
                    &mut msg_buf,
                );
                if want_to_close_conn.is_err() {
                    close_msg = format!(
                        "udp connection will be closed because of the p_on_msg_callback request. Reason: {:?}",
                        want_to_close_conn
                    );
                    break;
                }

                if msg_size > 0 {
                    let s_res = send(
                        &socket,
                        &peer_addr,
                        &mut msg_buf,
                        p_read_write_timeout_in_secs,
                    )
                    .await;
                    if s_res.is_err() {
                        close_msg = format!("{:?}", s_res);
                        break;
                    }
                }
            }
        }
        UdpConnectionType::UDPClient => {
            let mut msg_size: usize = 0;
            let peer_addr = socket.local_addr()?;
            // let's loop for write and reading to the socket
            loop {
                let elpased_secs = socket_live_time.elapsed().as_secs_f64();
                // write buffer
                let want_to_close_conn = p_on_message.run(
                    elpased_secs,
                    &peer_addr,
                    &mut msg_type,
                    &mut msg_size,
                    &mut msg_buf,
                );
                if want_to_close_conn.is_err() {
                    close_msg = format!(
                        "udp client will be closed because of the p_on_msg_callback request. Reason: {:?}",
                        want_to_close_conn
                    );
                    break;
                }
                if msg_size > 0 {
                    s_res = send(
                        &socket,
                        &peer_addr,
                        &mut msg_buf,
                        p_read_write_timeout_in_secs,
                    )
                    .await;
                    if s_res.is_err() {
                        close_msg = format!("{:?}", s_res);
                        break;
                    }
                    r_res = read(&socket, &mut msg_buf, p_read_write_timeout_in_secs).await;
                    if r_res.is_err() {
                        close_msg = format!("{:?}", r_res);
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
            println!("udp socket {:?} just binded", p_socket_address);
            Ok(())
        },
    ));
    let on_close_socket = OnCloseSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr, p_close_msg: &str| -> anyhow::Result<()> {
            println!(
                "udp socket {:?} just closed, because: {}",
                p_socket_address, p_close_msg
            );
            Ok(())
        },
    ));

    let on_msg_callback = OnMessageCallback::new(Box::new(
        |p_socket_time_in_secs: f64,
         p_peer_address: &SocketAddr,
         _p_msg_type: &mut MessageType,
         p_msg_size: &mut usize,
         p_msg_buffer: &mut [u8]|
         -> anyhow::Result<()> {
            println!(
                "client: number of received byte(s) from {:?} is {}. socket live time {}",
                p_peer_address, *p_msg_size, p_socket_time_in_secs
            );

            if *p_msg_size > 0 {
                let msg = std::str::from_utf8(p_msg_buffer)?;
                println!("client: received buffer is {}", msg);
            }
            //now store new bytes for write
            let msg = "hello...world!\0"; //make sure append NULL terminate
            p_msg_buffer[0..msg.len()].copy_from_slice(msg.as_bytes());
            *p_msg_size = msg.len();

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
    assert!(ret.is_ok(), "{:?}", ret);

    println!("udp tests done");
}
