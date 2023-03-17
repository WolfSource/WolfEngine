use crate::ScopeCall;
use std::fmt::{Debug, Formatter};

use crate::{
    error::WError,
    media::bindgen::ffmpeg::{
        av_dict_set, av_dict_set_int, av_parser_close, avcodec_close, avcodec_free_context,
        avcodec_is_open, AVCodec, AVCodecContext, AVCodecParserContext, AVDictionary,
        _av_make_error_string, avcodec_alloc_context3, AVCodecID,
    },
    //w_defer,
};

use super::av_video_config::AvVideoConfig;

#[repr(C)]
#[derive(Debug, Clone, Copy, Eq, PartialEq)]
pub enum FFmpegMode {
    Encoder,
    Decoder,
    Both,
}

#[derive(Debug, Clone, Copy)]
pub struct AvCodecOpt {
    pub bitrate: i64,
    pub fps: i32,
    pub gop: i32,
    pub level: i32,
    pub max_b_frames: i32,
    pub refs: i32,
    pub thread_count: i32,
}

#[derive(Clone, Copy)]
pub union AvOptionValue<'a> {
    pub i: i64,
    pub s: &'a str,
}

impl Debug for AvOptionValue<'_> {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        unsafe {
            f.debug_struct("AvOptionValue")
                .field("i", &self.i)
                .field("s", &self.s)
                .finish()
        }
    }
}

#[derive(Clone)]
pub struct AvSetOpt<'a> {
    // name of option
    pub name: String,
    // value of option
    pub value: AvOptionValue<'a>,
}

impl Debug for AvSetOpt<'_> {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("AvSetOpt")
            .field("name", &self.name)
            .field("value", &self.value)
            .finish()
    }
}

#[derive(Debug, Clone)]
pub struct FFmpeg {
    _mode: FFmpegMode,
    codec_ctx: *mut AVCodecContext,
    _codec: *mut AVCodec,
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
            #[cfg(debug_assertions)]
            println!("FFmpeg dropped");
        }
    }
}

impl FFmpeg {
    /// create a new ffmpeg object with both encoder & decoder modes
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new(
        p_encoder_id: AVCodecID,
        p_decoder_id: AVCodecID,
        _p_codec_opt: AvCodecOpt,
        _p_av_set_options: &[AvSetOpt],
    ) -> Result<Self, WError> {
        // if p_encoder_id != AVCodecID::AV_CODEC_ID_NONE {
        //     unsafe {
        //         let codec = avcodec_find_encoder(p_encoder_id);
        //     }
        // }
        // if p_decoder_id != AVCodecID::AV_CODEC_ID_NONE {
        //     unsafe {
        //         let codec = avcodec_find_decoder(p_decoder_id);
        //     }
        // }

        // Ok(Self {
        //     mode: (),
        //     codec_ctx: (),
        //     codec: (),
        //     parser: (),
        // })
        Err(WError::None)
    }

