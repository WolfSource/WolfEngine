pub const MAX_MSG_SIZE: usize = 1024; //1K

#[derive(Debug, Copy, Clone)]
pub enum TcpProtocol {
    TcpNative = 0,
    TcpWebsocket,
}
