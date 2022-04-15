use instant::Duration;

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
use std::net::SocketAddr;

#[cfg(not(target_arch = "wasm32"))]
pub async fn timeout_for_read(p_timeout_in_secs: f64) -> std::io::Result<usize> {
    use std::io::{Error, ErrorKind};

    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout for read from tcp socket reached",
    ))
}

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
pub async fn timeout_for_read_ws(
    p_timeout_in_secs: f64,
) -> Option<Result<tokio_tungstenite::tungstenite::Message, tokio_tungstenite::tungstenite::Error>>
{
    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    None
}

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
pub async fn timeout_for_accept(
    p_timeout_in_secs: f64,
) -> std::io::Result<(tokio::net::TcpStream, SocketAddr)> {
    use std::io::{Error, ErrorKind};

    tokio::time::sleep(Duration::from_secs_f64(p_timeout_in_secs)).await;
    //return Error
    Err(Error::new(
        ErrorKind::TimedOut,
        "timeout for accept tcp connection reached",
    ))
}
