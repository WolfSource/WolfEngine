#[cfg(all(not(target_arch = "wasm32"), any(feature = "ffmpeg")))]
pub mod ffmpeg;

#[cfg(all(not(target_arch = "wasm32"), feature = "media_av1"))]
pub mod av1;

#[cfg(all(not(target_arch = "wasm32"), feature = "media_vpx"))]
pub mod vpx;
