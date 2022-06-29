//#![feature(extern_types)]
#![allow(unused_crate_dependencies)]

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

#[allow(dead_code)]
const MAX_TRACE_BUFFER_SIZE: usize = 256;

#[cfg(feature = "render")]
pub mod render;
pub mod stream;
pub mod system;

pub mod media;

#[cfg(not(target_arch = "wasm32"))]
#[must_use]
pub fn sys_version() -> String {
    use crate::system::ffi::version::{size_t, w_sys_version};

    // create a buffer
    let mut buf = [0i8; 32];
    let buf_ptr = buf.as_mut_ptr();

    // call unsafe function
    let c_str = unsafe {
        w_sys_version(buf_ptr, buf.len() as size_t);
        std::ffi::CStr::from_ptr(buf_ptr)
    };

    // return result
    let str = c_str.to_str().unwrap_or_default();
    String::from(str)
}
