#![allow(unused_assignments)]
use crate::{
    error::WError,
    system::{
        buffer::{Buffer, BufferType, MAX_BUFFER_SIZE},
        socket::timeouts::{timeout_for_read_write, timeout_for_udp_read},
    },
};
use std::{net::SocketAddr, str::FromStr};
use tokio::{net::UdpSocket, time::Instant};

use super::callback::{OnCloseSocketCallback, OnMessageCallback, OnSocketCallback};

#[derive(Debug)]
pub enum UdpConnectionType {
    Server = 0, // read then write
    Client,     // write then read
}

#[derive(Debug)]
pub struct UdpClient {
    pub address: String,
    pub port: u16,
    pub connection_type: UdpConnectionType,
    pub io_number_of_retry: u32,
    pub io_timeout_in_secs: f64,
}

impl UdpClient {
    #[must_use]
    pub const fn new(p_address: String, p_port: u16, p_connection_type: UdpConnectionType) -> Self {
        Self {
            address: p_address,
            port: p_port,
            connection_type: p_connection_type,
            io_number_of_retry: 5,
            io_timeout_in_secs: 5.0,
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub async fn connect(
        &self,
        p_on_bind_socket: OnSocketCallback,
        p_on_message: OnMessageCallback,
        p_on_close_socket: OnCloseSocketCallback,
    ) -> Result<(), WError> {
        let address = format!("{}:{}", self.address, self.port);
        let socket_addr =
            SocketAddr::from_str(&address).map_err(WError::SystemSocketAddressParseFailed)?;

        //no tls-mode
        let socket = UdpSocket::bind(socket_addr)
            .await
            .map_err(WError::SystemSocketBindFailed)?;
        // on bind
        p_on_bind_socket.run(&socket_addr)?;

        // don't read more than 1K
        let mut msg = Buffer::new(BufferType::BINARY);
        let close_msg: String;

        let socket_live_time = Instant::now();

        match self.connection_type {
            UdpConnectionType::Server => {
                // let's loop for read and writing to the socket
                loop {
                    let elpased_secs = socket_live_time.elapsed().as_secs_f64();
                    let res_read = self.try_read(&socket, &mut msg.buf).await;
                    if res_read.is_err() {
                        close_msg = format!(
                            "while reading data the udp connection {socket_addr:?} will be closed. Reason: {res_read:?}"
                        );
                        break;
                    }

                    let (msg_size, peer_addr) = res_read?;

                    // copy msg size
                    msg.size = msg_size;

                    let want_to_close_conn = p_on_message.run(elpased_secs, &peer_addr, &mut msg);
                    if want_to_close_conn.is_err() {
                        close_msg = format!("udp connection {socket_addr:?} will be closed because of the p_on_msg_callback request. Reason: {want_to_close_conn:?}");
                        break;
                    }

                    if msg_size > 0 {
                        let res_send = self.try_send(&socket, &peer_addr, &mut msg.buf).await;
                        if res_send.is_err() {
                            close_msg = format!(
                                "while sending data the udp connection {socket_addr:?} will be closed. Reason: {res_send:?}"
                            );
                            break;
                        }
                    }
                }
            }
            UdpConnectionType::Client => {
                let mut msg_size: usize;
                let peer_addr = socket
                    .local_addr()
                    .map_err(WError::SystemSocketLoadLocalAddressFailed)?;

                // let's loop for write and reading to the socket
                loop {
                    let elpased_secs = socket_live_time.elapsed().as_secs_f64();
                    // write buffer
                    let want_to_close_conn = p_on_message.run(elpased_secs, &peer_addr, &mut msg);
                    if want_to_close_conn.is_err() {
                        close_msg = format!("udp client connected to {socket_addr:?} will be closed because of the p_on_msg_callback request. Reason: {want_to_close_conn:?}");
                        break;
                    }
                    msg_size = msg.size;

                    if msg_size > 0 {
                        let res_send = self.try_send(&socket, &peer_addr, &mut msg.buf).await;
                        if res_send.is_err() {
                            close_msg = format!(
                                "while sending data the udp connection {socket_addr:?} will be closed. Reason: {res_send:?}"
                            );
                            break;
                        }

                        let res_read = self.try_read(&socket, &mut msg.buf).await;
                        if res_read.is_err() {
                            close_msg = format!(
                                "while reading data udp connection {socket_addr:?} will be closed. Reason: {res_read:?}"
                            );
                            break;
                        }
                        msg.size = res_read?.0;
                    }
                }
            }
        };
        // on close socket
        p_on_close_socket.run(&socket_addr, &close_msg)
    }

    async fn read(
        p_socket: &UdpSocket,
        p_buffer: &mut [u8; MAX_BUFFER_SIZE],
        p_read_write_timeout_in_secs: f64,
    ) -> Result<(usize, SocketAddr), WError> {
        let res = if p_read_write_timeout_in_secs > 0.0 {
            //try to read data from UDP socket
            tokio::select! {
                res1 = timeout_for_udp_read(p_read_write_timeout_in_secs) =>
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

        res.map_err(|_| WError::SystemUDPSocketReadFailed)
    }

    async fn send(
        p_socket: &UdpSocket,
        peer_addr: &SocketAddr,
        p_buffer: &mut [u8; MAX_BUFFER_SIZE],
        p_read_write_timeout_in_secs: f64,
    ) -> Result<usize, WError> {
        let res = if p_read_write_timeout_in_secs > 0.0 {
            //try to send data via UDP
            tokio::select! {
                res1 = timeout_for_read_write(p_read_write_timeout_in_secs) =>
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
        res.map_err(|_| WError::SystemUDPSocketReadFailed)
    }

    async fn try_send(
        &self,
        p_socket: &UdpSocket,
        p_peer_addr: &SocketAddr,
        p_buffer: &mut [u8; MAX_BUFFER_SIZE],
    ) -> Result<usize, WError> {
        let mut res: Result<usize, WError> = Err(WError::Unknown);
        let mut retry = 0;
        while retry < self.io_number_of_retry {
            res = Self::send(p_socket, p_peer_addr, p_buffer, self.io_timeout_in_secs).await;
            if res.is_ok() {
                break;
            }
            retry += 1;
            if retry == self.io_number_of_retry {
                break;
            }
        }
        res
    }

    async fn try_read(
        &self,
        p_socket: &UdpSocket,
        p_buffer: &mut [u8; MAX_BUFFER_SIZE],
    ) -> Result<(usize, SocketAddr), WError> {
        let mut res: Result<(usize, SocketAddr), WError> = Err(WError::Unknown);
        let mut retry = 0;
        while retry < self.io_number_of_retry {
            res = Self::read(p_socket, p_buffer, self.io_timeout_in_secs).await;
            if res.is_ok() {
                break;
            }
            retry += 1;
            if retry == self.io_number_of_retry {
                break;
            }
        }
        res
    }
}
