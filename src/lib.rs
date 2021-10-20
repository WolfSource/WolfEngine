use mimalloc::MiMalloc;

#[global_allocator]
static GLOBAL: MiMalloc = MiMalloc;

pub mod compression;

#[tokio::test]
async fn test() {
    println!("p");
}
