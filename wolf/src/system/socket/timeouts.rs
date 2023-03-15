use instant::Duration;
use std::io::{Error, ErrorKind};
use std::net::SocketAddr;

use crate::error::WError;

#[cfg(feature = "system_ws_server")]
pub async fn timeout_for_read_ws(
    p_timeout_in_secs: f64,
) -> Option<Result<tokio_tungstenite::tungstenite::Message, tokio_tungstenite::tungstenite::Error>>
{
    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    None
}

/// do write with timeout
/// # Errors
///
/// `std::io::error::ErrorKind`
pub async fn timeout_for_read_write(p_timeout_in_secs: f64) -> std::io::Result<usize> {
    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    //return Error
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout reached while writing to the socket",
    ))
}

/// do write with timeout
/// # Errors
///
/// `std::io::error::ErrorKind`
pub async fn timeout_for_udp_read(p_timeout_in_secs: f64) -> std::io::Result<(usize, SocketAddr)> {
    tokio::time::sleep(std::time::Duration::from_secs_f64(p_timeout_in_secs)).await;
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout reached for udp read data from socket",
    ))
}

/// do accept with timeout
/// # Errors
///
/// `std::io::error::ErrorKind`
pub async fn timeout_for_accept(
    p_timeout_in_secs: f64,
) -> std::io::Result<(tokio::net::TcpStream, SocketAddr)> {
    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    //return Error
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout reached while accepting the socket",
    ))
}

/// connect with timeout
/// # Errors
///
/// `std::io::error::ErrorKind`
pub async fn timeout_for_connect(p_timeout_in_secs: f64) -> std::io::Result<tokio::net::TcpStream> {
    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    //return Error
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout reached while connecting to the socket",
    ))
}

/// ssl connect with timeout
/// # Errors
///
/// `std::io::error::ErrorKind`
pub async fn timeout_for_ssl_connect<S>(
    p_timeout_in_secs: f64,
) -> Result<tokio_boring::SslStream<S>, WError>
where
    S: tokio::io::AsyncRead + tokio::io::AsyncWrite + Unpin,
{
    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    Err(WError::SystemSocketSSLHandshakeTimeout)
}
