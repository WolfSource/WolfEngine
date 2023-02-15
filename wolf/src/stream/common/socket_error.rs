#[derive(Debug, thiserror::Error)]
pub enum SocketError {
    #[error("socket is already in use")]
    AlreadyInUse,
}
