use crate::media::ffi::ffmpeg::{av_image_get_buffer_size, AVPixelFormat};

#[derive(Debug, Clone)]
pub struct AvVideoConfig {
    // the format of av frame
    pub format: AVPixelFormat,
    // the width of av frame
    pub width: u32,
    // the height of av frame
    pub height: u32,
    // data alignment
    pub alignment: u32,
}

impl Default for AvVideoConfig {
    fn default() -> Self {
        Self {
            format: AVPixelFormat::AV_PIX_FMT_NONE,
            width: 0,
            height: 0,
            alignment: 1,
        }
    }
}

impl AvVideoConfig {
    pub fn new(p_format: AVPixelFormat, p_width: u32, p_height: u32, p_alignment: u32) -> Self {
        Self {
            format: p_format,
            width: p_width,
            height: p_height,
            alignment: p_alignment,
        }
    }

    pub fn get_required_buffer_size(&self) -> i32 {
        unsafe {
            av_image_get_buffer_size(
                self.format,
                self.width as i32,
                self.height as i32,
                self.alignment as i32,
            )
        }
    }
}
