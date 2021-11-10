use mimalloc::MiMalloc;

#[global_allocator]
static GLOBAL: MiMalloc = MiMalloc;

pub mod chrono;
pub mod compression;
pub mod net;
pub mod os;
pub mod script;

#[test]
fn test() {
    // let lz4 = compression::lz4::lz4::new_lz4();
    // println!("p");
}
