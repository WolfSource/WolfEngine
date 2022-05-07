#[cfg(all(feature = "system_script", target_arch = "wasm32"))]
pub mod javascript;
#[cfg(feature = "system_script")]
pub mod rhai;
