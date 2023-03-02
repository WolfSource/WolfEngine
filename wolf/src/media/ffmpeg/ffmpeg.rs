// use crate::media::binding::ffmpeg::{
//     av_parser_close, avcodec_close, avcodec_free_context, avcodec_is_open, AVCodec, AVCodecContext,
//     AVCodecParserContext,
// };

// #[derive(Debug, Clone, Copy)]
// pub enum FFmpegMode {
//     Encoder = 0,
//     Decoder,
//     EncoderAndDecoder,
// }

// pub struct AvCodecOpt {
//     bitrate: i64,
//     fps: i32,
//     gop: i32,
//     level: i32,
//     max_b_frames: i32,
//     refs: i32,
//     thread_count: i32,
// }

// union AvOptionValue<'a> {
//     i: i32,
//     f: f64,
//     s: &'a str,
// }

// struct AvSetOpt<'a> {
//     // name of option
//     name: String,
//     // value of option
//     value: AvOptionValue<'a>,
// }

// #[derive(Clone)]
// pub struct FFmpeg {
//     mode: FFmpegMode,
//     codec_ctx: *mut AVCodecContext,
//     codec: *mut AVCodec,
//     parser: *mut AVCodecParserContext,
// }

// impl Drop for FFmpeg {
//     fn drop(&mut self) {
//         unsafe {
//             if !self.parser.is_null() {
//                 av_parser_close(self.parser);
//             }
//             if !self.codec_ctx.is_null() {
//                 if avcodec_is_open(self.codec_ctx) > 0 {
//                     avcodec_close(self.codec_ctx);
//                 }
//                 avcodec_free_context(&mut self.codec_ctx);
//             }
//         }
//     }
// }

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

// // /// create a new ffmpeg object with both encoder & decoder modes
// // /// # Errors
// // ///
// // /// TODO: add error description
// // pub fn new_encoder(
// //     p_audio_config: Option<AvAudioConfig>,
// //     p_video_config: Option<AvVideoConfig>,
// //     p_id: &str,
// //     p_codec_opts: &AvCodecOpt,
// //     p_opts: &[AvSetOpt],
// // ) -> Result<Self, AvError> {
// //     Ok(())
// // }

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
