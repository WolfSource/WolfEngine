use std::str::Utf8Error;

#[derive(Debug, thiserror::Error)]
pub enum WError {
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not parse the address '{0}'")]
    SystemSocketAddressParseFailed(std::net::AddrParseError),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("socket could not connect to the address '{0}'")]
    SystemSocketConnectFailed(std::io::Error),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not bind to the socket '{0}'")]
    SystemSocketBindFailed(std::io::Error),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not create boring SSL builder '{0}'")]
    SystemSocketSSLBuilderFailed(boring::error::ErrorStack),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not acceot ssl socket stream '{0}'")]
    SystemSocketSSLAcceptFailed(tokio_boring::HandshakeError<tokio::net::TcpStream>),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not connect to the ssl socket stream")]
    SystemSocketSSLConnectFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not read from the tcp socket")]
    SystemTCPSocketReadFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not write to the tcp socket")]
    SystemTCPSocketWriteFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not read from the udp socket")]
    SystemUDPSocketReadFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not write to the udp socket")]
    SystemUDPSocketWriteFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not connect to the server after '{0}' seconds")]
    SystemSocketConnectTimeout(f64),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not handshake to the server after '{0}' seconds")]
    SystemSocketSSLHandshakeTimeout(f64),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not load local address '{0}'")]
    SystemSocketLoadLocalAddressFailed(std::io::Error),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not convert from UTF8 '{0}'")]
    SystemSocketUtf8Error(Utf8Error),
    #[error("unknown error")]
    Unknown,
}
