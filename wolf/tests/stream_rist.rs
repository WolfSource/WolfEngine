#![allow(unused_crate_dependencies)]

#[cfg(feature = "stream_rist")]
use {
    std::os::raw::{c_char, c_int, c_void},
    wolf::{
        stream::rist::{rist, rist_ctx_mode, rist_data_block, rist_log_level, rist_profile},
        w_log,
    },
};

#[cfg(feature = "stream_rist")]
extern "C" fn log_callback(
    p_arg: *mut c_void,
    p_level: rist_log_level,
    p_msg: *const c_char,
) -> c_int {
    let c_str = unsafe { std::ffi::CStr::from_ptr(p_msg) };
    w_log!("{:?} {:?} with args {:?}", p_level, c_str, p_arg);
    0
}

#[cfg(feature = "stream_rist")]
#[test]
fn test_send_rist() {
    println!("wolf_sys version is : {:?}", wolf::sys_version());

    let url = "rist://127.0.0.1:1234?rtt-max=10&rtt-min=3";
    let rist_ret = rist::new(
        url,
        rist_ctx_mode::RIST_SENDER_MODE,
        rist_profile::RIST_PROFILE_MAIN,
        0u16,
        rist_log_level::RIST_LOG_DEBUG,
        log_callback,
    );
    assert!(rist_ret.is_ok());

    let sender = rist_ret.unwrap();
    let mock_data = "HELLO WOLF!";

    loop {
        let mut data_block = rist_data_block::new().unwrap();
        data_block.set(mock_data.as_bytes());

        let sent = sender.send(data_block);
        println!("{} bytes just sent to the rist stream", sent);
        if sent as usize != mock_data.len() {
            eprintln!("Error: breaking from rist sender loop");
            break;
        }
        std::thread::sleep(std::time::Duration::from_millis(1));
    }
}

#[cfg(feature = "stream_rist")]
#[test]
fn test_receive_rist() {
    println!("wolf_sys version is : {:?}", wolf::sys_version());

    let url = "rist://@127.0.0.1:1234?rtt-max=10&rtt-min=3";
    let rist_ret = rist::new(
        url,
        rist_ctx_mode::RIST_RECEIVER_MODE,
        rist_profile::RIST_PROFILE_MAIN,
        0u16,
        rist_log_level::RIST_LOG_DEBUG,
        log_callback,
    );
    assert!(rist_ret.is_ok());

    let receiver = rist_ret.unwrap();

    let mut retry: i32 = 10;
    loop {
        let mut data_block = rist_data_block::default();
        let read = receiver.read(&mut data_block, 5);
        if read > 0 {
            println!("{} bytes just received from rist stream", read);
        } else {
            eprintln!("Error: Could not read from rist {}", url);
            retry -= 1;
            if retry < 0 {
                break;
            }
        }
        std::thread::sleep(std::time::Duration::from_millis(100));
    }
}

#[cfg(feature = "stream_rist")]
#[test]
fn test_send_receive_rist() {
    let t_sender_handle = std::thread::spawn(|| {
        test_send_rist();
    });
    let t_receiver_handle = std::thread::spawn(|| {
        //std::thread::sleep(std::time::Duration::from_secs(1));
        test_receive_rist();
    });

    t_sender_handle.join().unwrap();
    t_receiver_handle.join().unwrap();
}
