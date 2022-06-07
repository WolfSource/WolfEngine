#![allow(unused_crate_dependencies)]

use wolf::stream::rist::{w_rist_data_block, w_rist_oob_block, w_rist_stats};

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

extern "C" fn rist_connection_status_callback(
    _p_arg: *mut c_void,
    _p_peer: wolf::stream::rist::w_rist_peer,
    p_peer_connection_status: wolf::stream::rist::rist_connection_status,
) -> c_int {
    w_log!("{:?}", p_peer_connection_status);
    0
}

extern "C" fn rist_out_of_band_callback(
    _p_arg: *mut c_void,
    _p_oob_block: w_rist_oob_block,
) -> c_int {
    w_log!("rist_out_of_band_callback");
    0
}

extern "C" fn rist_stats_callback(_p_arg: *mut c_void, _p_stats_container: w_rist_stats) -> c_int {
    w_log!("rist_stats_callback");
    0
}

extern "C" fn receiver_data_callback(
    _p_arg: *const c_void,
    _p_data_block: w_rist_data_block,
) -> c_int {
    w_log!("receiver_data_callback");
    0
}

#[cfg(feature = "stream_rist")]
#[test]
fn test_send_rist() {
    println!("wolf_sys version is : {:?}", wolf::sys_version());

    let mut sender = rist::new(
        rist_ctx_mode::RIST_SENDER_MODE,
        rist_profile::RIST_PROFILE_MAIN,
        0u16,
        rist_log_level::RIST_LOG_DEBUG,
        log_callback,
    )
    .unwrap();

    sender.connect("rist://127.0.0.1:1234").unwrap();

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

    let mut receiver = rist::new(
        rist_ctx_mode::RIST_RECEIVER_MODE,
        rist_profile::RIST_PROFILE_MAIN,
        0u16,
        rist_log_level::RIST_LOG_DEBUG,
        log_callback,
    )
    .unwrap();

    unsafe {
        receiver
            .set_conn_status_callback(std::ptr::null_mut(), rist_connection_status_callback)
            .unwrap();
        receiver
            .set_out_of_band_callback(std::ptr::null_mut(), rist_out_of_band_callback)
            .unwrap();
        receiver
            .set_stats_callback(1000, std::ptr::null_mut(), rist_stats_callback)
            .unwrap();
        receiver
            .set_receiver_data_callback(std::ptr::null_mut(), receiver_data_callback)
            .unwrap();
    }

    receiver.connect("rist://@127.0.0.1:1234").unwrap();
    loop {
        std::thread::sleep(std::time::Duration::from_millis(10));
    }
}

#[cfg(feature = "stream_rist")]
#[test]
fn test_send_receive_rist() {
    let t_sender_handle = std::thread::spawn(|| {
        test_send_rist();
    });
    let t_receiver_handle = std::thread::spawn(|| {
        std::thread::sleep(std::time::Duration::from_secs(1));
        test_receive_rist();
    });

    t_sender_handle.join().unwrap();
    t_receiver_handle.join().unwrap();
}
