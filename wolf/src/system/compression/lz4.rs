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
