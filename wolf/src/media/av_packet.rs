use std::ptr::null_mut;

use super::ffi::ffmpeg::{
    w_av_packet, w_av_packet_fini, w_av_packet_get, w_av_packet_init, W_MAX_PATH,
};
use anyhow::{bail, Result};

#[derive(Clone)]
pub struct AVPacket {
    pub ctx: w_av_packet,
}

impl Drop for AVPacket {
    fn drop(&mut self) {
        if !self.ctx.is_null() {
            unsafe { w_av_packet_fini(&mut self.ctx) };
        }
    }
}

impl AVPacket {
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new() -> Result<Self> {
        // create self object
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
        };

        // create a buffer for error
        let mut err = [1i8; W_MAX_PATH as usize];
        let err_ptr = err.as_mut_ptr();

        unsafe {
            let ret = w_av_packet_init(&mut obj.ctx, err_ptr);

            if ret == 0 {
                Ok(obj)
            } else {
                let c_err_str = std::ffi::CStr::from_ptr(err_ptr);
                let str = c_err_str.to_str().unwrap_or_default();
                bail!(
                    "could not create av_frame object because {}. trace: {:?}",
                    String::from(str),
                    std::backtrace::Backtrace::force_capture()
                )
            }
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    #[must_use]
    pub fn get_data(&self) -> (*mut u8, i32) {
        let data: *mut u8 = null_mut();
        let size = unsafe { w_av_packet_get(self.ctx, data) };
        (data, size)
    }
}
