//cargo test test_system_gamepad --release -- --exact --nocapture
#[cfg(all(feature = "system_gamepad_sim", target_os = "windows"))]
#[test]
fn test_system_gamepad() {
    use wolf::system::ffi::vigem_client::{xinput_gamepad_t, xinput_state_t};
    use wolf::system::os::gamepad_sim::GamePadSim;
    println!("testing GamePad Simulator");
    for _index in 0..2 {
        let gamepad_res = &GamePadSim::new();
        match gamepad_res {
            Ok(gamepad) => {
                // add gamepad
                gamepad.add().unwrap();
                assert!(gamepad.get_number_of_gamepads() == 1);
                let mut input = xinput_state_t {
                    packet_number: 0,
                    gamepad: xinput_gamepad_t {
                        buttons: 0u16,
                        left_trigger: 0u8,
                        right_trigger: 0u8,
                        thumb_lx: 0i16,
                        thumb_ly: 0i16,
                        thumb_rx: 0i16,
                        thumb_ry: 0i16,
                    },
                };
                gamepad.send_input(0, &mut input).unwrap();
                gamepad.remove(0).unwrap();
                assert!(gamepad.get_number_of_gamepads() == 0);
                gamepad.add().unwrap();
                gamepad.clear_state(0).unwrap();
            }
            Err(e) => {
                println!("error just happened {e:?}");
            }
        }
        println!("object is {gamepad_res:?}");
    }
}
