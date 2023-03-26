use crate::{
    error::WError,
    system::bindgen::lzma::{CLzmaEncProps, ISzAlloc, ISzAllocPtr, LzmaEncProps_Init, LzmaEncode},
};

unsafe extern "C" fn s_lzma_alloc(
    _p_ptr: ISzAllocPtr,
    p_size: usize,
) -> *mut ::std::os::raw::c_void {
    libc::malloc(p_size)
}

unsafe extern "C" fn s_lzma_free(_p_ptr: ISzAllocPtr, p_address: *mut ::std::os::raw::c_void) {
    libc::free(p_address);
}

const LZMA_PROPS_SIZE: usize = 5;
const LZMA_HEADER_SRC_SIZE: usize = 8;
//const MAX_HEADER_SIZE: usize = 256 * 1024 * 1024;

fn lzma_prop(p_prop: *mut CLzmaEncProps, p_level: i32, p_src_size: u32) {
    const MIN_DIC_SIZE: u32 = 1 << 12;
    const MAX_DIC_SIZE: u32 = 3 << 29;
    const MAX_LEVEL: i32 = 9;
    const FB: i32 = 40;

    unsafe {
        // set up properties
        LzmaEncProps_Init(p_prop);

        if p_src_size <= MIN_DIC_SIZE {
            (*p_prop).dictSize = MIN_DIC_SIZE;
        } else if p_src_size >= MAX_DIC_SIZE {
            (*p_prop).dictSize = MAX_DIC_SIZE;
        } else {
            (*p_prop).dictSize = p_src_size; // smaller dictionary = faster!
        }
        (*p_prop).fb = FB;

        if p_level >= MAX_LEVEL {
            (*p_prop).level = MAX_LEVEL;
        } else {
            (*p_prop).level = p_level;
        }
    }
}

/// Compress data using LZMA1 algorithm
///
/// # Errors
///
/// returns `WError`
pub fn compress_lzma1(p_src: &[u8], p_level: u32) -> Result<Vec<u8>, WError> {
    if p_src.is_empty() {
        return Err(WError::SystemLZMAInvalidSourceSize);
    }

    // set up properties
    let src_usize = p_src.len();
    let level = i32::try_from(p_level).map_err(|_e| WError::IntCastError)?;
    let src_size_u32 = u32::try_from(src_usize).map_err(|_e| WError::IntCastError)?;

    let props: *mut CLzmaEncProps = std::ptr::null_mut();
    lzma_prop(props, level, src_size_u32);

    // allocate some space for the compression output
    // this is way more than necessary in most cases
    let mut output_size_64 = src_usize * 2;
    if output_size_64 < 1024 {
        output_size_64 = 1024;
    }

    let mut tmp = vec![0u8; output_size_64];

    // prepare space for the encoded properties
    let props_encoded = &mut [0u8; LZMA_PROPS_SIZE];
    // allocator & free functions
    let s_alloc_funcs = ISzAlloc {
        Alloc: Some(s_lzma_alloc),
        Free: Some(s_lzma_free),
    };

    let mut props_size = LZMA_PROPS_SIZE;
    let lzma_status = unsafe {
        LzmaEncode(
            tmp.as_mut_ptr(),
            std::ptr::addr_of_mut!(output_size_64),
            p_src.as_ptr(),
            src_usize,
            props,
            props_encoded.as_mut_ptr(),
            std::ptr::addr_of_mut!(props_size),
            0,
            std::ptr::null_mut(),
            &s_alloc_funcs,
            &s_alloc_funcs,
        )
    };

    if lzma_status == 0 {
        let compressed_size = output_size_64 + LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE;

        let mut dst = vec![0u8; compressed_size];

        // we have to generate the LZMA header
        // 5 bytes properties + 8 byte uncompressed size
        dst[0..LZMA_PROPS_SIZE].copy_from_slice(props_encoded);

        for i in 0..LZMA_HEADER_SRC_SIZE {
            let index = LZMA_PROPS_SIZE + i;
            let value = (src_usize >> (i * 8)) & 0xFF;
            let u_value = u8::try_from(value).map_err(|_e| WError::CharCastError)?;
            dst[index] = u_value;
        }

        dst[(LZMA_PROPS_SIZE + LZMA_PROPS_SIZE)..output_size_64].copy_from_slice(&tmp);

        return Ok(dst);
    }
    Err(WError::SystemLZMACompressionFailed)
}

// #[derive(Clone, Debug, thiserror::Error)]
// pub enum LZMAError {
//     #[error("An invalid header found for LZMA")]
//     /// An invalid header found for LZMA
//     InvalidHeader,
//     /// Unknown LZMA error
//     #[error("Unknown LZMA error")]
//     Unknown,
// }

// impl LZMAError {
//     const fn from_w_result(p_value: i64) -> Self {
//         match p_value {
//             -2 => Self::InvalidHeader,
//             _ => Self::Unknown,
//         }
//     }
// }
// #![allow(clippy::expl_impl_clone_on_copy)]
// #![allow(clippy::use_self)]

