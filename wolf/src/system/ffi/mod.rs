#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(clippy::unreadable_literal)]
#![allow(clippy::use_self)]

#[cfg(all(feature = "system_lz4", not(target_arch = "wasm32")))]
pub mod lz4;

#[cfg(all(feature = "system_lzma", not(target_arch = "wasm32")))]
pub mod lzma;
#[cfg(not(target_arch = "wasm32"))]
pub mod sys_init;

#[cfg(all(feature = "system_gamepad_sim", target_os = "windows"))]
pub mod vigem_client;
