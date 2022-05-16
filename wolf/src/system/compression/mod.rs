#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

#[cfg(all(feature = "system_lz4", not(target_arch = "wasm32")))]
pub mod lz4;

#[cfg(all(feature = "system_lzma", not(target_arch = "wasm32")))]
pub mod lzma;
