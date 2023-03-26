#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

pub mod media;
//#[cfg(feature = "render")]
//pub mod render;
pub mod error;
pub mod stream;
pub mod system;

const W_MAJOR: i32 = 3;
const W_MINOR: i32 = 1;
const W_PATCH: i32 = 0;

#[must_use]
pub const fn version() -> (i32, i32, i32) {
    (W_MAJOR, W_MINOR, W_PATCH)
}

/// # Safety
///
/// unsafe function for C ABI
#[cfg(feature = "ffi")]
#[no_mangle]
pub unsafe extern "C" fn w_version(p_major: *mut i32, p_minor: *mut i32, p_patch: *mut i32) {
    std::ptr::write(p_major, W_MAJOR);
    std::ptr::write(p_minor, W_MINOR);
    std::ptr::write(p_patch, W_PATCH);
}
