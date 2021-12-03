#[cxx::bridge(namespace = "lzma_cxx")]
pub mod ffi {
    unsafe extern "C++" {
        include!("wolf_system/src/compression/cxx/lzma/lzma.hpp");

        pub fn compress_1(pSourceBuffer: &[u8], pLevel: u32, pTrace: &mut String) -> Vec<u8>;
        pub fn decompress_1(pSourceBuffer: &[u8], pTrace: &mut String) -> Vec<u8>;

        pub fn compress_2(pSourceBuffer: &[u8], pLevel: u32, pTrace: &mut String) -> Vec<u8>;
        pub fn decompress_2(pSourceBuffer: &[u8], pTrace: &mut String) -> Vec<u8>;
    }
}

#[test]
fn test() {
    use crate::compression::lzma;

    let content = "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!".as_bytes();
    println!(
        "original memory is {:?} with size {}",
        std::str::from_utf8(content),
        content.len(),
    );

    let mut trace = String::new();
    let compressed = lzma::ffi::compress_1(content, 5, &mut trace);
    println!(
        "lzma compressed memory is {:?} with size {}. trace info: {:?}",
        compressed,
        compressed.len(),
        trace
    );

    let decompressed = lzma::ffi::decompress_1(compressed.as_slice(), &mut trace);
    println!(
        "lzma de-compressed memory is {:?} with size {}. trace info: {:?}",
        std::str::from_utf8(decompressed.as_slice()),
        decompressed.len(),
        trace
    );

    let mut trace2 = String::new();
    let compressed2 = lzma::ffi::compress_2(content, 5, &mut trace2);
    println!(
        "lzma2 compressed memory is {:?} with size {}. trace info: {:?}",
        compressed2,
        compressed2.len(),
        trace2
    );

    let decompressed2 = lzma::ffi::decompress_2(compressed2.as_slice(), &mut trace2);
    println!(
        "lzma2 de-compressed memory is {:?}with size {}. trace info: {:?}",
        std::str::from_utf8(decompressed2.as_slice()),
        decompressed2.len(),
        trace2
    );
}
