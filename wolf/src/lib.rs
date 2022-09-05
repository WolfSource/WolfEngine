#![allow(trivial_casts)]
#![allow(unused_crate_dependencies)]

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

#[allow(dead_code)]
const MAX_TRACE_BUFFER_SIZE: usize = 256;

pub mod media;
#[cfg(feature = "render")]
pub mod render;
pub mod stream;
pub mod system;
pub mod quic;

#[cfg(not(target_arch = "wasm32"))]
#[must_use]
pub fn sys_init() -> String {
    use crate::system::ffi::sys_init::{size_t, w_sys_init};

    // create a buffer
    let mut buf = [0i8; 32];
    let buf_ptr = buf.as_mut_ptr();

    let current_exe_path = std::env::current_exe().unwrap_or_default();
    let current_exe_str = current_exe_path.to_str().unwrap_or_default();
    // call unsafe function
    let c_str = unsafe {
        w_sys_init(
            current_exe_str.as_ptr() as *const std::os::raw::c_char,
            buf_ptr,
            buf.len() as size_t,
        );
        std::ffi::CStr::from_ptr(buf_ptr)
    };

    // return result
    let str = c_str.to_str().unwrap_or_default();
    String::from(str)
}
