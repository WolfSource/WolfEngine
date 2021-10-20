#[tokio::test]

async fn test() {
    use super::c_lz4::lz4frame::LZ4F_getVersion;
    let ver = unsafe { LZ4F_getVersion() };
    println!("lz4 version: {}", ver);
}
