#![allow(clippy::expl_impl_clone_on_copy)]
#![allow(clippy::use_self)]

#[cxx::bridge(namespace = "lzma_cxx")]
pub mod ffi {

    #[repr(u8)]
    #[derive(Debug, Clone)]
    enum LZMAType {
        LZMA1 = 0,
        LZMA2,
    }

    unsafe extern "C++" {
        include!("wolf_system/src/compression/cxx/lzma/lzma.hpp");

        /// Compress a buffer via LZMA algorithm
        ///
        /// # Arguments
        ///
        /// * `p_source_buffer` - The source buffer of bytes.
        /// * `p_compress_type` - The type of LZMA compression.
        /// * `pLevel` - The level of compression. ( 0 <= level <= 9)
        /// * `pTrace` - The trace log which will be provided by the C++ side
        ///
        /// # Examples
        ///
        /// ```
        /// use wolf_system::compression::lzma;
        ///
        /// let content = "HELLO WOLF!".as_bytes();
        /// let mut trace = String::new();
        /// let compress = lzma::ffi::compress(content, lzma::ffi::LZMAType::LZMA1, 5, &mut trace);
        ///
        /// println!("{:?}", compress);
        /// ```
        pub fn compress(
            p_source_buffer: &[u8],
            p_compress_type: LZMAType,
            p_level: u32,
            p_trace: &mut String,
        ) -> Vec<u8>;

        /// Compress a buffer via LZMA1 algorithm
        ///
        /// # Arguments
        ///
        /// * `p_source_buffer` - The source buffer of bytes.
        /// * `p_compress_type` - The type of LZMA compression.
        /// * `pTrace` - The trace log which will be provided by the C++ side
        ///
        /// # Examples
        ///
        /// ```
        /// use wolf_system::compression::lzma;
        ///
        /// let content = [93, 0, 16, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 36, 17, 69, 207, 114, 217, 14, 201, 65, 103, 35, 131, 32, 189, 192];
        /// let mut trace = String::new();
        /// let decompress = lzma::ffi::decompress(content.as_slice(), lzma::ffi::LZMAType::LZMA1, &mut trace);
        /// ```
        pub fn decompress(
            p_source_buffer: &[u8],
            p_compress_type: LZMAType,
            p_trace: &mut String,
        ) -> Vec<u8>;
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

    let mut now = std::time::Instant::now();
    let mut trace = String::new();
    let compress_1 = lzma::ffi::compress(content, lzma::ffi::LZMAType::LZMA1, 5, &mut trace);
    let mut duration = std::time::Instant::now() - now;
    println!(
        "lzma compressed done in {} sec. Memory is {:?} with size {}. trace info: {:?}",
        duration.as_secs_f64(),
        compress_1,
        compress_1.len(),
        trace
    );

    trace.clear();
    now = std::time::Instant::now();
    let decompress_1 = lzma::ffi::decompress(
        compress_1.as_slice(),
        lzma::ffi::LZMAType::LZMA1,
        &mut trace,
    );
    duration = std::time::Instant::now() - now;
    println!(
        "lzma de-compressed done in {} sec. Memory is {:?} with size {}. trace info: {:?}",
        duration.as_secs_f64(),
        std::str::from_utf8(decompress_1.as_slice()),
        decompress_1.len(),
        trace
    );

    trace.clear();
    now = std::time::Instant::now();
    let compress_2 = lzma::ffi::compress(content, lzma::ffi::LZMAType::LZMA2, 5, &mut trace);
    duration = std::time::Instant::now() - now;
    println!(
        "lzma2 compressed done in {}. Memory is {:?} with size {}. trace info: {:?}",
        duration.as_secs_f64(),
        compress_2,
        compress_2.len(),
        trace
    );

    trace.clear();
    now = std::time::Instant::now();
    let decompress_2 = lzma::ffi::decompress(
        compress_2.as_slice(),
        lzma::ffi::LZMAType::LZMA2,
        &mut trace,
    );
    duration = std::time::Instant::now() - now;
    println!(
        "lzma2 de-compressed done in {}. Memory is {:?}with size {}. trace info: {:?}",
        duration.as_secs_f64(),
        std::str::from_utf8(decompress_2.as_slice()),
        decompress_2.len(),
        trace
    );
}
