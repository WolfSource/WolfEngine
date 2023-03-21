use super::{av_config::AvConfig, av_frame::AvFrame, av_packet::AvPacket};
use crate::{
    error::WError,
    media::bindgen::ffmpeg::{
        av_dict_set, av_dict_set_int, av_opt_set, av_opt_set_double, av_opt_set_int,
        av_parser_close, av_parser_init, av_parser_parse2, av_strerror, avcodec_alloc_context3,
        avcodec_close, avcodec_find_decoder, avcodec_find_decoder_by_name, avcodec_find_encoder,
        avcodec_find_encoder_by_name, avcodec_free_context, avcodec_is_open, avcodec_open2,
        avcodec_receive_frame, avcodec_receive_packet, avcodec_send_frame, avcodec_send_packet,
        AVCodec, AVCodecContext, AVCodecID, AVCodecParserContext, AVDictionary, AVFMT_GLOBALHEADER,
        AV_CODEC_FLAG_GLOBAL_HEADER,
    },
};
use std::fmt::{Debug, Formatter};

const EOF: i32 = -22;
const EAGAIN: i32 = -11;

#[allow(clippy::cast_possible_wrap)]
const AV_NOPTS_VALUE: i64 = 0x8000_0000_0000_0000_u64 as i64;

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
pub union Value<'a> {
    pub i: i64,
    pub d: f64,
    pub s: &'a str,
}

