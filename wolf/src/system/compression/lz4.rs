use crate::system::ffi::lz4::{
    w_lz4_compress_bound, w_lz4_compress_default, w_lz4_compress_fast, w_lz4_decompress_safe,
};
use std::os::raw::c_char;

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

pub fn compress(
    p_dst: &mut Vec<u8>,
    p_src: &[u8],
    p_mode: LZ4CompressMode,
    p_acceleration: i32,
) -> anyhow::Result<()> {
    const TRACE: &str = "lz4::compress";

    // allocate size for compressed data
    let src_len = p_src.len() as i32;
    let mut max_dst_size = unsafe { w_lz4_compress_bound(src_len) };
    p_dst.clear();
    p_dst.resize(max_dst_size as usize, 0u8);

    max_dst_size = match p_mode {
        LZ4CompressMode::DEFAULT => unsafe {
            w_lz4_compress_default(
                p_src.as_ptr() as *mut c_char,
                p_dst.as_mut_ptr() as *mut c_char,
                src_len,
                max_dst_size,
            )
        },
        LZ4CompressMode::FAST => unsafe {
            w_lz4_compress_fast(
                p_src.as_ptr() as *mut c_char,
                p_dst.as_mut_ptr() as *mut c_char,
                src_len,
                max_dst_size,
                p_acceleration,
            )
        },
    };

    let new_size = max_dst_size as usize;
    if new_size > 0 {
        p_dst.shrink_to(new_size);
        unsafe { p_dst.set_len(new_size) };
        return Ok(());
    }
    anyhow::bail!("missing buffer destination size. trace info: {}", TRACE)
}

pub fn de_compress(p_dst: &mut Vec<u8>, p_src: &[u8]) -> anyhow::Result<()> {
    const TRACE: &str = "lz4::decompress";

    // allocate size for decompressed data
    let src_len = p_src.len() as i32;
    let mut max_dst_size = src_len * 2;
    let mut ret_size = 0;

    p_dst.clear();
    for _ in 0..10 {
        // reserve new size for destination
        p_dst.resize(max_dst_size as usize, 0u8);
        // try for decompress
        ret_size = unsafe {
            w_lz4_decompress_safe(
                p_src.as_ptr() as *const c_char,
                p_dst.as_mut_ptr() as *mut c_char,
                src_len,
                max_dst_size,
            )
        };
        if ret_size > 0 {
            break;
        }
        max_dst_size *= 2;
    }

    if ret_size > 0 {
        let new_size = ret_size as usize;
        p_dst.shrink_to(new_size);
        unsafe { p_dst.set_len(new_size) };
        return Ok(());
    }
    anyhow::bail!("missing buffer destination size. trace info: {}", TRACE)
}
