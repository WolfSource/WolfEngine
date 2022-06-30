#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(clippy::too_many_lines)]
#![allow(clippy::unreadable_literal)]
#![allow(clippy::use_self)]

#[cfg(all(feature = "ffmpeg", not(target_arch = "wasm32")))]
pub mod ffmpeg;
