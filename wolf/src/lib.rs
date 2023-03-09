#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

pub mod media;
//#[cfg(feature = "render")]
//pub mod render;
pub mod error;
pub mod stream;
pub mod system;
