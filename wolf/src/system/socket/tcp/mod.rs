#[cfg(feature = "system_socket_client")]
pub mod tcp_client;
#[cfg(feature = "system_socket_server")]
pub mod tcp_server;
#[cfg(all(feature = "system_socket_server", feature = "system_fiber"))]
pub mod tcp_server_fiber;

#[derive(Debug, Copy, Clone)]
pub enum TcpProtocol {
    Native = 0,
    Websocket,
}
