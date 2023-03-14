use crate::{
    error::WError,
    media::ffi::ffmpeg::{av_image_get_buffer_size, AVPixelFormat},
};

#[derive(Debug, Clone, Copy)]
pub struct AvVideoConfig {
    // the format of av frame
    pub format: AVPixelFormat,
    // the width of av frame
    pub width: i32,
    // the height of av frame
    pub height: i32,
    // data alignment
    pub alignment: i32,
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
    /// # Errors
    ///
    /// returns an `WError`
    pub fn new(
        p_format: AVPixelFormat,
        p_width: u32,
        p_height: u32,
        p_alignment: u32,
    ) -> Result<Self, WError> {
        // cast to i32
        let width = i32::try_from(p_width).map_err(|_| WError::IntCastError)?;
        let height = i32::try_from(p_height).map_err(|_| WError::IntCastError)?;
        let alignment = i32::try_from(p_alignment).map_err(|_| WError::IntCastError)?;

        Ok(Self {
            format: p_format,
            width,
            height,
            alignment,
        })
    }

    #[must_use]
    pub fn get_required_buffer_size(&self) -> i32 {
        unsafe { av_image_get_buffer_size(self.format, self.width, self.height, self.alignment) }
    }
}
