use crate::media::ffi::ffmpeg::{
    w_ffmpeg_action, w_ffmpeg_decode, w_ffmpeg_encode, w_ffmpeg_fini, w_ffmpeg_init, w_ffmpeg_opt,
};
use anyhow::{bail, Result};
use std::os::raw::c_char;

#[derive(Clone)]
pub struct CBuffer {
    pub ptr: *mut u8,
    pub len: i32,
}

#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub fn ffmpeg_init(p_ffmpeg_opt: w_ffmpeg_opt, p_error: &[u8]) -> Result<()> {
    let mut ret: i32 = unsafe { w_ffmpeg_init(p_ffmpeg_opt, p_error.as_ptr() as *mut c_char) };

    match ret {
        0 => Ok(()),
        _ => {
            bail!("could not initialize ffmpeg")
        }
    }
}

#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub fn ffmpeg_encode(
    p_ffmpeg_opt: w_ffmpeg_opt,
    p_data_in: &[u8],
    p_encoded_buffer: &mut CBuffer,
    p_error: &[u8],
) -> Result<()> {
    let mut ret: i32 = unsafe {
        w_ffmpeg_encode(
            p_ffmpeg_opt,
            p_data_in.as_ptr() as *mut u8,
            &mut p_encoded_buffer.ptr,
            &mut p_encoded_buffer.len,
            p_error.as_ptr() as *mut c_char,
        )
    };

    match ret {
        0 => Ok(()),
        _ => {
            bail!("could not encode the buffer")
        }
    }
}

#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub fn ffmpeg_decode(
    p_ffmpeg_opt: w_ffmpeg_opt,
    p_data_in: &[u8],
    p_data_size: i32,
    p_decoded_buffer: &mut CBuffer,
    p_error: &[u8],
) -> Result<()> {
    let mut ret: i32 = unsafe {
        w_ffmpeg_decode(
            p_ffmpeg_opt,
            p_data_in.as_ptr() as *mut u8,
            p_data_size,
            &mut p_decoded_buffer.ptr,
            &mut p_decoded_buffer.len,
            p_error.as_ptr() as *mut c_char,
        )
    };

    match ret {
        0 => Ok(()),
        _ => {
            bail!("could not decode the buffer")
        }
    }
}

#[allow(clippy::not_unsafe_ptr_arg_deref)]
pub fn ffmpeg_fini(p_ffmpeg_opt: w_ffmpeg_opt) -> Result<()> {
    unsafe { w_ffmpeg_fini(p_ffmpeg_opt) };

    Ok(())
}
