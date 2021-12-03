#![allow(missing_debug_implementations)]

use anyhow::Result;
use std::{net::SocketAddr, sync::Arc};

#[derive(Debug)]
pub enum MessageType {
    BINARY = 0,
    TEXT,
}

// OnMessageCallback
type Fp1 = Box<
    dyn Fn(&f64, &SocketAddr, &mut MessageType, &mut usize, &mut [u8]) -> Result<()> + Send + Sync,
>;

pub struct OnMessageCallback {
    f: Arc<Fp1>,
}

impl OnMessageCallback {
    pub fn new(f: Fp1) -> Self {
        Self { f: Arc::new(f) }
    }

    pub fn run(
        &self,
        p_socket_time_in_secs: &f64,
        p_peer_address: &SocketAddr,
        p_type_of_msg: &mut MessageType,
        p_size_of_msg: &mut usize,
        p_buf: &mut [u8],
    ) -> Result<()> {
        (self.f)(
            p_socket_time_in_secs,
            p_peer_address,
            p_type_of_msg,
            p_size_of_msg,
            p_buf,
        )
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

// OnCloseSocketCallback
type Fp3 = Box<dyn Fn(&SocketAddr, &str) -> Result<()> + Send + Sync>;

pub struct OnCloseSocketCallback {
    f: Arc<Fp3>,
}

impl OnCloseSocketCallback {
    pub fn new(f: Fp3) -> Self {
        Self { f: Arc::new(f) }
    }

    pub fn run(&self, p_arg1: &SocketAddr, p_arg2: &str) -> Result<()> {
        (self.f)(p_arg1, p_arg2)
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
