use crate::{
    error::WError,
    media::ffi::ffmpeg::{
        av_channel_layout_default, av_channel_layout_uninit, AVChannelLayout, AVSampleFormat,
    },
};

#[derive(Debug, Clone)]
pub struct AvAudioConfig {
    // the sample rate of the audio
    pub sample_rate: u32,
    // the sample format of the audio
    pub sample_fmt: AVSampleFormat,
    // the channel layout of the audio
    pub channel_layout: *mut AVChannelLayout,
}

impl Drop for AvAudioConfig {
    fn drop(&mut self) {
        if self.channel_layout.is_null() {
            return;
        }
        unsafe { av_channel_layout_uninit(self.channel_layout) };
    }
}

impl Default for AvAudioConfig {
    fn default() -> Self {
        Self {
            sample_rate: 44100,
            sample_fmt: AVSampleFormat::AV_SAMPLE_FMT_S16,
            channel_layout: std::ptr::null_mut(),
        }
    }
}

impl AvAudioConfig {
    /// # Errors
    ///
    /// returns an `WError`
    pub fn new(
        p_channels: u32,
        p_sample_rate: u32,
        p_sample_fmt: AVSampleFormat,
    ) -> Result<Self, WError> {
        let channels = i32::try_from(p_channels).map_err(|_| WError::IntCastError)?;
        let config = Self {
            sample_rate: p_sample_rate,
            sample_fmt: p_sample_fmt,
            channel_layout: std::ptr::null_mut(),
        };
        unsafe { av_channel_layout_default(config.channel_layout, channels) };
        Ok(config)
    }
}

/// # Safety
///
/// unsafe function for C ABI
#[cfg(feature = "ffi")]
#[no_mangle]
pub unsafe extern "C" fn w_media_av_audio_config_new(
    p_channels: u32,
    p_sample_rate: u32,
    p_sample_fmt: i32,
    p_error: *mut WError,
) -> *mut AvAudioConfig {
    let sample_fmt: AVSampleFormat = std::mem::transmute(p_sample_fmt);
    let res = AvAudioConfig::new(p_channels, p_sample_rate, sample_fmt);
    match res {
        Ok(obj) => {
            std::ptr::write(p_error, WError::None);
            Box::into_raw(Box::new(obj))
        }
        Err(e) => {
            std::ptr::write(p_error, e);
            std::ptr::null_mut()
        }
    }
}

/// # Safety
///
/// unsafe function for C ABI
#[cfg(feature = "ffi")]
#[no_mangle]
pub unsafe extern "C" fn w_media_av_audio_config_free(p_ptr: *mut AvAudioConfig) {
    if p_ptr.is_null() {
        return;
    }
    std::mem::drop(Box::from_raw(p_ptr));
}
