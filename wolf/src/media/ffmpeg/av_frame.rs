use super::{av_audio_config::AvAudioConfig, av_video_config::AvVideoConfig};
use crate::{
    error::WError,
    media::ffi::ffmpeg::{
        av_frame_alloc, av_frame_free, av_image_fill_arrays, sws_freeContext, sws_getContext,
        sws_scale, AVFrame, SWS_BICUBIC,
    },
};
use core::result::Result;

#[derive(Debug, Clone)]
pub struct AvFrame {
    pub audio_config: Option<AvAudioConfig>,
    pub video_config: Option<AvVideoConfig>,
    frame: *mut AVFrame,
    video_data: Vec<u8>,
    audio_data: Vec<u8>,
}

impl Default for AvFrame {
    fn default() -> Self {
        Self {
            audio_config: None,
            video_config: None,
            frame: std::ptr::null_mut(),
            video_data: Vec::new(),
            audio_data: Vec::new(),
        }
    }
}

impl Drop for AvFrame {
    fn drop(&mut self) {
        unsafe {
            av_frame_free(&mut self.frame);
        }
    }
}

impl AvFrame {
    /// # Errors
    ///
    /// returns an `WError`
    pub fn new(
        p_audio_config: AvAudioConfig,
        p_video_config: AvVideoConfig,
        p_audio_data: Vec<u8>,
        p_video_data: Vec<u8>,
    ) -> Result<Self, WError> {
        let mut av_frame = Self {
            audio_config: Some(p_audio_config),
            video_config: Some(p_video_config),
            frame: std::ptr::null_mut(),
            video_data: Vec::new(),
            audio_data: Vec::new(),
        };
        Self::init(&mut av_frame, p_audio_data, p_video_data)?;
        Ok(av_frame)
    }

    /// # Errors
    ///
    /// returns an `WError`
    pub fn new_audio(p_config: AvAudioConfig, p_audio_data: Vec<u8>) -> Result<Self, WError> {
        let mut av_frame = Self {
            audio_config: Some(p_config),
            video_config: None,
            frame: std::ptr::null_mut(),
            video_data: Vec::new(),
            audio_data: Vec::new(),
        };
        Self::init(&mut av_frame, p_audio_data, Vec::new())?;
        Ok(av_frame)
    }

    /// # Errors
    ///
    /// returns an `WError`
    pub fn new_video(p_config: &AvVideoConfig, p_video_data: Vec<u8>) -> Result<Self, WError> {
        let mut av_frame = Self {
            audio_config: None,
            video_config: Some(*p_config),
            frame: std::ptr::null_mut(),
            video_data: Vec::new(),
            audio_data: Vec::new(),
        };
        Self::init(&mut av_frame, Vec::new(), p_video_data)?;
        Ok(av_frame)
    }

    /// # Errors
    ///
    /// returns an `WError`
    fn init(
        p_frame: &mut Self,
        p_audio_data: Vec<u8>,
        p_video_data: Vec<u8>,
    ) -> Result<(), WError> {
        if p_frame.audio_config.is_none() && p_frame.video_config.is_none() {
            return Err(WError::MediaInvalidAvConfig);
        }
        // allocate memory for frame
        unsafe {
            p_frame.frame = av_frame_alloc();
            if p_frame.frame.is_null() {
                return Err(WError::OutOfMemory);
            }
        }

        // set audio frame buffer
        p_frame.set_audio_frame(p_audio_data)?;

        // set video frame buffer
        p_frame.set_video_frame(p_video_data)?;

        Ok(())
    }

    /// # Errors
    ///
    /// returns an `WError`
    #[allow(clippy::missing_const_for_fn)]
    pub fn get_video_data_ptr(&self, p_index: usize) -> Result<&[u8], WError> {
        if p_index > 7 {
            return Err(WError::InvalidParameter);
        }
        let slice = unsafe {
            let len = usize::try_from((*self.frame).linesize[p_index] * (*self.frame).height)
                .map_err(|_| WError::SizeCastError)?;
            std::slice::from_raw_parts((*self.frame).data[p_index] as *const u8, len)
        };
        Ok(slice)
    }

    /// # Errors
    ///
    /// returns an `WError`
    pub fn set_audio_frame(&mut self, p_data: Vec<u8>) -> Result<(), WError> {
        self.audio_data = p_data;
        Ok(())
        // if let Some(_p_config) = &self.audio_config {
        //     Ok(())
        // } else {
        //     Err(AvError::InvalidAvConfig)
        // }
    }

    /// # Errors
    ///
    /// returns an `WError`
    pub fn set_video_frame(&mut self, p_data: Vec<u8>) -> Result<(), WError> {
        // set frame data
        self.video_data = p_data;

        let config = self.video_config;
        if let Some(p_config) = &config {
            let format = p_config.format as i32;
            // store width, height and format to frame
            unsafe {
                (*self.frame).width = p_config.width;
                (*self.frame).height = p_config.height;
                (*self.frame).format = format;
            }
            return self.fill_video_frame(p_config);
        }
        Err(WError::MediaInvalidAvConfig)
    }

    /// # Errors
    ///
    /// returns an `WError`
    pub fn convert_video(&self, p_dst_config: &AvVideoConfig) -> Result<Self, WError> {
        if let Some(p_video_config) = &self.video_config {
            // create flag
            #[allow(clippy::cast_possible_wrap)]
            let flags = SWS_BICUBIC as i32;

            // create a destination video frame
            let dst_frame = Self::new_video(p_dst_config, Vec::new())?;
            // create sws context
            unsafe {
                let sws_context = sws_getContext(
                    p_video_config.width,
                    p_video_config.height,
                    p_video_config.format,
                    p_dst_config.width,
                    p_dst_config.height,
                    p_dst_config.format,
                    flags,
                    std::ptr::null_mut(),
                    std::ptr::null_mut(),
                    std::ptr::null_mut(),
                );
                if sws_context.is_null() {
                    return Err(WError::MediaInvalidSwsContext);
                }

                // scale video frame
                let height = sws_scale(
                    sws_context,
                    (*self.frame).data.as_ptr().cast::<*const u8>(),
                    (*self.frame).linesize.as_ptr(),
                    0,
                    p_video_config.height,
                    (*dst_frame.frame).data.as_mut_ptr(),
                    (*dst_frame.frame).linesize.as_mut_ptr(),
                );
                // free context
                sws_freeContext(sws_context);

                if height < 0 {
                    return Err(WError::MediaSwsScaleFailed);
                }
            };
            return Ok(dst_frame);
        }
        Err(WError::MediaInvalidAvConfig)
    }

    /// # Errors
    ///
    /// returns an `WError`
    fn fill_video_frame(&mut self, p_config: &AvVideoConfig) -> Result<(), WError> {
        let buf_size = usize::try_from(p_config.get_required_buffer_size())
            .map_err(|_| WError::SizeCastError)?;
        if self.video_data.is_empty() {
            // allocate buffer based on the config
            self.video_data = vec![0u8; buf_size];
        } else if buf_size < self.video_data.len() {
            return Err(WError::MediaInvalidVideoFrameSize);
        }

        unsafe {
            let size = av_image_fill_arrays(
                (*self.frame).data.as_mut_ptr(),
                (*self.frame).linesize.as_mut_ptr(),
                self.video_data.as_ptr(),
                p_config.format,
                p_config.width,
                p_config.height,
                p_config.alignment,
            );
            if size < 0 {
                return Err(WError::MediaImageFillFailed);
            }
        }
        Ok(())
    }
}
