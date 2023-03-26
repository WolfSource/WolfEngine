use crate::{
    error::WError,
    system::bindgen::lz4::{
        LZ4_compressBound, LZ4_compress_default, LZ4_compress_fast, LZ4_decompress_safe,
    },
};

#[derive(Copy, Clone, Debug)]
pub enum LZ4CompressMode {
    DEFAULT,
    FAST,
}

/// Compress data using LZ4 algorithm
///
/// # Errors
///
/// returns `WError`
pub fn compress(
    p_dst: &mut Vec<u8>,
    p_src: &[u8],
    p_mode: LZ4CompressMode,
    p_acceleration: i32,
) -> Result<(), WError> {
    // allocate size for compressed data
    let src_len = i32::try_from(p_src.len()).unwrap_or(0);
    if src_len == 0 {
        return Err(WError::SystemLZ4InvalidSourceSize);
    }

    let max_dst_size = unsafe { LZ4_compressBound(src_len) };
    if max_dst_size == 0 {
        return Err(WError::SystemLZ4InvalidSourceSize);
    }

    let safe_max_dst_usize = usize::try_from(max_dst_size).unwrap_or(0);
    if safe_max_dst_usize == 0 {
        return Err(WError::SizeCastError);
    }

    // now we are sure that safe_max_dst_size is a signed integer
    p_dst.clear();
    p_dst.resize(safe_max_dst_usize, 0u8);

    let new_size = match p_mode {
        LZ4CompressMode::DEFAULT => unsafe {
            LZ4_compress_default(
                p_src.as_ptr().cast::<i8>(),
                p_dst.as_mut_ptr().cast::<i8>(),
                src_len,
                max_dst_size,
            )
        },
        LZ4CompressMode::FAST => unsafe {
            LZ4_compress_fast(
                p_src.as_ptr().cast::<i8>(),
                p_dst.as_mut_ptr().cast::<i8>(),
                src_len,
                max_dst_size,
                p_acceleration,
            )
        },
    };

    let safe_new_usize = usize::try_from(new_size).unwrap_or(0);
    if safe_new_usize > 0 {
        p_dst.shrink_to(safe_new_usize);
        unsafe { p_dst.set_len(safe_new_usize) };
        return Ok(());
    }
    Err(WError::SizeCastError)
}

/// Decompress data using LZ4 algorithm
///
/// # Errors
///
/// returns `LZ4Error`
pub fn de_compress(p_dst: &mut Vec<u8>, p_src: &[u8]) -> Result<(), WError> {
    // allocate size for decompressed data
    let src_len = i32::try_from(p_src.len()).unwrap_or(0);
    if src_len == 0 {
        return Err(WError::SystemLZ4InvalidSourceSize);
    }
    let mut max_dst_size = src_len * 2;
    let mut ret_size = 0;

    p_dst.clear();
    for _ in 0..10 {
        let max_dst_safe_usize = usize::try_from(max_dst_size).unwrap_or(0);
        if max_dst_safe_usize == 0 {
            break;
        }

        // reserve new size for destination
        p_dst.resize(max_dst_safe_usize, 0u8);
        // try for decompress
        ret_size = unsafe {
            LZ4_decompress_safe(
                p_src.as_ptr().cast::<i8>(),
                p_dst.as_mut_ptr().cast::<i8>(),
                src_len,
                max_dst_size,
            )
        };
        if ret_size > 0 {
            break;
        }
        max_dst_size *= 2;
    }

    let safe_ret_size = usize::try_from(ret_size).unwrap_or(0);
    if safe_ret_size > 0 {
        p_dst.shrink_to(safe_ret_size);
        unsafe { p_dst.set_len(safe_ret_size) };
        return Ok(());
    }
    Err(WError::SizeCastError)
}
