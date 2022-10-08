use super::ffi::openal::{
    size_t, w_openal, w_openal_close, w_openal_fini, w_openal_init, w_openal_list_all_devices,
    w_openal_open, w_openal_reset, w_openal_update_i16,
};
use crate::system::ffi::sys_init::W_MAX_PATH;
use anyhow::{bail, Result};

#[derive(Clone, Debug)]
pub enum SampleRate {
    _44100 = 44_100,
    _48000 = 48_000,
}

#[derive(Clone, Debug)]
pub enum Format {
    Mono8 = 0x1100,
    /// Signed 16-bit mono buffer format
    Mono16 = 0x1101,
    /// Unsigned 8-bit stereo buffer format
    Stereo8 = 0x1102,
    /// Signed 16-bit stereo buffer format
    Stereo16 = 0x1103,
}

#[derive(Clone, Debug)]
pub struct OpenAl {
    ctx: w_openal,
    format: Format,
    sample_rate: SampleRate,
    refresh: u32,
}

impl Drop for OpenAl {
    fn drop(&mut self) {
        if !self.ctx.is_null() {
            unsafe { w_openal_fini(&mut self.ctx) };
        }
    }
}

impl OpenAl {
    /// # Errors
    ///
    /// TODO: add error description
    #[allow(clippy::cast_possible_wrap)]
    #[allow(clippy::cast_possible_truncation)]
    pub fn new(
        p_format: Format,
        p_sample_rate: SampleRate,
        p_refresh: u32,
        p_number_of_channels: std::num::NonZeroU32,
    ) -> Result<Self> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
            format: p_format,
            sample_rate: p_sample_rate,
            refresh: p_refresh,
        };

        // create a buffer for error
        let mut buf = [1i8; W_MAX_PATH as usize];
        let buf_ptr = buf.as_mut_ptr();

        let channels = p_number_of_channels.get() as i32;
        unsafe {
            let ret = w_openal_init(
                &mut obj.ctx,
                obj.format as i32,
                obj.sample_rate as i32,
                obj.refresh as i32,
                channels,
                buf_ptr,
            );
            if ret == 0 {
                Ok(obj)
            } else {
                let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
                let str = c_err_str.to_str().unwrap_or_default();
                bail!(
                    "could not create openal object because {}",
                    String::from(str)
                )
            }
        }
    }

    /// # Safety
    ///
    /// TODO: add safety description
    /// # Errors
    ///
    /// TODO: add error description
    pub fn open(&self) -> Result<()> {
        // create a buffer for error
        let mut buf = [1i8; W_MAX_PATH as usize];
        let buf_ptr = buf.as_mut_ptr();

        unsafe {
            let ret = w_openal_open(self.ctx, buf_ptr);
            if ret == 0 {
                Ok(())
            } else {
                let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
                let str = c_err_str.to_str().unwrap_or_default();
                bail!(
                    "could not create openal object because {}",
                    String::from(str)
                )
            }
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn update_from_stack_i16(&self, p_audio_frame_buffer: &[i16]) -> Result<()> {
        let mut buf = [1i8; W_MAX_PATH as usize];
        let buf_ptr = buf.as_mut_ptr();

        unsafe {
            let ret = w_openal_update_i16(
                self.ctx,
                p_audio_frame_buffer.as_ptr(),
                p_audio_frame_buffer.len() as size_t,
                buf_ptr,
            );
            if ret == 0 {
                Ok(())
            } else {
                let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
                let str = c_err_str.to_str().unwrap_or_default();
                bail!(
                    "could not update openal object because {}",
                    String::from(str)
                )
            }
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn update_from_heap_i16(&self, p_audio_frame_buffer: &Vec<i16>) -> Result<()> {
        let mut buf = [1i8; W_MAX_PATH as usize];
        let buf_ptr = buf.as_mut_ptr();

        unsafe {
            let ret = w_openal_update_i16(
                self.ctx,
                p_audio_frame_buffer.as_ptr(),
                p_audio_frame_buffer.len() as size_t,
                buf_ptr,
            );
            if ret == 0 {
                Ok(())
            } else {
                let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
                let str = c_err_str.to_str().unwrap_or_default();
                bail!(
                    "could not update openal object because {}",
                    String::from(str)
                )
            }
        }
    }

    /// reset openal
    pub fn reset(&self) {
        unsafe {
            w_openal_reset(self.ctx);
        }
    }

    /// close openal
    pub fn close(&self, p_force_stop: bool) {
        unsafe {
            w_openal_close(self.ctx, p_force_stop);
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn list_all_devices() -> Result<(String, String)> {
        // create two buffers for input & output devices
        let mut input = [0i8; W_MAX_PATH as usize];
        let mut output = [0i8; W_MAX_PATH as usize];
        let input_ptr = input.as_mut_ptr();
        let output_ptr = output.as_mut_ptr();

        unsafe {
            w_openal_list_all_devices(output_ptr, input_ptr);

            let in_cstr = std::ffi::CStr::from_ptr(input_ptr);
            let out_cstr = std::ffi::CStr::from_ptr(output_ptr);

            let in_devices = in_cstr.to_str().unwrap_or_default();
            let out_devices = out_cstr.to_str().unwrap_or_default();

            Ok((String::from(out_devices), String::from(in_devices)))
        }
    }
}
