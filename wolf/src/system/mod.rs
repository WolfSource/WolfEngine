#[cfg(not(target_arch = "wasm32"))]
pub mod algorithm;

pub mod chrono;
pub mod macros;
// pub mod compression;
// pub mod db;
#[cfg(not(target_arch = "wasm32"))]
pub mod net;
#[cfg(not(target_arch = "wasm32"))]
pub mod os;
//pub mod script;
