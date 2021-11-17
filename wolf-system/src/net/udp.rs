use super::callback::{OnMessageCallback, OnSocketCallback};
use crate::chrono::gametime::GameTime;
use std::{net::SocketAddr, str::FromStr};
use tokio::{net::UdpSocket, time::Instant};

const MAX_BUFFER_SIZE: usize = 1024; //1K

pub enum UdpConnectionType {
    UDPServer = 0, // read then write
    UDPClient,     // write then read
}

pub async fn connect(
    p_address: &str,
    p_port: u16,
    p_connection_type: UdpConnectionType,
    p_timeout_in_seconds: f64,
    p_on_bind_socket: OnSocketCallback,
    p_on_message: OnMessageCallback,
    p_on_close_socket: OnSocketCallback,
) -> anyhow::Result<()> {
    let address = format!("{}:{}", p_address, p_port);
    let socket_addr = SocketAddr::from_str(&address)?;

    //no tls-mode
    let socket = UdpSocket::bind(socket_addr).await?;
    // on bind
    p_on_bind_socket.run(&socket_addr)?;

    // don't read more than 1K
    let mut buf = [0u8; MAX_BUFFER_SIZE];
    let mut socket_io_timeout = Instant::now();
    let mut socket_live_time = GameTime::new();
    socket_live_time.set_fixed_time_step(false);

    match p_connection_type {
        UdpConnectionType::UDPServer => {
            // let's loop for read and writing to the socket
            loop {
                socket_live_time.tick();
                // read buffer
                let (mut size, peer_addr) = socket.recv_from(&mut buf).await?;
                if size > 0 {
                    socket_io_timeout = Instant::now();
                    let ret = p_on_message.run(&socket_live_time, &peer_addr, &mut size, &mut buf);
                    if ret.is_err() {
                        break;
                    }
                    if size > 0 {
                        socket.send_to(&buf, peer_addr).await?;
                    }
                } else {
                    let dur = Instant::now() - socket_io_timeout;
                    if dur.as_secs_f64() > p_timeout_in_seconds {
                        break;
                    }
                }
            }
        }
        UdpConnectionType::UDPClient => {
            let mut size: usize = 0;
            let mut peer_addr = socket.local_addr()?;
            // let's loop for write and reading to the socket
            loop {
                socket_live_time.tick();
                // write buffer
                let ret = p_on_message.run(&socket_live_time, &peer_addr, &mut size, &mut buf);
                if ret.is_err() {
                    break;
                }
                if size > 0 {
                    socket.send(&buf).await?;
                }
                let (read_size, addr) = socket.recv_from(&mut buf).await?;
                size = read_size;
                peer_addr = addr;
                if size > 0 {
                    socket_io_timeout = Instant::now();
                } else {
                    let dur = Instant::now() - socket_io_timeout;
                    if dur.as_secs_f64() > p_timeout_in_seconds {
                        break;
                    }
                }
            }
        }
    };

    // on close socket
    let ret = p_on_close_socket.run(&socket_addr);
    ret
}

#[tokio::main]
#[test]
async fn test() -> () {
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
    let on_close_socket = OnSocketCallback::new(Box::new(
        |p_socket_address: &SocketAddr| -> anyhow::Result<()> {
            println!("udp socket {:?} just closed", p_socket_address);
            Ok(())
        },
    ));

    let on_msg_callback = OnMessageCallback::new(Box::new(
        |p_socket_live_time: &GameTime,
         p_peer_address: &SocketAddr,
         p_size: &mut usize,
         p_buffer: &mut [u8]|
         -> anyhow::Result<()> {
            println!(
                "client: number of received byte(s) from {:?} is {}. socket live time {}",
                p_peer_address,
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
