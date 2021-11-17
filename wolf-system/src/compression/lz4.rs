#[cxx::bridge(namespace = "lz4_cxx")]
pub mod ffi {
    unsafe extern "C++" {
        include!("wolf-system/src/compression/cxx/lz4/lz4.hpp");

        pub fn compress(
            pSourceBuffer: &[u8],
            pFastMode: bool,
            pAcceleration: u32, //1..65537
            pTrace: &mut String,
        ) -> Vec<u8>;

        pub fn decompress(pSourceBuffer: &[u8], pTrace: &mut String) -> Vec<u8>;
    }
}

#[test]
fn test() {
    use crate::compression::lz4;

    let content = "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!".as_bytes();
    println!(
        "original memory is {:?} with size {}",
        std::str::from_utf8(content),
        content.len(),
    );

    let mut trace = String::new();
    let compressed = lz4::ffi::compress(content, true, 5, &mut trace);
    println!(
        "lz4 compressed memory is {:?} with size {}. trace info: {:?}",
        compressed,
        compressed.len(),
        trace
    );

    let decompressed = lz4::ffi::decompress(compressed.as_slice(), &mut trace);
    println!(
        "lz4 de-compressed memory is {:?}. trace info: {:?}",
        std::str::from_utf8(decompressed.as_slice()),
        trace
    );
}
