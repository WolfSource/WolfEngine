use crate::error::WError;
use std::{net::SocketAddr, str::FromStr};

//const MAX_BUFFER_SIZE: usize = 65535;
const MAX_EVENT_SIZE: usize = 1024; //1K
                                    //const MAX_DATAGRAM_SIZE: usize = 1350;

#[derive(Debug, Default, Clone)]
pub struct Quic {
    pub address: String,
    pub port: u16,
    pub io_number_of_retry: u32,
    pub io_timeout_in_secs: f64,
}

impl Quic {
    #[must_use]
    pub fn new(p_address: &str, p_port: u16) -> Self {
        Self {
            address: p_address.to_owned(),
            port: p_port,
            io_number_of_retry: 5,
            io_timeout_in_secs: 5.0,
        }
    }

    /// connect to the server via QUIC
    /// # Errors
    ///
    /// `WError`
    pub fn connect(&self) -> Result<(), WError> {
        // setup the event loop.
        let poll = mio::Poll::new().map_err(|_e| WError::SystemPollFailed)?;
        let _events = mio::Events::with_capacity(MAX_EVENT_SIZE);

        let peer_addr = SocketAddr::from_str(&self.address)
            .map_err(|_e| WError::SystemSocketAddressParseFailed)?;

        // Bind to INADDR_ANY or IN6ADDR_ANY depending on the IP family of the
        // server address. This is needed on macOS and BSD variants that don't
        // support binding to IN6ADDR_ANY for both v4 and v6.
        let bind_addr = match peer_addr {
            std::net::SocketAddr::V4(_) => "0.0.0.0:0",
            std::net::SocketAddr::V6(_) => "[::]:0",
        };

        let address = bind_addr
            .parse()
            .map_err(|_e| WError::SystemSocketAddressParseFailed)?;

        // Create the UDP socket backing the QUIC connection, and register it with the event loop.
        let mut socket =
            mio::net::UdpSocket::bind(address).map_err(|_e| WError::SystemSocketBindFailed)?;
        poll.registry()
            .register(&mut socket, mio::Token(0), mio::Interest::READABLE)
            .map_err(|_e| WError::SystemPollRegisterFailed)?;

        Ok(())
    }
}
