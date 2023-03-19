use crate::{
    error::WError,
    media::bindgen::ffmpeg::{
        av_channel_layout_default, av_channel_layout_uninit, av_image_get_buffer_size,
        AVChannelLayout, AVPixelFormat, AVSampleFormat,
    },
};

#[derive(Debug, Clone)]
pub struct AvConfig {
    // the format of av frame
    pub format: AVPixelFormat,
    // the width of av frame
    pub width: i32,
    // the height of av frame
    pub height: i32,
    // data alignment
    pub alignment: i32,
    // the sample rate of the audio
    pub sample_rate: u32,
    // the sample format of the audio
    pub sample_fmt: AVSampleFormat,
    // the channel layout of the audio
    pub channel_layout: *mut AVChannelLayout,
}

impl Drop for AvConfig {
    fn drop(&mut self) {
        if self.channel_layout.is_null() {
            return;
        }
        unsafe { av_channel_layout_uninit(self.channel_layout) };

        #[cfg(debug_assertions)]
        println!("AvConfig dropped");
    }
}

impl Default for AvConfig {
    fn default() -> Self {
        Self {
            format: AVPixelFormat::AV_PIX_FMT_NONE,
            width: 0,
            height: 0,
            alignment: 1,
            sample_rate: 44100,
            sample_fmt: AVSampleFormat::AV_SAMPLE_FMT_S16,
            channel_layout: std::ptr::null_mut(),
        }
    }
}

impl AvConfig {
    /// # Errors
    ///
    /// returns an `WError`
    pub fn new(
        p_format: AVPixelFormat,
        p_width: u32,
        p_height: u32,
        p_alignment: u32,
        p_channels: u32,
        p_sample_rate: u32,
        p_sample_fmt: AVSampleFormat,
    ) -> Result<Self, WError> {
        let channels = i32::try_from(p_channels).map_err(|_| WError::IntCastError)?;
        // cast to i32
        let width = i32::try_from(p_width).map_err(|_| WError::IntCastError)?;
        let height = i32::try_from(p_height).map_err(|_| WError::IntCastError)?;
        let alignment = i32::try_from(p_alignment).map_err(|_| WError::IntCastError)?;
        let config = Self {
            format: p_format,
            width,
            height,
            alignment,
            sample_rate: p_sample_rate,
            sample_fmt: p_sample_fmt,
            channel_layout: std::ptr::null_mut(),
        };
        unsafe { av_channel_layout_default(config.channel_layout, channels) };
        Ok(config)
    }

    /// # Errors
    ///
    /// returns an `WError`
    pub fn new_video(
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
            ..Default::default()
        })
    }

    /// # Errors
    ///
    /// returns an `WError`
    pub fn new_audio(
        p_channels: u32,
        p_sample_rate: u32,
        p_sample_fmt: AVSampleFormat,
    ) -> Result<Self, WError> {
        let channels = i32::try_from(p_channels).map_err(|_| WError::IntCastError)?;
        let config = Self {
            sample_rate: p_sample_rate,
            sample_fmt: p_sample_fmt,
            channel_layout: std::ptr::null_mut(),
            ..Default::default()
        };
        unsafe { av_channel_layout_default(config.channel_layout, channels) };
        Ok(config)
    }

    #[must_use]
    pub fn get_required_buffer_size(&self) -> i32 {
        unsafe { av_image_get_buffer_size(self.format, self.width, self.height, self.alignment) }
    }
}
