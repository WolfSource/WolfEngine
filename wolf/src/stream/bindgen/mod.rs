#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(clippy::unreadable_literal)]
#![allow(clippy::use_self)]

#[cfg(all(feature = "stream_rist", not(target_arch = "wasm32")))]
pub mod rist;