#[derive(Clone, Copy)]
pub enum AvOptionValue<'a> {
    Int(Value<'a>),
    Double(Value<'a>),
    Str(Value<'a>),
}

impl Debug for AvOptionValue<'_> {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        unsafe {
            match self {
                AvOptionValue::Int(v) => f.debug_tuple("Int").field(&v.i).finish(),
                AvOptionValue::Double(v) => f.debug_tuple("Double").field(&v.d).finish(),
                AvOptionValue::Str(v) => f.debug_tuple("Str").field(&v.s).finish(),
            }
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
    codec: *const AVCodec,
    codec_ctx: *mut AVCodecContext,
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
    /// create a new ffmpeg encoder from codec id
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new_encoder(
        p_codec_id: AVCodecID,
        p_config: &AvConfig,
        p_codec_opt: &AvCodecOpt,
        p_options: &[AvSetOpt],
        p_av_dict: Option<*mut AVDictionary>,
    ) -> Result<Self, WError> {
        let mut obj = unsafe {
            let codec = avcodec_find_encoder(p_codec_id);
            if codec.is_null() {
                return Err(WError::MediaCodecNotFound);
            }
            Self {
                codec,
                codec_ctx: std::ptr::null_mut(),
                parser: std::ptr::null_mut(),
            }
        };
        obj.init(p_config, p_codec_opt, p_options, p_av_dict)?;
        Ok(obj)
    }

    /// create a new ffmpeg encoder from codec id string
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new_encoder_from_codec_id_str(
        p_codec_id: &str,
        p_config: &AvConfig,
        p_codec_opt: &AvCodecOpt,
        p_options: &[AvSetOpt],
        p_av_dict: Option<*mut AVDictionary>,
    ) -> Result<Self, WError> {
        let mut obj = unsafe {
            let codec = avcodec_find_encoder_by_name(p_codec_id.as_ptr().cast::<i8>());
            if codec.is_null() {
                return Err(WError::MediaCodecNotFound);
            }
            Self {
                codec,
                codec_ctx: std::ptr::null_mut(),
                parser: std::ptr::null_mut(),
            }
        };
        obj.init(p_config, p_codec_opt, p_options, p_av_dict)?;
        Ok(obj)
    }

    /// create a new ffmpeg decoder from codec id
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new_decoder(
        p_codec_id: AVCodecID,
        p_config: &mut AvConfig,
        p_codec_opt: &AvCodecOpt,
        p_options: &[AvSetOpt],
        p_av_dict: Option<*mut AVDictionary>,
    ) -> Result<Self, WError> {
        let mut obj = unsafe {
            let codec = avcodec_find_decoder(p_codec_id);
            if codec.is_null() {
                return Err(WError::MediaCodecNotFound);
            }
            if !(*codec).sample_fmts.is_null() {
                p_config.sample_fmt = *(*codec).sample_fmts;
            }
            Self {
                codec,
                codec_ctx: std::ptr::null_mut(),
                parser: std::ptr::null_mut(),
            }
        };
        obj.init(p_config, p_codec_opt, p_options, p_av_dict)?;
        obj.parser = unsafe { av_parser_init((*obj.codec).id as i32) };
        Ok(obj)
    }

    /// create a new ffmpeg decoder from codec id string
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new_decoder_from_codec_id_str(
        p_codec_id: &str,
        p_config: &mut AvConfig,
        p_codec_opt: &AvCodecOpt,
        p_options: &[AvSetOpt],
        p_av_dict: Option<*mut AVDictionary>,
    ) -> Result<Self, WError> {
        let mut obj = unsafe {
            let codec = avcodec_find_decoder_by_name(p_codec_id.as_ptr().cast::<i8>());
            if codec.is_null() {
                return Err(WError::MediaCodecNotFound);
            }
            if !(*codec).sample_fmts.is_null() {
                p_config.sample_fmt = *(*codec).sample_fmts;
            }
            Self {
                codec,
                codec_ctx: std::ptr::null_mut(),
                parser: std::ptr::null_mut(),
            }
        };
        obj.init(p_config, p_codec_opt, p_options, p_av_dict)?;
        obj.parser = unsafe { av_parser_init((*obj.codec).id as i32) };
        if obj.parser.is_null() {
            return Err(WError::MediaInitParserFailed);
        }
        Ok(obj)
    }

    fn init(
        &mut self,
        p_config: &AvConfig,
        p_codec_opts: &AvCodecOpt,
        p_options: &[AvSetOpt],
        p_av_dict: Option<*mut AVDictionary>,
    ) -> Result<(), WError> {
        // allocate codec context
        self.codec_ctx = unsafe { avcodec_alloc_context3(self.codec) };
        if self.codec_ctx.is_null() {
            return Err(WError::OutOfMemory);
        }

        unsafe {
            (*self.codec_ctx).width = p_config.width;
            (*self.codec_ctx).height = p_config.height;
            (*self.codec_ctx).bit_rate = p_codec_opts.bitrate;
            (*self.codec_ctx).time_base.num = 1;
            (*self.codec_ctx).time_base.den = p_codec_opts.fps;
            (*self.codec_ctx).framerate.num = p_codec_opts.fps;
            (*self.codec_ctx).framerate.den = 1;
            (*self.codec_ctx).pix_fmt = p_config.format;

            // set gop
            if p_codec_opts.gop >= 0 {
                (*self.codec_ctx).gop_size = p_codec_opts.gop;
            }

            // set refs
            if p_codec_opts.refs >= 0 {
                (*self.codec_ctx).refs = p_codec_opts.refs;
            }

            // set frames
            if p_codec_opts.max_b_frames >= 0 {
                (*self.codec_ctx).max_b_frames = p_codec_opts.max_b_frames;
            }
            // set thread numbers
            if p_codec_opts.thread_count >= 0 {
                (*self.codec_ctx).thread_count = p_codec_opts.thread_count;
            }
            // set level
            if p_codec_opts.level >= 0 {
                (*self.codec_ctx).level = p_codec_opts.level;
            }
            // set flags
            let flag = i32::try_from(AVFMT_GLOBALHEADER).map_err(|_e| WError::IntCastError)?;
            if ((*self.codec_ctx).flags & flag) != 0 {
                let flag_header = i32::try_from(AV_CODEC_FLAG_GLOBAL_HEADER)
                    .map_err(|_e| WError::IntCastError)?;
                (*self.codec_ctx).flags |= flag_header;
            }
        }

        for opt in p_options {
            if opt.name.is_empty() {
                continue;
            }

            let name_str = opt.name.as_ptr().cast::<i8>();
            match opt.value {
                AvOptionValue::Int(val) => unsafe {
                    let ret = av_opt_set_int((*self.codec_ctx).priv_data, name_str, val.i, 0);
                    if ret < 0 {
                        avcodec_free_context(&mut self.codec_ctx);
                        return Err(WError::MediaInvalidAvSetOption);
                    }
                },
                AvOptionValue::Double(val) => unsafe {
                    let ret = av_opt_set_double((*self.codec_ctx).priv_data, name_str, val.d, 0);
                    if ret < 0 {
                        avcodec_free_context(&mut self.codec_ctx);
                        return Err(WError::MediaInvalidAvSetOption);
                    }
                },
                AvOptionValue::Str(val) => unsafe {
                    let ret = av_opt_set(
                        (*self.codec_ctx).priv_data,
                        name_str,
                        val.s.as_ptr().cast::<i8>(),
                        0,
                    );
                    if ret < 0 {
                        avcodec_free_context(&mut self.codec_ctx);
                        return Err(WError::MediaInvalidAvSetOption);
                    }
                },
            };
        }

        let mut av_dic = p_av_dict.map_or(std::ptr::null_mut(), |av_dic| av_dic);
        // open avcodec
        let ret = unsafe {
            avcodec_open2(
                self.codec_ctx,
                (*self.codec_ctx).codec,
                std::ptr::addr_of_mut!(av_dic),
            )
        };
        if ret < 0 {
            return Err(WError::MediaAvCodecOpenFailed);
        }
        Ok(())
    }

    /// create `AvDictionary` from `AvSetOpt`
    /// # Errors
    ///
    /// returns `WError`
    pub fn setup_av_dictionary(p_options: &[AvSetOpt]) -> Result<*mut AVDictionary, WError> {
        if p_options.is_empty() {
            return Err(WError::InvalidParameter);
        }

        let mut av_dic: *mut AVDictionary = std::ptr::null_mut();
        let ret = unsafe {
            av_dict_set(
                std::ptr::addr_of_mut!(av_dic),
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
            match &opt.value {
                AvOptionValue::Int(value) => {
                    let ret = unsafe {
                        av_dict_set_int(std::ptr::addr_of_mut!(av_dic), name_str, value.i, 0)
                    };
                    if ret < 0 {
                        return Err(WError::MediaInvalidAvSetOption);
                    }
                }
                AvOptionValue::Double(_value) => {}
                AvOptionValue::Str(value) => unsafe {
                    if !value.s.is_empty() {
                        let value_str = value.s.as_ptr().cast::<i8>();
                        let ret =
                            av_dict_set(std::ptr::addr_of_mut!(av_dic), name_str, value_str, 0);
                        if ret < 0 {
                            return Err(WError::MediaInvalidAvSetOption);
                        }
                    }
                },
            }
        }
        Ok(av_dic)
    }

    /// get error
    #[must_use]
    pub fn get_av_error_str(p_error_code: i32) -> String {
        const MAX_ARRAY_SIZE: usize = 255;
        let mut error: [std::ffi::c_char; MAX_ARRAY_SIZE] = [0; MAX_ARRAY_SIZE];
        let ptr = error.as_mut_ptr();
        unsafe {
            av_strerror(p_error_code, ptr, MAX_ARRAY_SIZE);
            let c_err_str = std::ffi::CStr::from_ptr(ptr);
            c_err_str.to_str().unwrap_or_default().to_string()
        }
    }

    /// encode `AvFrame` to the `AvPacket`
    /// # Errors
    ///
    /// returns `WError`
    pub fn encode(&self, p_frame: &AvFrame, p_flush: bool) -> Result<AvPacket, WError> {
        let mut packet_data = Vec::new();
        // encode frame to packet
        self.encode_frame_to_packet(Some(p_frame), &mut packet_data)?;
        if p_flush {
            // flush
            self.encode_frame_to_packet(None, &mut packet_data)?;
        }
        AvPacket::new_from_data(packet_data)
    }

    /// encode `AvFrame` to the `AvPacket`
    /// # Errors
    ///
    /// returns `WError`
    fn encode_frame_to_packet(
        &self,
        p_frame: Option<&AvFrame>,
        p_packet_data: &mut Vec<u8>,
    ) -> Result<(), WError> {
        let tmp_packet = AvPacket::new()?;
        let frame = p_frame.map_or(std::ptr::null_mut(), |packet| packet.frame);
        loop {
            unsafe {
                let send_ret = avcodec_send_frame(self.codec_ctx, frame);
                if send_ret < 0 {
                    // let str = crate::media::ffmpeg::ffmpeg_ctx::FFmpeg::get_av_error_str(send_ret);
                    // println!("{str}");
                    return Err(WError::MediaEncodeSendFrameFailed);
                }
                loop {
                    let rec_ret = avcodec_receive_packet(self.codec_ctx, tmp_packet.pkt);
                    if rec_ret == 0 || rec_ret == EOF {
                        // append temp packet to source packet
                        let append_size = usize::try_from((*tmp_packet.pkt).size)
                            .map_err(|_e| WError::SizeCastError)?;
                        let ptr_offset = p_packet_data.len();
                        p_packet_data.resize(p_packet_data.len() + append_size, 0u8);
                        std::ptr::copy_nonoverlapping(
                            (*tmp_packet.pkt).data,
                            p_packet_data.as_mut_ptr().add(ptr_offset),
                            append_size,
                        );
                        return Ok(());
                    }
                    tmp_packet.unref();
                    if rec_ret == EAGAIN {
                        break;
                    }
                    if rec_ret < 0 {
                        return Err(WError::MediaEncodeReceivePacketFailed);
                    }
                }
            }
        }
    }

    /// decode `AvFrame` from the `AvPacket`
    /// # Errors
    ///
    /// returns `WError`
    pub fn decode(
        &self,
        p_packet: &AvPacket,
        p_frame: &mut AvFrame,
        p_flush: bool,
    ) -> Result<(), WError> {
        let dst_packet = AvPacket::new()?;
        unsafe {
            loop {
                let bytes = av_parser_parse2(
                    self.parser,
                    self.codec_ctx,
                    std::ptr::addr_of_mut!((*dst_packet.pkt).data),
                    std::ptr::addr_of_mut!((*dst_packet.pkt).size),
                    (*p_packet.pkt).data,
                    (*p_packet.pkt).size,
                    AV_NOPTS_VALUE,
                    AV_NOPTS_VALUE,
                    0,
                );
                let bytes_usize = usize::try_from(bytes).map_err(|_e| WError::MediaParserFailed)?;
                if bytes_usize == 0 {
                    break;
                }

                if (*dst_packet.pkt).size == 0 {
                    self.decode_frame_from_packet(Some(p_packet), p_frame)?;
                } else {
                    (*p_packet.pkt).data = (*p_packet.pkt).data.add(bytes_usize);
                    (*p_packet.pkt).size -= bytes;
                    if (*dst_packet.pkt).size > 0 {
                        self.decode_frame_from_packet(Some(&dst_packet), p_frame)?;
                    }
                }

                // (*p_packet.pkt).data = (*p_packet.pkt).data.add(bytes_usize);
                // (*p_packet.pkt).size -= bytes;

                // if (*dst_packet.pkt).size > 0 {
                //     self.decode_frame_from_packet(Some(&dst_packet), p_frame)?;
                // }
            }

            if p_flush {
                // flush the decoder
                self.decode_frame_from_packet(None, p_frame)?;
            }
        }
        Ok(())
    }

    /// decode `AvFrame` from the `AvPacket`
    /// # Errors
    ///
    /// returns `WError`
    fn decode_frame_from_packet(
        &self,
        p_packet: Option<&AvPacket>,
        p_frame: &mut AvFrame,
    ) -> Result<(), WError> {
        let packet = p_packet.map_or(std::ptr::null_mut(), |packet| packet.pkt);
        loop {
            unsafe {
                let send_ret = avcodec_send_packet(self.codec_ctx, packet);
                if send_ret < 0 {
                    // let str = FFmpeg::get_av_error_str(send_ret);
                    // println!("{str}");
                    return Err(WError::MediaDecodeSendFrameFailed);
                }
                let rec_ret = avcodec_receive_frame(self.codec_ctx, p_frame.frame);
                if rec_ret == 0 || rec_ret == EOF {
                    return Ok(());
                }
                if rec_ret == EAGAIN {
                    continue;
                }
                return Err(WError::MediaDecodeReceivePacketFailed);
            }
        }
    }
}
