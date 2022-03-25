//#![allow(trivial_casts)]
#![allow(unused_qualifications)]
#![allow(missing_debug_implementations)]
// #![allow(clippy::future_not_send)]
// #![allow(clippy::ptr_as_ptr)]
#![allow(clippy::use_self)]
#![allow(clippy::expl_impl_clone_on_copy)]

#[cxx::bridge]
pub mod ffi {

    #[repr(u8)]
    #[derive(Debug, Clone)]
    pub enum GamepadType {
        XBox360 = 0,
        PS4,
    }

    pub struct XINPUT_GAMEPAD_SHARED {
        wButtons: u16,
        bLeftTrigger: u8,
        bRightTrigger: u8,
        sThumbLX: i16,
        sThumbLY: i16,
        sThumbRX: i16,
        sThumbRY: i16,
    }

    pub struct XINPUT_STATE_SHARED {
        dwPacketNumber: u32,
        gamepad: XINPUT_GAMEPAD_SHARED,
    }

    unsafe extern "C++" {
        include!("wolf_system/src/os/cxx/vigem_client/vigem_client.hpp");

        pub type vigem_client;

        /// Disconnects from the ViGem bus device and resets the driver object state.
        /// The driver object may be reused again after calling this function.
        /// When called, all targets which may still be connected will be destroyed automatically.
        /// Be aware, that allocated target objects won't be automatically freed,
        /// this has to be taken care of by the caller so make sure drop this class
        ///
        /// Example:
        ///
        /// ```no_run
        /// let mut vc = vigem_client::new();
        /// let success = vc.reset_driver();
        ///
        /// ```
        #[must_use]
        pub fn reset_driver(self: &vigem_client) -> bool;

        /// Allocates an object representing an Xbox 360 or PS4 Controller device.
        ///
        /// Example:
        ///
        /// ```no_run
        /// use ffi::vigem_client;
        /// let mut vc = vigem_client::new();
        /// let index = vc.add_gamepad_controller(vigem_client::GamepadType::XBox360);
        ///
        /// ```
        #[must_use]
        pub fn add_gamepad_controller(self: &vigem_client, pGameType: &GamepadType) -> i32;

        /// Allocates an object representing an Xbox 360 or PS4 Controller device.
        ///
        /// Example:
        ///
        /// ```no_run
        /// use ffi::vigem_client;
        /// let x_input_state = XINPUT_STATE_SHARED();
        /// let mut vc = vigem_client::new();
        /// let success = vc.send_xbox360_input(0, x_input_state);
        ///
        /// ```
        #[must_use]
        pub fn send_xbox360_input(
            self: &vigem_client,
            pGamePadIndex: &u32,
            pXInput: &XINPUT_STATE_SHARED,
        ) -> bool;

        /// Get number of active gamepad(s).
        ///
        /// Example:
        ///
        /// ```no_run
        /// use ffi::vigem_client;
        /// let vc = vigem_client::new();
        /// let num = vc.get_number_of_gamepads();
        ///
        /// ```
        #[must_use]
        pub fn get_number_of_gamepads(self: &vigem_client) -> u32;

        /// Get lat error message
        ///
        /// Example:
        ///
        /// ```no_run
        /// use ffi::vigem_client;
        /// let vc = vigem_client::new();
        /// let error_msg = vc.get_last_error();
        ///
        /// ```
        #[must_use]
        pub fn get_last_error(self: &vigem_client) -> String;

        #[must_use]
        pub fn New() -> UniquePtr<vigem_client>;
    }
}

#[tokio::main]
#[test]
async fn tests() {
    let vigem = ffi::New();
    let _success = vigem.add_gamepad_controller(&ffi::GamepadType::XBox360);
    println!("{}", vigem.get_last_error());
}
