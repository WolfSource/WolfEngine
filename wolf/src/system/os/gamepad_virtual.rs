use crate::system::ffi::vigem_client::{
    vigem_target_add, vigem_target_free, vigem_target_remove, vigem_target_x360_alloc,
    vigem_target_x360_update, PVIGEM_CLIENT, PVIGEM_TARGET, VIGEM_ERROR, XUSB_REPORT,
};
use parking_lot::RwLock;
use std::sync::Arc;
use windows_sys::Win32::UI::Input::XboxController::XINPUT_STATE;

#[derive(Clone, Debug)]
pub struct GamePadVirtual {
    client: Arc<RwLock<PVIGEM_CLIENT>>,
    target: PVIGEM_TARGET,
}

impl Drop for GamePadVirtual {
    fn drop(&mut self) {
        if !self.client.read().is_null() {
            unsafe {
                vigem_target_remove(*self.client.read(), self.target);
                vigem_target_free(self.target);
            }
        };
    }
}

impl GamePadVirtual {
    /// Create a `ViGEm` virtual game
    /// # Errors
    ///
    /// `VIGEM_ERROR` from `vigem_client`
    pub fn new(p_client: Arc<RwLock<PVIGEM_CLIENT>>) -> Result<Self, VIGEM_ERROR> {
        let target_opt = p_client.try_read().map(|locked_client| {
            // allocate target handle to identify new pad
            let target = unsafe { vigem_target_x360_alloc() };
            if target.is_null() {
                return Err(VIGEM_ERROR::VIGEM_ERROR_INVALID_TARGET);
            }
            // add client to the bus, this equals a plug-in event
            let res = unsafe { vigem_target_add(*locked_client, target) };
            if res != VIGEM_ERROR::VIGEM_ERROR_NONE {
                return Err(res);
            }
            Ok(target)
        });

        match target_opt {
            Some(target_res) => {
                let target = target_res?;
                Ok(Self {
                    client: p_client,
                    target,
                })
            }
            None => Err(VIGEM_ERROR::VIGEM_ERROR_BUS_ACCESS_FAILED),
        }
    }

    /// Clear state of virtual gamepad
    /// # Errors
    ///
    /// `VIGEM_ERROR` from `vigem_client`
    pub fn clear_state(&mut self) -> Result<(), VIGEM_ERROR> {
        // try lock the bus
        let opt_ret = self.client.try_read().map(|p_locked_client| {
            // reset x360 gamepad
            unsafe {
                let xusb: XUSB_REPORT = std::mem::zeroed();
                let error = vigem_target_x360_update(*p_locked_client, self.target, xusb);
                if error == VIGEM_ERROR::VIGEM_ERROR_NONE {
                    return Ok(());
                }
                Err(error)
            }
        });
        opt_ret.map_or(Err(VIGEM_ERROR::VIGEM_ERROR_BUS_ACCESS_FAILED), |res| res)
    }

    /// Send an input to the virtual gamepad
    /// # Arguments
    ///
    /// * `p_input` - the input state of gamepad
    ///
    /// # Errors
    ///
    /// `VIGEM_ERROR` from `vigem_client`
    pub fn send_input(&mut self, p_input: XINPUT_STATE) -> Result<(), VIGEM_ERROR> {
        // try lock the bus
        let opt_ret = self.client.try_read().map(|p_locked_client| {
            // reset x360 gamepad
            unsafe {
                let xusb: XUSB_REPORT = std::mem::transmute(p_input.Gamepad);
                let error = vigem_target_x360_update(*p_locked_client, self.target, xusb);
                if error == VIGEM_ERROR::VIGEM_ERROR_NONE {
                    return Ok(());
                }
                Err(error)
            }
        });
        opt_ret.map_or(Err(VIGEM_ERROR::VIGEM_ERROR_BUS_ACCESS_FAILED), |res| res)
    }
}
