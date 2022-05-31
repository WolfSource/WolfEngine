#[cfg(all(feature = "system_lz4", not(target_arch = "wasm32")))]
pub mod lz4;

#[cfg(all(feature = "system_lzma", not(target_arch = "wasm32")))]
pub mod lzma;
