#![allow(unused_crate_dependencies)]

#[cfg(feature = "system_lz4")]
#[test]
fn test_system_compression_lz4() {
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
        let mut ret = lz4::compress(
            &mut compress_buffer,
            mock_compression_data.as_bytes(),
            mode,
            5,
        );
        assert!(ret.is_ok(), "lz4 compress failed");
        println!(
            "compressd buffer is {:?} with size of {} bytes and mode: {:?}",
            compress_buffer,
            compress_buffer.len(),
            mode
        );

        let mut decompress_buffer = Vec::new();
        ret = lz4::de_compress(&mut decompress_buffer, compress_buffer.as_slice());
        assert!(ret.is_ok(), "lz4 compress failed");
        println!(
            "decompressd buffer is {:?} with size of {} bytes",
            std::str::from_utf8(decompress_buffer.as_slice()),
            decompress_buffer.len(),
        );
    }
}

#[test]
fn test_system_compression_lzma() {
    use wolf::system::compression::lzma::{compress, decompress, LZMAType};

    let compress_level = 9;
    let content = "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!".as_bytes();
    println!(
        "original memory is {:?} with size {}",
        std::str::from_utf8(content),
        content.len(),
    );

    let mut now = std::time::Instant::now();
    let compress_1 = compress(content, LZMAType::LZMA1, compress_level).unwrap();
    let mut duration = std::time::Instant::now() - now;
    println!(
        "lzma compress has done in {} sec. Memory is {:?} {} bytes",
        duration.as_secs_f64(),
        compress_1,
        compress_1.len(),
    );

    now = std::time::Instant::now();
    let decompress_1 = decompress(compress_1.as_slice(), LZMAType::LZMA1).unwrap();
    duration = std::time::Instant::now() - now;
    println!(
        "lzma de-compress has done in {} sec. Memory is {:?} {} bytes",
        duration.as_secs_f64(),
        std::str::from_utf8(decompress_1.as_slice()),
        decompress_1.len(),
    );

    now = std::time::Instant::now();
    let compress_2 = compress(content, LZMAType::LZMA2, compress_level).unwrap();
    duration = std::time::Instant::now() - now;
    println!(
        "lzma2 compress has done in {}. Memory is {:?} {} bytes",
        duration.as_secs_f64(),
        compress_2,
        compress_2.len(),
    );

    now = std::time::Instant::now();
    let decompress_2 = decompress(compress_2.as_slice(), LZMAType::LZMA2).unwrap();
    duration = std::time::Instant::now() - now;
    println!(
        "lzma2 de-compress has done in {}. Memory is {:?} {} bytes",
        duration.as_secs_f64(),
        std::str::from_utf8(decompress_2.as_slice()),
        decompress_2.len(),
    );
}
