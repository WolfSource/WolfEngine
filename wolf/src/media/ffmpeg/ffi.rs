use super::{av_audio_config::AvAudioConfig, av_video_config::AvVideoConfig};
use crate::{
    error::WError,
    media::bindgen::ffmpeg::{AVPixelFormat, AVSampleFormat},
};

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_media_av_audio_config_init(
    p_obj: *mut *mut AvAudioConfig,
    p_channels: u32,
    p_sample_rate: u32,
    p_sample_fmt: i32,
) -> WError {
    let sample_fmt: AVSampleFormat = std::mem::transmute(p_sample_fmt);
    let res = AvAudioConfig::new(p_channels, p_sample_rate, sample_fmt);
    match res {
        Ok(obj) => {
            *p_obj = Box::into_raw(Box::new(obj));
            WError::None
        }
        Err(e) => {
            *p_obj = std::ptr::null_mut();
            e
        }
    }
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_media_av_audio_config_fini(p_ptr: *mut AvAudioConfig) {
    if p_ptr.is_null() {
        return;
    }
    std::mem::drop(Box::from_raw(p_ptr));
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_media_av_video_config_init(
    p_obj: *mut *mut AvVideoConfig,
    p_pixel_format: i32,
    p_width: u32,
    p_height: u32,
    p_alignment: u32,
) -> WError {
    let pixel_fmt: AVPixelFormat = std::mem::transmute(p_pixel_format);
    let res = AvVideoConfig::new(pixel_fmt, p_width, p_height, p_alignment);
    match res {
        Ok(obj) => {
            *p_obj = Box::into_raw(Box::new(obj));
            WError::None
        }
        Err(e) => {
            *p_obj = std::ptr::null_mut();
            e
        }
    }
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_media_av_video_config_fini(p_ptr: *mut AvVideoConfig) {
    if p_ptr.is_null() {
        return;
    }
    std::mem::drop(Box::from_raw(p_ptr));
}
