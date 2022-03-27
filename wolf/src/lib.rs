//TODO: remove this on refactor
#![allow(clippy::missing_errors_doc)]
// #![allow(unused_crate_dependencies)]

#[cfg(not(target_arch = "wasm32"))]
#[global_allocator]
static GLOBAL: mimalloc::MiMalloc = mimalloc::MiMalloc;

pub mod render;
pub mod system;
