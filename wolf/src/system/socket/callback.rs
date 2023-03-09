use crate::{error::WError, system::buffer::Buffer};
use std::{net::SocketAddr, sync::Arc};

// OnMessageCallback
type Fp1 = Box<dyn Fn(f64, &SocketAddr, &mut Buffer) -> Result<(), WError> + Send + Sync>;

pub struct OnMessageCallback {
    f: Arc<Fp1>,
}

impl OnMessageCallback {
    #[must_use]
    pub fn new(f: Fp1) -> Self {
        Self { f: Arc::new(f) }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn run(
        &self,
        p_socket_time_in_secs: f64,
        p_peer_address: &SocketAddr,
        p_msg: &mut Buffer,
    ) -> Result<(), WError> {
        (self.f)(p_socket_time_in_secs, p_peer_address, p_msg)
    }
}

impl std::fmt::Debug for OnMessageCallback {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("OnMessageCallback")
            .field(
                "f",
                &"Box<dyn Fn(&f64, &SocketAddr, &mut MessageType, &mut usize, &mut [u8]) -> Result<()> + Send + Sync>".to_string()
            )
            .finish()
    }
}

impl Clone for OnMessageCallback {
    fn clone(&self) -> Self {
        Self { f: self.f.clone() }
    }

    fn clone_from(&mut self, p_source: &Self) {
        self.f = p_source.f.clone();
    }
}

// OnSocketCallback
type Fp2 = Box<dyn Fn(&SocketAddr) -> Result<(), WError> + Send + Sync>;

pub struct OnSocketCallback {
    f: Arc<Fp2>,
}

impl OnSocketCallback {
    #[must_use]
    pub fn new(f: Fp2) -> Self {
        Self { f: Arc::new(f) }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn run(&self, p_arg: &SocketAddr) -> Result<(), WError> {
        (self.f)(p_arg)
    }
}

impl std::fmt::Debug for OnSocketCallback {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("OnSocketCallback")
            .field(
                "f",
                &"Box<dyn Fn(&SocketAddr) -> Result<()> + Send + Sync>".to_string(),
            )
            .finish()
    }
}

impl Clone for OnSocketCallback {
    fn clone(&self) -> Self {
        Self { f: self.f.clone() }
    }
    fn clone_from(&mut self, p_source: &Self) {
        self.f = p_source.f.clone();
    }
}

// OnCloseSocketCallback
type Fp3 = Box<dyn Fn(&SocketAddr, &str) -> Result<(), WError> + Send + Sync>;

pub struct OnCloseSocketCallback {
    f: Arc<Fp3>,
}

impl OnCloseSocketCallback {
    #[must_use]
    pub fn new(f: Fp3) -> Self {
        Self { f: Arc::new(f) }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn run(&self, p_arg1: &SocketAddr, p_arg2: &str) -> Result<(), WError> {
        (self.f)(p_arg1, p_arg2)
    }
}

impl std::fmt::Debug for OnCloseSocketCallback {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("OnCloseSocketCallback")
            .field(
                "f",
                &"Box<dyn Fn(&SocketAddr, &str) -> Result<()> + Send + Sync>".to_string(),
            )
            .finish()
    }
}

impl Clone for OnCloseSocketCallback {
    fn clone(&self) -> Self {
        Self { f: self.f.clone() }
    }
    fn clone_from(&mut self, p_source: &Self) {
        self.f = p_source.f.clone();
    }
}
