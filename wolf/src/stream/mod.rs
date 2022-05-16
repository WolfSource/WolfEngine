#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

pub mod common;
pub mod ffi;
pub mod http;
#[cfg(all(not(target_arch = "wasm32"), feature = "stream_rist"))]
pub mod rist;
#[cfg(not(target_arch = "wasm32"))]
pub mod tcp;
#[cfg(not(target_arch = "wasm32"))]
pub mod udp;
