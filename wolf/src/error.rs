#[derive(Debug, thiserror::Error)]
pub enum WError {
    #[error("invalid parameter")]
    InvalidParameter,
    #[error("could not allocate memory")]
    OutOfMemory,
    #[error("could not cast to int '{0}'")]
    IntCastError(std::num::TryFromIntError),
    #[error("could not cast to usize or isize '{0}'")]
    SizeCastError(std::num::TryFromIntError),
    #[cfg(feature = "media_ffmpeg")]
    #[error("invalid video frame size")]
    MediaInvalidVideoFrameSize,
    #[cfg(feature = "media_ffmpeg")]
    #[error("invalid av config")]
    MediaInvalidAvConfig,
    #[cfg(feature = "media_ffmpeg")]
    #[error("could not fill image")]
    MediaImageFillFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("could not create SWS context")]
    MediaInvalidSwsContext,
    #[cfg(feature = "media_ffmpeg")]
    #[error("could not scale via SWS context")]
    MediaSwsScaleFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("invalid AvSetOption because '{0}'")]
    MediaInvalidAvSetOption(String),
    #[cfg(any(
        feature = "system_socket_client",
        feature = "system_socket_server",
        feature = "stream_quic"
    ))]
    #[error("could not parse the address '{0}'")]
    SystemSocketAddressParseFailed(std::net::AddrParseError),
    #[cfg(any(
        feature = "system_socket_client",
        feature = "system_socket_server",
        feature = "stream_quic"
    ))]
    #[error("could not create poll '{0}'")]
    SystemPollFailed(std::io::Error),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("socket could not connect to the address '{0}'")]
    SystemSocketConnectFailed(std::io::Error),
    #[cfg(any(
        feature = "system_socket_client",
        feature = "system_socket_server",
        feature = "stream_quic"
    ))]
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
    #[cfg(any(
        feature = "system_socket_client",
        feature = "system_socket_server",
        feature = "stream_quic"
    ))]
    #[error("could not register the poll '{0}'")]
    SystemPollRegisterFailed(std::io::Error),
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("could not convert from UTF8 '{0}'")]
    SystemSocketUtf8Error(Utf8Error),
    #[error("unknown error")]
    Unknown,
}
