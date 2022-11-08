use crate::system::ffi::lz4::{
    w_lz4_compress_bound, w_lz4_compress_default, w_lz4_compress_fast, w_lz4_decompress_safe,
};

#[derive(Copy, Clone)]
pub enum LZ4CompressMode {
    DEFAULT,
    FAST,
}

impl std::fmt::Debug for LZ4CompressMode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::DEFAULT => write!(f, "LZ4CompressMode_DEFAULT"),
            Self::FAST => write!(f, "LZ4CompressMode_FAST"),
        }
    }
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
) -> anyhow::Result<()> {
    const TRACE: &str = "lz4::compress";

    // allocate size for compressed data
    let src_len = i32::try_from(p_src.len()).unwrap_or(0);
    if src_len == 0 {
        anyhow::bail!("invalid source size. trace info: {}", TRACE)
    }

    let max_dst_size = unsafe { w_lz4_compress_bound(src_len) };
    let safe_max_dst_size = usize::try_from(max_dst_size).unwrap_or(0);
    if safe_max_dst_size == 0 {
        anyhow::bail!("invalid destination size. trace info: {}", TRACE)
    }

    // now we are sure that safe_max_dst_size is a signed integer
    p_dst.clear();
    p_dst.resize(safe_max_dst_size, 0u8);

    let new_size_i32 = match p_mode {
        LZ4CompressMode::DEFAULT => unsafe {
            w_lz4_compress_default(
                src_len,
                p_src.as_ptr().cast(),
                safe_max_dst_size.try_into().unwrap(),
                p_dst.as_mut_ptr().cast::<i8>(),
            )
        },
        LZ4CompressMode::FAST => unsafe {
            w_lz4_compress_fast(
                p_acceleration,
                src_len,
                p_src.as_ptr().cast(),
                safe_max_dst_size.try_into().unwrap(),
                p_dst.as_mut_ptr().cast::<i8>(),
            )
        },
    };

    let new_size = usize::try_from(new_size_i32).unwrap_or(0);
    if new_size > 0 {
        p_dst.shrink_to(new_size);
        unsafe { p_dst.set_len(new_size) };
        return Ok(());
    }
    anyhow::bail!("missing buffer destination size. trace info: {}", TRACE)
}

/// # Errors
///
/// TODO: add error description
pub fn de_compress(p_dst: &mut Vec<u8>, p_src: &[u8]) -> anyhow::Result<()> {
    const TRACE: &str = "lz4::decompress";

    // allocate size for decompressed data
    let src_len = i32::try_from(p_src.len()).unwrap_or(0);
    if src_len == 0 {
        anyhow::bail!("invalid source length. trace info: {}", TRACE)
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
            w_lz4_decompress_safe(
                src_len,
                p_src.as_ptr().cast::<i8>(),
                max_dst_size,
                p_dst.as_mut_ptr().cast::<i8>(),
            )
        };
        if ret_size > 0 {
            break;
        }
        max_dst_size *= 2;
    }

    let new_size = usize::try_from(ret_size).unwrap_or(0);
    if new_size > 0 {
        p_dst.shrink_to(new_size);
        unsafe { p_dst.set_len(new_size) };
        return Ok(());
    }
    anyhow::bail!("missing buffer destination size. trace info: {}", TRACE)
}
