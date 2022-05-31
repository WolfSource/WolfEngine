#![allow(unused_crate_dependencies)]

use std::os::raw::{c_char, c_int, c_void};
use wolf::{
    stream::rist::{rist, rist_log_level, RistMode},
    w_log,
};

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
#[tokio::test]
async fn test_rist() {
    println!("wolf_sys version is : {:?}", wolf::sys_version());

    let r = rist::new(RistMode::RECEIVER, log_callback);
    assert!(r.is_ok());
}
