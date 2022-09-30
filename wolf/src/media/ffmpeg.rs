// use super::ffi::ffmpeg::{w_ffmpeg, w_ffmpeg_fini};

use super::{
    av_frame::AVFrame,
    av_packet::AVPacket,
    ffi::ffmpeg::{
        w_ffmpeg, w_ffmpeg_decode, w_ffmpeg_encode, w_ffmpeg_fini, w_ffmpeg_init, AVCodeOptions,
        AVSetOption, W_MAX_PATH,
    },
};
use anyhow::{bail, Result};

#[derive(Clone)]
pub enum FFmpegMode {
    ENCODER = 0,
    DECODER,
}

#[derive(Clone)]
pub struct FFmpeg {
    pub ctx: w_ffmpeg,
    pub mode: FFmpegMode,
    pub codec_id: String,
    pub codec_opt: AVCodeOptions,
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
    pub fn new(
        p_av_frame: &AVFrame,
        p_mode: FFmpegMode,
        p_codec_id: &str,
        p_codec_opt: AVCodeOptions,
        p_av_set_options: &[AVSetOption],
    ) -> Result<Self> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
            mode: p_mode,
            codec_id: p_codec_id.to_owned(),
            codec_opt: p_codec_opt,
        };

        // create a buffer for error
        let mut buf = [1i8; W_MAX_PATH as usize];
        let buf_ptr = buf.as_mut_ptr();

        unsafe {
            #[allow(clippy::cast_possible_truncation)]
            let ret = w_ffmpeg_init(
                &mut obj.ctx,
                p_av_frame.ctx,
                obj.mode as u32,
                p_codec_id.as_ptr().cast::<i8>(),
                &mut obj.codec_opt,
                p_av_set_options.as_ptr(),
                p_av_set_options.len() as u32,
                buf_ptr,
            );
            if ret == 0 {
                Ok(obj)
            } else {
                let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
                let str = c_err_str.to_str().unwrap_or_default();
                bail!(
                    "could not create ffmpeg object because {}",
                    String::from(str)
                )
            }
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn encode(&self, p_av_frame: &AVFrame, p_packet: &mut AVPacket) -> Result<i32> {
        // create a buffer for error
        let mut buf = [1i8; W_MAX_PATH as usize];
        let buf_ptr = buf.as_mut_ptr();

        unsafe {
            let packet_size = w_ffmpeg_encode(self.ctx, p_av_frame.ctx, &mut p_packet.ctx, buf_ptr);
            if packet_size < 0 {
                let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
                let str = c_err_str.to_str().unwrap_or_default();
                bail!("could not encode av frame because {}", String::from(str))
            }
            Ok(packet_size)
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn decode(&self, p_packet: &AVPacket, p_av_frame: &mut AVFrame) -> Result<()> {
        // create a buffer for error
        let mut buf = [1i8; W_MAX_PATH as usize];
        let buf_ptr = buf.as_mut_ptr();

        unsafe {
            let ret = w_ffmpeg_decode(self.ctx, p_packet.ctx, &mut p_av_frame.ctx, buf_ptr);
            if ret == 0 {
                Ok(())
            } else {
                let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
                let str = c_err_str.to_str().unwrap_or_default();
                bail!("could not decode av frame because {}", String::from(str))
            }
        }
    }
}
