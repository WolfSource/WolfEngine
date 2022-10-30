use crate::system::ffi::vigem_client::{
    w_vigem_add_gamepad, w_vigem_clear_gamepad_state, w_vigem_client, w_vigem_client_fini,
    w_vigem_client_init, w_vigem_get_number_of_gamepads, w_vigem_remove_gamepad,
    w_vigem_send_input, xinput_state, xinput_state_t,
};
use core::result::Result;
use thiserror::Error;

#[derive(Clone, Debug, Error)]
pub enum ViGEmError {
    #[error("ViGEm bus was not found")]
    /// A compatible bus driver wasn't found on the system
    BusNotFound,
    #[error("ViGEm bust has no free slot")]
    /// All device slots are occupied, no new device can be spawned (0xE0000002)
    NoFreeSlot,
    /// Invalid target for ViGEm (0xE0000003)
    #[error("Invalid target for ViGEm")]
    InvalidTarget,
    /// Failed on remove for ViGEm (0xE0000004)
    #[error("Failed on remove for ViGEm")]
    RemovalFailed,
    /// An attempt has been made to plug in an already connected device (0xE0000005)
    #[error("An attempt has been made to plug in an already connected device")]
    AlreadyConnected,
    /// The target device is not initialized (0xE0000006)
    #[error("The target device is not initialized")]
    TargetUninitialized,
    /// The target device is not plugged in (0xE0000007)
    #[error("The target device is not plugged in")]
    TargetNotPluggedIn,
    /// It's been attempted to communicate with an incompatible driver version (0xE0000008)
    #[error("It's been attempted to communicate with an incompatible driver version")]
    BusVersionMismatch,
    /// Failed on accessing to the ViGEm bus (0xE0000009)
    #[error("Failed on accessing to the ViGEm bus")]
    BusAccessFailed,
    /// the callback function was already registered (0xE0000010)
    #[error("the ViGEm callback function was already registered")]
    CallbackAlreadyRegistered,
    /// the callback function was not found (0xE0000011)
    #[error("the callback function was not found")]
    CallbackNotFound,
    /// Bus was already connected (0xE0000012)
    #[error("Bus was already connected")]
    BusAlreadyConnected,
    /// Bus has invalid handle (0xE0000013)
    #[error("Bus has invalid handle")]
    BusInvalidHandle,
    /// USB user index is out of the range (0xE0000014)
    #[error("USB user index is out of the range")]
    XusbUserindexOutOfRange,
    /// ViGEm invalid parameter (0xE0000015)
    #[error("ViGEm invalid parameter")]
    InvalidParameter,
    /// ViGEm operation not supported (0xE0000016)
    #[error("ViGEm operation not supported")]
    NotSupported,
    /// An unexpected Win32 API error occurred. Call Windows GetLastError() for details (0xE0000017)
    #[error("An unexpected Win32 API error occurred. Call Windows GetLastError() for details")]
    WinapiError,
    /// The specified timeout has been reached (0xE0000018)
    #[error("The specified timeout has been reached for ViGEm client")]
    TimedOut,
    /// Unknown ViGEm error
    #[error("Unknown ViGEm error")]
    Unknown,
}

impl ViGEmError {
    const fn from_w_result(p_value: i64) -> Self {
        match p_value {
            -536_870_911 => Self::BusNotFound,
            -536_870_910 => Self::NoFreeSlot,
            -536_870_909 => Self::InvalidTarget,
            -536_870_908 => Self::RemovalFailed,
            -536_870_907 => Self::AlreadyConnected,
            -536_870_906 => Self::TargetUninitialized,
            -536_870_905 => Self::TargetNotPluggedIn,
            -536_870_904 => Self::BusVersionMismatch,
            -536_870_903 => Self::BusAccessFailed,
            -536_870_896 => Self::CallbackAlreadyRegistered,
            -536_870_895 => Self::CallbackNotFound,
            -536_870_894 => Self::BusAlreadyConnected,
            -536_870_893 => Self::BusInvalidHandle,
            -536_870_892 => Self::XusbUserindexOutOfRange,
            -536_870_891 => Self::InvalidParameter,
            -536_870_890 => Self::NotSupported,
            -536_870_889 => Self::WinapiError,
            -536_870_888 => Self::TimedOut,
            _ => Self::Unknown,
        }
    }
}

#[derive(Clone, Debug)]
pub struct GamePadSim {
    ctx: w_vigem_client,
}

impl Drop for GamePadSim {
    fn drop(&mut self) {
        if !self.ctx.is_null() {
            unsafe { w_vigem_client_fini(&mut self.ctx) };
        }
    }
}

impl GamePadSim {
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new() -> Result<Self, ViGEmError> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
        };
        let ret = unsafe { w_vigem_client_init(&mut obj.ctx) };
        if ret == 0 {
            return Ok(obj);
        }
        Err(ViGEmError::from_w_result(ret))
    }

    /// clear states of gamepad
    /// # Errors
    ///
    /// TODO: add error description
    pub fn clear_state(&self, p_index: usize) -> Result<(), ViGEmError> {
        let ret = unsafe { w_vigem_clear_gamepad_state(self.ctx, p_index) };
        if ret == 0 {
            return Ok(());
        }
        Err(ViGEmError::from_w_result(ret))
    }

    /// add a gamepad
    /// # Errors
    ///
    /// TODO: add error description
    pub fn add(&self) -> Result<usize, ViGEmError> {
        let mut index = 0usize;
        let ret = unsafe { w_vigem_add_gamepad(self.ctx, &mut index) };
        if ret == 0 {
            return Ok(index);
        }
        Err(ViGEmError::from_w_result(ret))
    }

    /// remove gamepad
    /// # Errors
    ///
    /// TODO: add error description
    pub fn remove(&self, p_index: usize) -> Result<(), ViGEmError> {
        let ret = unsafe { w_vigem_remove_gamepad(self.ctx, p_index) };
        if ret == 0 {
            return Ok(());
        }
        Err(ViGEmError::from_w_result(ret))
    }

    /// get number of gamepads from this driver
    /// # Errors
    ///
    /// TODO: add error description
    pub fn send_input(
        &self,
        p_index: usize,
        p_xinput: &mut xinput_state_t,
    ) -> Result<(), ViGEmError> {
        let ret = unsafe { w_vigem_send_input(self.ctx, p_index, p_xinput as xinput_state) };
        if ret == 0 {
            return Ok(());
        }
        Err(ViGEmError::from_w_result(ret))
    }

    /// get number of gamepads from this driver
    /// # Errors
    ///
    // TODO: add error description
    #[must_use]
    pub fn get_number_of_gamepads(&self) -> usize {
        unsafe { w_vigem_get_number_of_gamepads(self.ctx) }
    }
}
