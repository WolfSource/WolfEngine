#![allow(unused_crate_dependencies)]

#[tokio::test]
async fn test_lz4() {
    // TODO: add assert for test
    use wolf::system::compression::lz4;

    let content = "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!".as_bytes();
    println!(
        "original memory is {:?} with size {}",
        std::str::from_utf8(content),
        content.len(),
    );

    let mut now = std::time::Instant::now();
    let mut trace = String::new();
    let compressed = lz4::ffi::compress(content, true, 5, &mut trace);
    let mut duration = std::time::Instant::now() - now;
    println!(
        "lz4 compressed done in {} sec. Memory is {:?} with size: {}. trace info: {:?}",
        duration.as_secs_f64(),
        compressed,
        compressed.len(),
        trace
    );

    now = std::time::Instant::now();
    let decompressed = lz4::ffi::decompress(compressed.as_slice(), &mut trace);
    duration = std::time::Instant::now() - now;
    println!(
        "lz4 decompressed done in {} sec. Memory is {:?}. trace info: {:?}",
        duration.as_secs_f64(),
        std::str::from_utf8(decompressed.as_slice()),
        trace
    );
}
