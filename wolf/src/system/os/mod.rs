pub mod runtime;
pub mod sigslot;

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
pub mod system;

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
pub mod screen;

//#[cfg(target_os = "windows")]
//pub mod vigem_client;
