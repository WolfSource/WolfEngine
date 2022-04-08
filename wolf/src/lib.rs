#![allow(clippy::missing_errors_doc)]
#![feature(async_closure)]

#[cfg(not(feature = "wasm"))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

pub mod render;
pub mod system;
