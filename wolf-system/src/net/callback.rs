use crate::chrono::gametime::GameTime;
use anyhow::Result;
use std::{net::SocketAddr, sync::Arc};

// OnMessageCallback
type Fp1 = Box<dyn Fn(&GameTime, &SocketAddr, &mut usize, &mut [u8]) -> Result<()> + Send + Sync>;

pub struct OnMessageCallback {
    f: Arc<Fp1>,
}

impl OnMessageCallback {
    pub fn new(f: Fp1) -> Self {
        Self { f: Arc::new(f) }
    }

    pub fn run(
        &self,
        p_socket_live_time: &GameTime,
        p_peer_address: &SocketAddr,
        p_size_of_msg: &mut usize,
        p_buf: &mut [u8],
    ) -> Result<()> {
        (self.f)(p_socket_live_time, p_peer_address, p_size_of_msg, p_buf)
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
type Fp2 = Box<dyn Fn(&SocketAddr) -> Result<()> + Send + Sync>;

pub struct OnSocketCallback {
    f: Arc<Fp2>,
}

impl OnSocketCallback {
    pub fn new(f: Fp2) -> Self {
        Self { f: Arc::new(f) }
    }

    pub fn run(&self, p_arg: &SocketAddr) -> Result<()> {
        (self.f)(p_arg)
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
