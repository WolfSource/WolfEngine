#[cxx::bridge(namespace = "lz4_cxx")]
pub mod ffi {
    unsafe extern "C++" {
        include!("src/system/compression/cxx/lz4/LZ4.hpp");

        /// Compress a buffer via lz4 algorithm
        ///
        /// # Arguments
        ///
        /// * `pSourceBuffer` - The source buffer of bytes
        /// * `pFastMode` - true means Fast mode and false is default mode
        /// * `pAcceleration` - The acceleration of process. (1 <= value <= 10)
        /// * `pTrace` - The trace log which will be provided by the C++ side
        ///
        /// # Examples
        ///
        /// ```
        /// use wolf_system::compression::lz4;
        ///
        /// let content = "HELLO WOLF!".as_bytes();
        /// let mut trace = String::new();
        /// let compress = lz4::ffi::compress(content, true, 5, &mut trace);
        /// ```
        pub fn compress(
            pSourceBuffer: &[u8],
            pFastMode: bool,
            pAcceleration: u32, //1..10
            pTrace: &mut String,
        ) -> Vec<u8>;

        /// Decompress a buffer via lz4 algorithm
        ///
        /// # Arguments
        ///
        /// * `pSourceBuffer` - The source buffer of compressed bytes
        /// * `pTrace` - The trace log which will be provided with C++ side
        ///
        /// # Examples
        ///
        /// ```
        /// use wolf_system::compression::lz4;
        /// let compressed = [176, 72, 69, 76, 76, 79, 32, 87, 79, 76, 70, 33];
        /// let mut trace = String::new();
        /// let decompress = lz4::ffi::decompress(compressed.as_slice(), &mut trace);
        /// ```
        pub fn decompress(pSourceBuffer: &[u8], pTrace: &mut String) -> Vec<u8>;
    }
}

#[test]
fn test() {
    use crate::system::compression::lz4;

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
