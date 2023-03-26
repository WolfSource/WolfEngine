#![allow(unused_crate_dependencies)]

#[cfg(feature = "system_lz4")]
#[test]
fn test_lz4() {
    use wolf::system::compression::lz4::{self, LZ4CompressMode};

    let mock_compression_data = "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!\r\nThe quick brown fox jumps over the lazy dog!";
    println!(
        "The content of the data is `{}` with the size `{}`",
        mock_compression_data,
        mock_compression_data.len(),
    );

    let mut mode = LZ4CompressMode::DEFAULT;
    for i in 0..2 {
        if i == 1 {
            mode = LZ4CompressMode::FAST;
        }

        let mut compress_buffer = Vec::new();
        lz4::compress(
            &mut compress_buffer,
            mock_compression_data.as_bytes(),
            mode,
            5,
        )
        .unwrap();
        println!(
            "compressd buffer is {:?} with size of {} bytes and mode: {:?}",
            compress_buffer,
            compress_buffer.len(),
            mode
        );

        let mut decompress_buffer = Vec::new();
        lz4::de_compress(&mut decompress_buffer, compress_buffer.as_slice()).unwrap();
        println!(
            "decompressd buffer is {:?} with size of {} bytes",
            std::str::from_utf8(decompress_buffer.as_slice()),
            decompress_buffer.len(),
        );
    }
}