    fn create(
        p_config: &AvVideoConfig,
        p_av_codec: *mut AVCodec,
        p_codec_opts: &AvCodecOpt,
        _p_options: &[AvSetOpt],
    ) -> Result<(), WError> {
        let has_error = false;
        // allocate codec context
        let mut codec_ctx = unsafe { avcodec_alloc_context3(p_av_codec) };
        if codec_ctx.is_null() {
            return Err(WError::OutOfMemory);
        }
        // w_defer!(unsafe {
        //     if has_error && !codec_ctx.is_null() {
        //         avcodec_free_context(&mut codec_ctx);
        //     }
        // });

        unsafe {
            (*codec_ctx).width = p_config.width;
            (*codec_ctx).height = p_config.height;
            (*codec_ctx).bit_rate = p_codec_opts.bitrate;
            (*codec_ctx).time_base.num = 1;
            (*codec_ctx).time_base.den = p_codec_opts.fps;
            (*codec_ctx).framerate.num = p_codec_opts.fps;
            (*codec_ctx).framerate.den = 1;
            (*codec_ctx).pix_fmt = p_config.format;
        }

        // // set gop
        // if (p_codec_opts.gop >= 0) {
        //     codec_ctx.gop_size = p_codec_opts.gop;
        // }

        // // set refs
        // if p_codec_opts.refs >= 0 {
        //     codec_ctx.refs = p_codec_opts.refs;
        // }

        // // set frames
        // if p_codec_opts.max_b_frames >= 0 {
        //     codec_ctx.max_b_frames = p_codec_opts.max_b_frames;
        // }
        // // set thread numbers
        // if p_codec_opts.thread_count >= 0 {
        //     codec_ctx.thread_count = p_codec_opts.thread_count;
        // }
        // // set level
        // if p_codec_opts.level >= 0 {
        //     codec_ctx.level = p_codec_opts.level;
        // }
        // // set flags
        // if (_codec_ctx.flags & AVFMT_GLOBALHEADER) {
        //     codec_ctx.flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        // }

        // try {
        // for (const auto &_opt : p_opts) {
        // if (_opt.name.empty()) {
        // continue;
        // }

        // auto _name_str = _opt.name.c_str();
        // if (std::holds_alternative<int>(_opt.value)) {
        // // set an integer value
        // const auto _value = std::get<int>(_opt.value);
        // const auto _ret = av_opt_set_int(_codec_ctx->priv_data, _name_str, _value, 0);
        // if (_ret < 0) {
        // return W_FAILURE(std::errc::invalid_argument, "could not set int value for " + _opt.name +
        //                            ":" + std::to_string(_value) +
        //                            " because " +
        //                            w_ffmpeg_ctx::get_av_error_str(_ret));
        // }
        // } else if (std::holds_alternative<double>(_opt.value)) {
        // // set double value
        // const auto _value = std::get<int>(_opt.value);
        // const auto _ret = av_opt_set_double(_codec_ctx->priv_data, _name_str, _value, 0);
        // if (_ret < 0) {
        // return W_FAILURE(std::errc::invalid_argument, "could not set double value for " +
        //                            _opt.name + ":" +
        //                            std::to_string(_value) + " because " +
        //                            w_ffmpeg_ctx::get_av_error_str(_ret));
        // }
        // } else {
        // // set string value
        // const auto _value_str = std::get<std::string>(_opt.value);
        // if (!_value_str.empty()) {
        // const auto _ret =
        // av_opt_set(_codec_ctx->priv_data, _opt.name.c_str(), _value_str.c_str(), 0);
        // if (_ret < 0) {
        // return W_FAILURE(std::errc::invalid_argument,
        // "could not set string value for " + _opt.name + ":" + _value_str +
        // " because " + w_ffmpeg_ctx::get_av_error_str(_ret));
        // }
        // }
        // }
        // }
        // } catch (const std::exception &p_exc) {
        // return W_FAILURE(std::errc::operation_canceled,
        // "could not set av option because: " + std::string(p_exc.what()));
        // }

        // // open avcodec
        // const auto _ret = avcodec_open2(_codec_ctx, _codec_ctx->codec, nullptr);
        // if (_ret < 0) {
        // return W_FAILURE(std::errc::operation_canceled,
        // "could not open avcodec because " + w_ffmpeg_ctx::get_av_error_str(_ret));
        // }
        Ok(())
    }

    fn set_dict(p_options: &[AvSetOpt]) -> Result<*mut AVDictionary, WError> {
        if p_options.is_empty() {
            return Err(WError::InvalidParameter);
        }

        let mut av_dic: *mut AVDictionary = std::ptr::null_mut();
        let ret = unsafe {
            av_dict_set(
                &mut av_dic as *mut *mut AVDictionary,
                std::ptr::null_mut(),
                std::ptr::null_mut(),
                0,
            )
        };
        if ret < 0 {
            return Err(WError::OutOfMemory);
        }

        for opt in p_options {
            if opt.name.is_empty() {
                continue;
            }
            let name_str = opt.name.as_ptr().cast::<i8>();
            let ret = unsafe {
                match opt.value {
                    AvOptionValue { i } => {
                        av_dict_set_int(&mut av_dic as *mut *mut AVDictionary, name_str, i, 0)
                    }
                    AvOptionValue { s } => av_dict_set(
                        &mut av_dic as *mut *mut AVDictionary,
                        name_str,
                        s.as_ptr().cast::<i8>(),
                        0,
                    ),
                }
            };
            if ret < 0 {
                return Err(WError::OutOfMemory);
            }
        }
        Ok(av_dic)
    }

    fn get_av_error_str(p_error_code: i32) -> String {
        const MAX_ARRAY_SIZE: usize = 255;
        let mut error: [std::ffi::c_char; MAX_ARRAY_SIZE] = [0; MAX_ARRAY_SIZE];
        let ptr = error.as_mut_ptr();
        unsafe {
            _av_make_error_string(ptr, MAX_ARRAY_SIZE, p_error_code);
            let c_err_str = std::ffi::CStr::from_ptr(ptr);
            c_err_str.to_str().unwrap_or_default().to_string()
        }
    }
}
