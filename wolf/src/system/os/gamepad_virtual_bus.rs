use super::gamepad_virtual::GamePadVirtual;
use crate::system::bindgen::vigem_client::{
    vigem_alloc, vigem_connect, vigem_disconnect, vigem_free, PVIGEM_CLIENT, VIGEM_ERROR,
};
use parking_lot::RwLock;
use std::sync::Arc;

#[derive(Debug)]
pub struct GamePadVirtualBus {
    client: Arc<RwLock<PVIGEM_CLIENT>>,
}

impl Drop for GamePadVirtualBus {
    fn drop(&mut self) {
        if !self.client.read().is_null() {
            unsafe {
                vigem_disconnect(*self.client.read());
                vigem_free(*self.client.read());
            }
        };
    }
}

impl GamePadVirtualBus {
    /// Initialize `ViGEm` bus and make a connection to it
    /// # Errors
    ///
    /// `VIGEM_ERROR` from `vigem_client`
    pub fn new() -> Result<Self, VIGEM_ERROR> {
        let client = unsafe { vigem_alloc() };
        if client.is_null() {
            return Err(VIGEM_ERROR::VIGEM_ERROR_BUS_INVALID_HANDLE);
        }

        let res = unsafe { vigem_connect(client) };
        if res != VIGEM_ERROR::VIGEM_ERROR_NONE {
            unsafe { vigem_free(client) };
            return Err(res);
        }
        Ok(Self {
            client: Arc::new(RwLock::new(client)),
        })
    }

    /// Create a virtual gamepad from `ViGEm` bus
    /// # Errors
    ///
    /// `VIGEM_ERROR` from `vigem_client`
    pub fn create(&self) -> Result<GamePadVirtual, VIGEM_ERROR> {
        GamePadVirtual::new(self.client.clone())
    }
}
