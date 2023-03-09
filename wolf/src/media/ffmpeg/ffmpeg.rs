use super::av_error::AvError;
use crate::media::ffi::ffmpeg::{
    _av_make_error_string, av_opt_set, av_opt_set_double, av_opt_set_int, av_parser_close,
    avcodec_close, avcodec_free_context, avcodec_is_open, AVCodec, AVCodecContext,
    AVCodecParserContext, AVFormatContext,
};

#[derive(Debug, Clone, Copy)]
pub enum FFmpegMode {
    Encoder = 0,
    Decoder,
    EncoderAndDecoder,
}

pub struct AvCodecOpt {
    pub bitrate: i64,
    pub fps: i32,
    pub gop: i32,
    pub level: i32,
    pub max_b_frames: i32,
    pub refs: i32,
    pub thread_count: i32,
}

// union AvOptionValue<'a> {
//     pub i: i32,
//     pub f: f64,
//     pub s: &'a str,
// }

// struct AvSetOpt<'a> {
//     // name of option
//     pub name: String,
//     // value of option
//     pub value: AvOptionValue<'a>,
// }

#[derive(Clone)]
pub struct FFmpeg {
    mode: FFmpegMode,
    codec_ctx: *mut AVCodecContext,
    codec: *mut AVCodec,
    parser: *mut AVCodecParserContext,
}

impl Drop for FFmpeg {
    fn drop(&mut self) {
        unsafe {
            if !self.parser.is_null() {
                av_parser_close(self.parser);
            }
            if !self.codec_ctx.is_null() {
                if avcodec_is_open(self.codec_ctx) > 0 {
                    avcodec_close(self.codec_ctx);
                }
                avcodec_free_context(&mut self.codec_ctx);
            }
        }
    }
}

// //impl FFmpeg {
// // /// create a new ffmpeg object with both encoder & decoder modes
// // /// # Errors
// // ///
// // /// TODO: add error description
// // pub fn new(
// //     p_mode: FFmpegMode,
// //     p_codec_id: &str,
// //     p_codec_opt: AVCodeOptions,
// //     p_av_set_options: &[AVSetOption],
// // ) -> Result<Self> {
// // }

// /// create a new ffmpeg object with both encoder & decoder modes
// /// # Errors
// ///
// /// TODO: add error description
// pub fn new_encoder(
//     p_audio_config: Option<AvAudioConfig>,
//     p_video_config: Option<AvVideoConfig>,
//     p_id: &str,
//     p_codec_opts: &AvCodecOpt,
//     p_opts: &[AvSetOpt],
// ) -> Result<Self, AvError> {
//     Ok(())
// }

// fn get_av_error_str(p_error_code: i32) -> String {
//     const max_array_size: usize = 255;
//     let mut error: [std::ffi::c_char; max_array_size] = [0; max_array_size];
//     let ptr = error.as_mut_ptr();
//     unsafe {
//         _av_make_error_string(ptr, max_array_size, p_error_code);
//         let c_err_str = std::ffi::CStr::from_ptr(ptr);
//         c_err_str.to_str().unwrap_or_default().to_string()
//     }
// }

// #[allow(unreachable_patterns)]
// fn set_opts(p_ctx: *mut AVFormatContext, p_opts: &[AvSetOpt]) -> Result<(), AvError> {
//     for opt in p_opts {
//         if opt.name.is_empty() {
//             continue;
//         }

//         let name_str = opt.name.as_str().as_ptr() as *const i8;
//         unsafe {
//             match opt.value {
//                 AvOptionValue { i } => {
//                     let ret = av_opt_set_int((*p_ctx).priv_data, name_str, i.into(), 0);
//                     if ret < 0 {
//                         let error_msg = get_av_error_str(ret);
//                         return Err(AvError::InvalidAvSetOption(error_msg));
//                     }
//                 }
//                 AvOptionValue { f } => {
//                     let ret = av_opt_set_double((*p_ctx).priv_data, name_str, f, 0);
//                     if ret < 0 {
//                         let error_msg = get_av_error_str(ret);
//                         return Err(AvError::InvalidAvSetOption(error_msg));
//                     }
//                 }
//                 AvOptionValue { s } => {
//                     let ret = av_opt_set((*p_ctx).priv_data, name_str, s.as_ptr() as *const i8, 0);
//                     if ret < 0 {
//                         let error_msg = get_av_error_str(ret);
//                         return Err(AvError::InvalidAvSetOption(error_msg));
//                     }
//                 }
//             }
//         }
//     }
//     Ok(())
// }

// // /// # Errors
// // ///
// // /// TODO: add error description
// // pub fn encode(&self, p_av_frame: &AVFrame, p_packet: &mut AVPacket) -> Result<i32> {
// //     // create a buffer for error
// //     let mut buf = [1i8; W_MAX_PATH as usize];
// //     let buf_ptr = buf.as_mut_ptr();

// //     unsafe {
// //         let packet_size = w_ffmpeg_encode(self.ctx, p_av_frame.ctx, &mut p_packet.ctx, buf_ptr);
// //         if packet_size < 0 {
// //             let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
// //             let str = c_err_str.to_str().unwrap_or_default();
// //             bail!("could not encode av frame because {}", String::from(str))
// //         }
// //         Ok(packet_size)
// //     }
// // }

// // /// # Errors
// // ///
// // /// TODO: add error description
// // pub fn decode(&self, p_packet: &AVPacket, p_av_frame: &mut AVFrame) -> Result<()> {
// //     // create a buffer for error
// //     let mut buf = [1i8; W_MAX_PATH as usize];
// //     let buf_ptr = buf.as_mut_ptr();

// //     unsafe {
// //         let ret = w_ffmpeg_decode(self.ctx, p_packet.ctx, &mut p_av_frame.ctx, buf_ptr);
// //         if ret == 0 {
// //             Ok(())
// //         } else {
// //             let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
// //             let str = c_err_str.to_str().unwrap_or_default();
// //             bail!("could not decode av frame because {}", String::from(str))
// //         }
// //     }
// // }
// //}
