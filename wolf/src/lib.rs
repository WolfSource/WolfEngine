// #[cfg(any(target_os = "android", target_os = "ios"))]
// use std::{
//     ffi::{CStr, CString},
//     os::raw::c_char,
// };

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

pub mod render;
pub mod system;

cfg_if::cfg_if! {
    if #[cfg(any(target_os = "android", target_os = "ios"))] {
        #[no_mangle]
        extern "C" fn wolf_init() {
            println!("Wolf initialized!");
        }

        #[no_mangle]
        pub extern "C" fn wolf_fini() {
            println!("Wolf released!");
        }
    }
}
