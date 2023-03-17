use super::{av_audio_config::AvAudioConfig, av_video_config::AvVideoConfig};
use crate::{
    error::WError,
    media::bindgen::ffmpeg::{
        av_channel_layout_default, av_channel_layout_uninit, AVPixelFormat, AVSampleFormat,
    },
    w_fini, w_init,
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
    w_init!(
        p_obj,
        AvAudioConfig::new(p_channels, p_sample_rate, sample_fmt)
    )
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_media_av_audio_config_set(
    p_obj: *mut AvAudioConfig,
    p_channels: u32,
    p_sample_rate: u32,
    p_sample_fmt: i32,
) -> WError {
    if p_obj.is_null() {
        return WError::MemoryWasNotInitialized;
    }
    let channels_res = i32::try_from(p_channels);
    match channels_res {
        Ok(channels) => {
            let sample_fmt: AVSampleFormat = std::mem::transmute(p_sample_fmt);
            let obj = &mut *p_obj;
            obj.sample_rate = p_sample_rate;
            obj.sample_fmt = sample_fmt;

            if (*obj.channel_layout).nb_channels != channels {
                av_channel_layout_uninit(obj.channel_layout);
                av_channel_layout_default(obj.channel_layout, channels);
            }
            WError::None
        }
        Err(_e) => WError::IntCastError,
    }
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_media_av_audio_config_fini(p_ptr: *mut *mut AvAudioConfig) {
    w_fini!(p_ptr);
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
    w_init!(
        p_obj,
        AvVideoConfig::new(pixel_fmt, p_width, p_height, p_alignment)
    )
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_media_av_video_config_set(
    p_obj: *mut AvVideoConfig,
    p_pixel_format: i32,
    p_width: u32,
    p_height: u32,
    p_alignment: u32,
) -> WError {
    if p_obj.is_null() {
        return WError::MemoryWasNotInitialized;
    }

    let pixel_fmt: AVPixelFormat = std::mem::transmute(p_pixel_format);
    let obj = &mut *p_obj;
    obj.format = pixel_fmt;

    let mut res = i32::try_from(p_width);
    match res {
        Ok(num) => obj.width = num,
        Err(_e) => return WError::IntCastError,
    }
    res = i32::try_from(p_height);
    match res {
        Ok(num) => obj.height = num,
        Err(_e) => return WError::IntCastError,
    }
    res = i32::try_from(p_alignment);
    match res {
        Ok(num) => obj.alignment = num,
        Err(_e) => return WError::IntCastError,
    }
    WError::None
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_media_av_video_config_get_required_buffer_size(
    p_obj: *mut AvVideoConfig,
) -> i32 {
    if p_obj.is_null() {
        return -1;
    }
    let obj = &mut *p_obj;
    obj.get_required_buffer_size()
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_media_av_video_config_fini(p_ptr: *mut *mut AvVideoConfig) {
    w_fini!(p_ptr);
}
