// define types of ffmpeg

pub type w_av_format = *mut c_void;

// extern functions
extern "C" {
    pub fn rgb2yuv(
        p_rgb_raw: *mut u8,
        p_width: c_int,
        p_height: c_int,
        p_frame: *mut w_av_format,
        p_error: *mut c_char,
    ) -> int;
}
