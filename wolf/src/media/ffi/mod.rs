#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#[cfg(all(not(target_arch = "wasm32"), any(feature = "ffmpeg")))]
pub mod ffmpeg;

#[cfg(all(not(target_arch = "wasm32"), feature = "media_av1"))]
pub mod av1;

#[cfg(all(not(target_arch = "wasm32"), feature = "media_vpx"))]
pub mod vpx;
