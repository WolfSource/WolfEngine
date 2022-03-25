//TODO: remove this on refactor
#![allow(clippy::missing_errors_doc)]

use mimalloc::MiMalloc;

#[global_allocator]
static GLOBAL: MiMalloc = MiMalloc;

pub mod system;