// #[cxx::bridge(namespace = "lzma_cxx")]
// pub mod ffi {

//     #[repr(u8)]
//     #[derive(Debug, Clone)]
//     enum LZMAType {
//         LZMA1 = 0,
//         LZMA2,
//     }

//     unsafe extern "C++" {
//         include!("wolf_system/src/compression/cxx/lzma/lzma.hpp");

//         /// Compress a buffer via LZMA algorithm
//         ///
//         /// # Arguments
//         ///
//         /// * `p_source_buffer` - The source buffer of bytes.
//         /// * `p_compress_type` - The type of LZMA compression.
//         /// * `pLevel` - The level of compression. ( 0 <= level <= 9)
//         /// * `pTrace` - The trace log which will be provided by the C++ side
//         ///
//         /// # Examples
//         ///
//         /// ```
//         /// use wolf_system::compression::lzma;
//         ///
//         /// let content = "HELLO WOLF!".as_bytes();
//         /// let mut trace = String::new();
//         /// let compress = lzma::ffi::compress(content, lzma::ffi::LZMAType::LZMA1, 5, &mut trace);
//         ///
//         /// println!("{:?}", compress);
//         /// ```
//         pub fn compress(
//             p_source_buffer: &[u8],
//             p_compress_type: LZMAType,
//             p_level: u32,
//             p_trace: &mut String,
//         ) -> Vec<u8>;

//         /// Compress a buffer via LZMA1 algorithm
//         ///
//         /// # Arguments
//         ///
//         /// * `p_source_buffer` - The source buffer of bytes.
//         /// * `p_compress_type` - The type of LZMA compression.
//         /// * `pTrace` - The trace log which will be provided by the C++ side
//         ///
//         /// # Examples
//         ///
//         /// ```
//         /// use wolf_system::compression::lzma;
//         ///
//         /// let content = [93, 0, 16, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 36, 17, 69, 207, 114, 217, 14, 201, 65, 103, 35, 131, 32, 189, 192];
//         /// let mut trace = String::new();
//         /// let decompress = lzma::ffi::decompress(content.as_slice(), lzma::ffi::LZMAType::LZMA1, &mut trace);
//         /// ```
//         pub fn decompress(
//             p_source_buffer: &[u8],
//             p_compress_type: LZMAType,
//             p_trace: &mut String,
//         ) -> Vec<u8>;
//     }
// }

// #[test]
// fn test() {
//     use crate::compression::lzma;

//     let content = "HELLO WOLF\r\nHELLO WOLF!*&%!HELLO WOLF!07*&%!".as_bytes();
//     println!(
//         "original memory is {:?} with size {}",
//         std::str::from_utf8(content),
//         content.len(),
//     );

//     let mut now = std::time::Instant::now();
//     let mut trace = String::new();
//     let compress_1 = lzma::ffi::compress(content, lzma::ffi::LZMAType::LZMA1, 5, &mut trace);
//     let mut duration = std::time::Instant::now() - now;
//     println!(
//         "lzma compressed done in {} sec. Memory is {:?} with size {}. trace info: {:?}",
//         duration.as_secs_f64(),
//         compress_1,
//         compress_1.len(),
//         trace
//     );

//     trace.clear();
//     now = std::time::Instant::now();
//     let decompress_1 = lzma::ffi::decompress(
//         compress_1.as_slice(),
//         lzma::ffi::LZMAType::LZMA1,
//         &mut trace,
//     );
//     duration = std::time::Instant::now() - now;
//     println!(
//         "lzma de-compressed done in {} sec. Memory is {:?} with size {}. trace info: {:?}",
//         duration.as_secs_f64(),
//         std::str::from_utf8(decompress_1.as_slice()),
//         decompress_1.len(),
//         trace
//     );

//     trace.clear();
//     now = std::time::Instant::now();
//     let compress_2 = lzma::ffi::compress(content, lzma::ffi::LZMAType::LZMA2, 5, &mut trace);
//     duration = std::time::Instant::now() - now;
//     println!(
//         "lzma2 compressed done in {}. Memory is {:?} with size {}. trace info: {:?}",
//         duration.as_secs_f64(),
//         compress_2,
//         compress_2.len(),
//         trace
//     );

//     trace.clear();
//     now = std::time::Instant::now();
//     let decompress_2 = lzma::ffi::decompress(
//         compress_2.as_slice(),
//         lzma::ffi::LZMAType::LZMA2,
//         &mut trace,
//     );
//     duration = std::time::Instant::now() - now;
//     println!(
//         "lzma2 de-compressed done in {}. Memory is {:?}with size {}. trace info: {:?}",
//         duration.as_secs_f64(),
//         std::str::from_utf8(decompress_2.as_slice()),
//         decompress_2.len(),
//         trace
//     );
// }
