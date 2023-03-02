//cargo test test_system_gamepad --release -- --exact --nocapture
#[cfg(all(feature = "system_gamepad_virtual", target_os = "windows"))]
#[test]
fn test_system_gamepad_virtual() -> Result<(), wolf::system::bindgen::vigem_client::VIGEM_ERROR> {
    use windows_sys::Win32::UI::Input::XboxController::{
        XINPUT_GAMEPAD, XINPUT_GAMEPAD_DPAD_UP, XINPUT_STATE,
    };
    use wolf::system::os::gamepad_virtual_bus::GamePadVirtualBus;

    let gamepad_virtual_bus = GamePadVirtualBus::new()?;
    for _index in 0..4 {
        let mut gamepad = gamepad_virtual_bus.create()?;
        gamepad.clear_state()?;
        gamepad.send_input(XINPUT_STATE {
            dwPacketNumber: 0,
            Gamepad: XINPUT_GAMEPAD {
                wButtons: XINPUT_GAMEPAD_DPAD_UP,
                bLeftTrigger: 0,
                bRightTrigger: 0,
                sThumbLX: 0,
                sThumbLY: 0,
                sThumbRX: 0,
                sThumbRY: 0,
            },
        })?;
    }
    Ok(())
}
