#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

pub mod media;
//#[cfg(feature = "render")]
//pub mod render;
pub mod error;
pub mod stream;
pub mod system;

/// # Safety
///
/// unsafe function for C ABI
#[cfg(feature = "ffi")]
#[no_mangle]
pub unsafe extern "C" fn w_version(p_major: *mut i32, p_minor: *mut i32, p_patch: *mut i32) {
    std::ptr::write(p_major, 3);
    std::ptr::write(p_minor, 1);
    std::ptr::write(p_patch, 0);
}
