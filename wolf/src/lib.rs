#![allow(clippy::missing_errors_doc)]
#![feature(async_closure)]
#![feature(fn_traits)]

#[cfg(not(target_arch = "wasm32"))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

pub mod render;
pub mod system;
