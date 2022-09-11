use super::ffi::ffmpeg::{w_ffmpeg, w_ffmpeg_fini, w_ffmpeg_init, W_MAX_PATH};
use anyhow::{bail, Result};

#[derive(Clone)]
pub struct FFmpeg {
    pub ctx: w_ffmpeg,
}

impl Drop for FFmpeg {
    fn drop(&mut self) {
        if !self.ctx.is_null() {
            unsafe { w_ffmpeg_fini(&mut self.ctx) };
        }
    }
}

impl FFmpeg {
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new() -> Result<Self> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
        };

        // create a buffer for error
        let mut buf = [1i8; W_MAX_PATH as usize];
        let buf_ptr = buf.as_mut_ptr();

        unsafe {
            let ret = w_ffmpeg_init(&mut obj.ctx, buf_ptr);
            let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
            if ret == 0 {
                Ok(obj)
            } else {
                let str = c_err_str.to_str().unwrap_or_default();
                bail!(
                    "could not create ffmpeg object because {}",
                    String::from(str)
                )
            }
        }
    }
}

// /// # Errors
// ///
// /// Will return `Err` if `ffmpeg` could not initialize
// #[allow(clippy::not_unsafe_ptr_arg_deref)]
// pub fn init(p_ffmpeg: w_ffmpeg) -> Result<()> {
//     const TRACE: &str = "rist::init";

//     let ret: i32 = unsafe { w_ffmpeg_init(p_ffmpeg, p_error.as_ptr() as *mut c_char) };
//     match ret {
//         0 => Ok(()),
//         _ => {
//             bail!("could not initialize ffmpeg")
//         }
//     }
// }

// /// # Errors
// ///
// /// Will return `Err` if `ffmpeg` could encode the frame.
// #[allow(clippy::not_unsafe_ptr_arg_deref)]
// pub fn ffmpeg_encode(
//     p_ffmpeg_opt: w_ffmpeg_opt,
//     p_data_in: &[u8],
//     p_encoded_buffer: &mut CBuffer,
//     p_error: &[u8],
// ) -> Result<()> {
//     let ret: i32 = unsafe {
//         w_ffmpeg_encode(
//             p_ffmpeg_opt,
//             p_data_in.as_ptr() as *mut u8,
//             &mut p_encoded_buffer.ptr,
//             &mut p_encoded_buffer.len,
//             p_error.as_ptr() as *mut c_char,
//         )
//     };

//     match ret {
//         0 => Ok(()),
//         _ => {
//             bail!("could not encode the buffer")
//         }
//     }
// }

// /// # Errors
// ///
// /// Will return `Err` if `ffmpeg` failed on decoding the frame.
// #[allow(clippy::not_unsafe_ptr_arg_deref)]
// pub fn ffmpeg_decode(
//     p_ffmpeg_opt: w_ffmpeg_opt,
//     p_data_in: &[u8],
//     p_data_size: i32,
//     p_decoded_buffer: &mut CBuffer,
//     p_error: &[u8],
// ) -> Result<()> {
//     let ret: i32 = unsafe {
//         w_ffmpeg_decode(
//             p_ffmpeg_opt,
//             p_data_in.as_ptr() as *mut u8,
//             p_data_size,
//             &mut p_decoded_buffer.ptr,
//             &mut p_decoded_buffer.len,
//             p_error.as_ptr() as *mut c_char,
//         )
//     };

//     match ret {
//         0 => Ok(()),
//         _ => {
//             bail!("could not decode the buffer")
//         }
//     }
// }

// #[allow(clippy::not_unsafe_ptr_arg_deref)]
// pub fn ffmpeg_fini(p_ffmpeg_opt: w_ffmpeg_opt) {
//     unsafe { w_ffmpeg_fini(p_ffmpeg_opt) };
// }
