use crate::system::ffi::lz4::{LZ4_compressBound, LZ4_compress_default, LZ4_compress_fast};
use thiserror::Error;

#[derive(Error, Debug)]
pub enum LZ4Error {
    #[error("invalid source size: `{0}`")]
    InvalidSourceSize(i32),
    #[error("invalid destination size: `{0}`")]
    InvalidDestinationSize(i32),
    #[error("could not cast `{0}` i32 to `{1}` usize")]
    USizeCastError(i32, usize),
    #[error("unknown data store error")]
    Unknown,
}

#[derive(Copy, Clone, Debug)]
pub enum LZ4CompressMode {
    DEFAULT,
    FAST,
}

/// # Errors
///
/// TODO: add error description
/// # Panics
///
/// TODO: add panic description
pub fn compress(
    p_dst: &mut Vec<u8>,
    p_src: &[u8],
    p_mode: LZ4CompressMode,
    p_acceleration: i32,
) -> Result<(), LZ4Error> {
    // allocate size for compressed data
    let src_len = i32::try_from(p_src.len()).unwrap_or(0);
    if src_len == 0 {
        return Err(LZ4Error::InvalidSourceSize(src_len));
    }

    let max_dst_size = unsafe { LZ4_compressBound(src_len) };
    if max_dst_size == 0 {
        return Err(LZ4Error::InvalidSourceSize(max_dst_size));
    }

    let max_dst_usize = usize::try_from(max_dst_size).unwrap_or(0);
    if max_dst_usize == 0 {
        return Err(LZ4Error::USizeCastError(max_dst_size, max_dst_usize));
    }

    // now we are sure that safe_max_dst_size is a signed integer
    p_dst.clear();
    p_dst.resize(max_dst_usize, 0u8);

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

    let new_usize = usize::try_from(new_size).unwrap_or(0);
    if new_usize > 0 {
        p_dst.shrink_to(new_usize);
        unsafe { p_dst.set_len(new_usize) };
        return Ok(());
    } else {
        return Err(LZ4Error::USizeCastError(new_size, new_usize));
    }
}

// /// # Errors
// ///
// /// TODO: add error description
// pub fn de_compress(p_dst: &mut Vec<u8>, p_src: &[u8]) -> anyhow::Result<()> {
//     const TRACE: &str = "lz4::decompress";

//     // allocate size for decompressed data
//     let src_len = i32::try_from(p_src.len()).unwrap_or(0);
//     if src_len == 0 {
//         anyhow::bail!("invalid source length. trace info: {}", TRACE)
//     }
//     let mut max_dst_size = src_len * 2;
//     let mut ret_size = 0;

//     p_dst.clear();
//     for _ in 0..10 {
//         let max_dst_safe_usize = usize::try_from(max_dst_size).unwrap_or(0);
//         if max_dst_safe_usize == 0 {
//             break;
//         }

//         // reserve new size for destination
//         p_dst.resize(max_dst_safe_usize, 0u8);
//         // try for decompress
//         ret_size = unsafe {
//             w_lz4_decompress_safe(
//                 src_len,
//                 p_src.as_ptr().cast::<i8>(),
//                 max_dst_size,
//                 p_dst.as_mut_ptr().cast::<i8>(),
//             )
//         };
//         if ret_size > 0 {
//             break;
//         }
//         max_dst_size *= 2;
//     }

//     let new_size = usize::try_from(ret_size).unwrap_or(0);
//     if new_size > 0 {
//         p_dst.shrink_to(new_size);
//         unsafe { p_dst.set_len(new_size) };
//         return Ok(());
//     }
//     anyhow::bail!("missing buffer destination size. trace info: {}", TRACE)
// }
