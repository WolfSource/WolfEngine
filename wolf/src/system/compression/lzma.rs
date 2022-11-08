use crate::system::ffi::lzma::{w_lzma_compress, w_lzma_decompress};

use std::ptr::null_mut;

//use core::result::Result;
use thiserror::Error;

#[derive(Clone, Debug, Error)]
pub enum LZMAError {
    #[error("Set Properties failed")]
    /// Set properties failed
    SetPropertiesFailed,
    #[error("Invalid header")]
    /// Invalid header
    InvalidHeader,
    #[error("Allocate memory for decoder failed")]
    /// failed on allocating memory for decoder
    AllocateMemoryForDecoderFailed,
    #[error("Decoding to buffer failed")]
    /// failed on decoding data to buffer
    DecodingToBufferFailed,
    /// Unknown LZMA error
    #[error("Unknown LZMA error")]
    Unknown,
}
impl LZMAError {
    const fn from_w_result(p_value: i64) -> Self {
        match p_value {
            -2 => Self::InvalidHeader,
            -3 => Self::SetPropertiesFailed,
            -4 => Self::AllocateMemoryForDecoderFailed,
            -5 => Self::DecodingToBufferFailed,
            _ => Self::Unknown,
        }
    }
}

#[repr(u8)]
#[derive(Debug, Clone)]
pub enum LZMAType {
    LZMA1 = 0,
    LZMA2,
}

/// Compress a buffer via LZMA algorithm
///
/// # Arguments
///
/// * `p_src` - The source buffer of bytes.
/// * `p_type` - The type of LZMA compression.
/// * `pLevel` - The level of compression. ( 0 <= level <= 9)
///
/// # Examples
///
/// ```
/// use wolf::system::compression::lzma;
///
/// let content = "HELLO WOLF!".as_bytes();
/// let compress = lzma::compress(content, lzma::ffi::LZMAType::LZMA1, 5);
///
/// println!("{:?}", compress);
/// ```
pub fn compress(p_src: &[u8], p_type: LZMAType, p_level: u32) -> Result<Vec<u8>, LZMAError> {
    let ret = unsafe {
        let mut dst_ptr: *mut u8 = null_mut();
        let dst_ptr_tp_ptr = &mut dst_ptr;
        let mut dst_size = 0;

        let ret = w_lzma_compress(
            dst_ptr_tp_ptr,
            &mut dst_size,
            p_src.as_ptr(),
            p_src.len(),
            p_type as u8,
            p_level,
        );
        if ret == 0 {
            let vec = std::slice::from_raw_parts(dst_ptr, dst_size).to_vec();
            return Ok(vec);
        }
        Err(LZMAError::from_w_result(ret))
    };
    ret
}

/// Compress a buffer via LZMA1 algorithm
///
/// # Arguments
///
/// * `p_src` - The source buffer of bytes.
/// * `p_type` - The type of LZMA compression.
///
/// # Examples
///
/// ```
/// use wolf_system::compression::lzma;
///
/// let content = [93, 0, 16, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 36, 17, 69, 207, 114, 217, 14, 201, 65, 103, 35, 131, 32, 189, 192];
/// let decompress = lzma::ffi::decompress(content.as_slice(), lzma::ffi::LZMAType::LZMA1);
/// ```
pub fn decompress(p_src: &[u8], p_type: LZMAType) -> Result<Vec<u8>, LZMAError> {
    let ret = unsafe {
        let mut dst_ptr: *mut u8 = null_mut();
        let dst_ptr_tp_ptr = &mut dst_ptr;
        let mut dst_size = 0;

        let ret = w_lzma_decompress(
            dst_ptr_tp_ptr,
            &mut dst_size,
            p_src.as_ptr(),
            p_src.len(),
            p_type as u8,
        );
        if ret == 0 {
            let vec = std::slice::from_raw_parts(dst_ptr, dst_size).to_vec();
            return Ok(vec);
        }
        Err(LZMAError::from_w_result(ret))
    };
    ret
}
