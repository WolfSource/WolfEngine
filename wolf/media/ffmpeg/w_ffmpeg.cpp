#ifdef WOLF_MEDIA_FFMPEG

#include "w_ffmpeg.hpp"

#include <DISABLE_ANALYSIS_BEGIN>
extern "C" {
#include <libavutil/opt.h>
}
#include <DISABLE_ANALYSIS_END>

using w_ffmpeg = wolf::media::ffmpeg::w_ffmpeg;
using w_ffmpeg_ctx = wolf::media::ffmpeg::w_ffmpeg_ctx;
using w_av_frame = wolf::media::ffmpeg::w_av_frame;
using w_av_codec_opt = wolf::media::ffmpeg::w_av_codec_opt;
using w_av_set_opt = wolf::media::ffmpeg::w_av_set_opt;
using w_encoder = wolf::media::ffmpeg::w_encoder;
using w_decoder = wolf::media::ffmpeg::w_decoder;

static const std::string s_get_av_err_str(_In_ const int p_error_code) noexcept {
  std::string _av_error;
  _av_error.reserve(W_MAX_PATH);
  std::ignore =
      av_make_error_string(_av_error.data(), W_MAX_PATH, p_error_code);
  return _av_error;
}

#pragma region w_ffmpeg_ctx

w_ffmpeg_ctx::w_ffmpeg_ctx(w_ffmpeg_ctx &&p_other) noexcept {
  _move(std::move(p_other));
}

w_ffmpeg_ctx &w_ffmpeg_ctx::operator=(w_ffmpeg_ctx &&p_other) noexcept {
  _move(std::move(p_other));
  return *this;
}

w_ffmpeg_ctx::~w_ffmpeg_ctx() noexcept { _release(); }

void w_ffmpeg_ctx::_move(w_ffmpeg_ctx &&p_other) noexcept {
  if (this == &p_other)
    return;

  _release();

  this->codec = std::exchange(p_other.codec, nullptr);
  this->codec_id = p_other.codec_id;
  this->context = std::exchange(p_other.context, nullptr);
  this->parser = std::exchange(p_other.parser, nullptr);
}

void w_ffmpeg_ctx::_release() noexcept {
  if (this->parser != nullptr) {
    av_parser_close(this->parser);
    this->parser = nullptr;
  }
  if (this->context) {
    avcodec_close(this->context);
    avcodec_free_context(&this->context);
    this->context = nullptr;
  }
}

#pragma endregion

static boost::leaf::result<int>
create(_Inout_ w_ffmpeg_ctx &p_ctx, _In_ const w_av_frame &p_frame,
       _In_ const std::string_view p_id, _In_ const w_av_codec_opt &p_settings,
       _In_ const std::vector<w_av_set_opt> &p_opts) noexcept {

  p_ctx.context = avcodec_alloc_context3(p_ctx.codec);
  auto _context_nn = gsl::narrow_cast<AVCodecContext *>(p_ctx.context);

  const auto _frame_config = p_frame.get_config();

  _context_nn->width = _frame_config.width;
  _context_nn->height = _frame_config.height;
  _context_nn->bit_rate = p_settings.bitrate;
  _context_nn->time_base = AVRational{1, p_settings.fps};
  _context_nn->framerate = AVRational{p_settings.fps, 1};
  _context_nn->pix_fmt = _frame_config.format;

  // set gop
  if (p_settings.gop >= 0) {
    _context_nn->gop_size = gsl::narrow_cast<int>(p_settings.gop);
  }
  // set refs
  if (p_settings.refs >= 0) {
    _context_nn->refs = gsl::narrow_cast<int>(p_settings.refs);
  }
  // set frames
  if (p_settings.max_b_frames >= 0) {
    _context_nn->max_b_frames = gsl::narrow_cast<int>(p_settings.max_b_frames);
  }
  // set thread numbers
  if (p_settings.thread_count >= 0) {
    _context_nn->thread_count = gsl::narrow_cast<int>(p_settings.thread_count);
  }
  // set level
  if (p_settings.level >= 0) {
    _context_nn->level = p_settings.level;
  }
  // set flags
  if (_context_nn->flags & AVFMT_GLOBALHEADER) {
    _context_nn->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
  }

  for (const auto &_opt : p_opts) {
    if (_opt.name.empty() == true) {
      continue;
    }

    auto _name_str = _opt.name.c_str();
    if (std::holds_alternative<int>(_opt.value)) {
      // set an integer value
      const auto _value = std::get<int>(_opt.value);
      const auto _ret =
          av_opt_set_int(_context_nn->priv_data, _name_str, _value, 0);
      if (_ret < 0) {
        std::string _value_str;
        try {
          _value_str = std::to_string(_value);
        } catch (...) {
        };
        return W_ERR(std::errc::invalid_argument,
                     "could not set int value for " + _opt.name + ":" +
                         _value_str + " because " + s_get_av_err_str(_ret));
      }
    } else if (std::holds_alternative<double>(_opt.value)) {
      // set double value
      const auto _value = std::get<int>(_opt.value);
      const auto _ret =
          av_opt_set_double(_context_nn->priv_data, _name_str, _value, 0);
      if (_ret < 0) {
        std::string _value_str;
        try {
          _value_str = std::to_string(_value);
        } catch (...) {
        };
        return W_ERR(std::errc::invalid_argument,
                     "could not set double value for " + _opt.name + ":" +
                         _value_str + " because " + s_get_av_err_str(_ret));
      }
    } else {
      // set string value
      const auto _value_str = std::get<std::string>(_opt.value);
      if (_value_str.empty() == false) {
        const auto _ret = av_opt_set(_context_nn->priv_data, _opt.name.c_str(),
                                     _value_str.c_str(), 0);
        if (_ret < 0) {
          return W_ERR(std::errc::invalid_argument,
                       "could not set string value for " + _opt.name + ":" +
                           _value_str + " because " + s_get_av_err_str(_ret));
        }
      }
    }
    // open avcodec
    const auto _ret = avcodec_open2(_context_nn, _context_nn->codec, nullptr);
    if (_ret < 0) {
      return W_ERR(std::errc::operation_canceled,
                   "could not open avcodec because " + s_get_av_err_str(_ret));
    }
  }
  return W_OK();
}

boost::leaf::result<w_encoder> w_ffmpeg::create_encoder(
    _In_ const w_av_frame &p_frame, _In_ const std::string &p_id,
    _In_ const w_av_codec_opt &p_settings,
    _In_ const std::vector<w_av_set_opt> &p_opts) noexcept {

  w_encoder _encoder = {};

  _encoder.ctx.codec = avcodec_find_encoder_by_name(p_id.c_str());
  if (_encoder.ctx.codec == nullptr) {
    return W_ERR(std::errc::invalid_argument,
                 "could not find encoder codec id: " + p_id);
  }

  BOOST_LEAF_CHECK(create(_encoder.ctx, p_frame, p_id, p_settings, p_opts));

  return _encoder;
}

boost::leaf::result<w_decoder> w_ffmpeg::create_decoder(
    _In_ const w_av_frame &p_frame, _In_ const std::string &p_id,
    _In_ const w_av_codec_opt &p_settings,
    _In_ const std::vector<w_av_set_opt> &p_opts) noexcept {

  w_decoder _decoder = {};

  _decoder.ctx.codec = avcodec_find_decoder_by_name(p_id.c_str());
  if (_decoder.ctx.codec == nullptr) {
    return W_ERR(std::errc::invalid_argument,
                 "could not find decoder codec id: " + p_id);
  }
  _decoder.ctx.parser = av_parser_init(_decoder.ctx.codec->id);
  if (_decoder.ctx.parser == nullptr) {
    return W_ERR(std::errc::invalid_argument,
                 "could not initialize parser for codec id: " + p_id);
  }

  BOOST_LEAF_CHECK(create(_decoder.ctx, p_frame, p_id, p_settings, p_opts));

  return _decoder;
}


//#include <cstdint>
//#include <cstdarg>
//#include <vector>
//
//#include <DISABLE_ANALYSIS_BEGIN>
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavformat/avio.h>
//#include <libavutil/imgutils.h>
//#include <libavutil/opt.h>
//#include <libavutil/time.h>
//#include <libswscale/swscale.h>
//
//#ifdef __cplusplus
//}
//#endif



//int w_ffmpeg_encode(
//    _In_ w_ffmpeg p_ffmpeg,
//    _In_ const w_av_frame p_av_frame,
//    _Inout_ w_av_packet* p_packet,
//    _Inout_ char* p_error)
//{
//    constexpr auto TRACE = "ffmpeg::w_ffmpeg_encode";
//    const auto _ffmpeg_nn = gsl::not_null<w_ffmpeg>(p_ffmpeg);
//    const auto _packet_ptr_nn = gsl::not_null<w_av_packet*>(p_packet);
//    const auto _packet_nn = gsl::not_null<w_av_packet>(*_packet_ptr_nn);
//    const auto _error = gsl::not_null<char*>(p_error);
//
//    av_packet_unref(_packet_nn);
//    _packet_nn->data = nullptr;
//    _packet_nn->size = 0;
//
//    auto _ret = avcodec_send_frame(_ffmpeg_nn->context, p_av_frame);
//    if (_ret < 0)
//    {
//        std::array<char, W_MAX_PATH> _av_error = { 0 };
//        std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);
//
//        std::ignore = snprintf(
//            _error,
//            W_MAX_PATH,
//            "failed to send the frame for encoding because: %s. trace info: %s",
//            _av_error.data(),
//            TRACE);
//        return W_FAILURE;
//    }
//
//    _ret = avcodec_receive_packet(p_ffmpeg->context, _packet_nn);
//    if (_ret == AVERROR(EAGAIN) || _ret == AVERROR_EOF)
//    {
//        return W_SUCCESS;
//    }
//    
//    if (_ret < 0)
//    {
//        std::array<char, W_MAX_PATH> _av_error = { 0 };
//        std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);
//
//        std::ignore = snprintf(
//            _error,
//            W_MAX_PATH,
//            "error happened during the encoding because: %s. trace info: %s",
//            _av_error.data(),
//            TRACE);
//        return _ret;
//    }
//
//    return _packet_nn->size;
//}
//
//int s_ffmpeg_packet_to_frame(
//    _In_ const gsl::not_null<w_ffmpeg>& p_ffmpeg, 
//    _In_ AVPacket* p_packet,
//    _Inout_ gsl::not_null<w_av_frame> p_av_frame,
//    _Inout_ const gsl::not_null<char*>& p_error)
//{
//    constexpr auto TRACE = "ffmpeg::s_ffmpeg_packet_to_frame";
//    
//    // start decoding
//    auto _ret = avcodec_send_packet(p_ffmpeg->context, p_packet);
//    if (_ret < 0)
//    {
//        std::array<char, W_MAX_PATH> _av_error = { 0 };
//        std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);
//
//        std::ignore = snprintf(
//            p_error,
//            W_MAX_PATH,
//            "could not parse packet for decoding because: %s. trace info: %s",
//            _av_error.data(),
//            TRACE);
//        return _ret;
//    }
//
//    for (;;)
//    {
//        _ret = avcodec_receive_frame(p_ffmpeg->context, p_av_frame);
//        if (_ret == 0 || _ret == AVERROR(EAGAIN) || _ret == AVERROR_EOF)
//        {
//            break;
//        }
//
//        if (_ret < 0)
//        {
//            std::array<char, W_MAX_PATH> _av_error = { 0 };
//            std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);
//
//            std::ignore = snprintf(
//                p_error,
//                W_MAX_PATH,
//                "error happened during the encoding because: %s. trace info: %s",
//                _av_error.data(),
//                TRACE);
//            break;
//        }
//    }
//    return _ret;
//}
//
//int w_ffmpeg_decode(
//    _In_ w_ffmpeg p_ffmpeg,
//    _In_ const w_av_packet p_packet,
//    _Inout_ w_av_frame* p_av_frame,
//    _Inout_ char* p_error)
//{
//    constexpr auto TRACE = "ffmpeg::w_ffmpeg_decode";
//    const auto _ffmpeg_nn = gsl::not_null<w_ffmpeg>(p_ffmpeg);
//
//    const auto _src_packet_nn = gsl::not_null<const w_av_packet>(p_packet);
//    auto _av_frame_ptr_nn = gsl::not_null<w_av_frame*>(p_av_frame);
//    auto _av_frame_nn = gsl::not_null<w_av_frame>(*_av_frame_ptr_nn);
//    const auto _error = gsl::not_null<char*>(p_error);
//
//    int _ret = 0;
//
//    auto _dst_packet = av_packet_alloc();
//    if (_dst_packet == nullptr)
//    {
//        std::ignore = snprintf(
//            _error,
//            W_MAX_PATH,
//            "could not allocate packet for decoding. trace info: %s",
//            TRACE);
//        return _ret = -1;
//    }
//
//#ifdef __clang__
//#pragma unroll
//#endif
//    for (;;)
//    {
//        const auto _num = av_parser_parse2(
//            _ffmpeg_nn->parser,
//            _ffmpeg_nn->context,
//            &_dst_packet->data,
//            &_dst_packet->size,
//            _src_packet_nn->data,
//            _src_packet_nn->size,
//            AV_NOPTS_VALUE,
//            AV_NOPTS_VALUE,
//            0);
//        if (_num < 0)
//        {
//            std::array<char, W_MAX_PATH> _av_error = { 0 };
//            std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);
//
//            std::ignore = snprintf(
//                _error,
//                W_MAX_PATH,
//                "could not parse packet for decoding because: %s. trace info: %s",
//                _av_error.data(),
//                TRACE);
//            _ret = -1;
//            break;
//        }
//
//        _src_packet_nn->data += _num;
//        _src_packet_nn->size -= _num;
//        
//        if (_dst_packet->size == 0)
//        {
//            break;
//        }
//
//        if (_dst_packet->size > 0)
//        {
//            _ret = s_ffmpeg_packet_to_frame(_ffmpeg_nn, _dst_packet, _av_frame_nn, _error);
//            if (_ret != 0)
//            {
//                break;
//            }
//        }
//    }
//
//    av_packet_free(&_dst_packet);
//
//    return _ret;
//}

#endif // WOLF_MEDIA_FFMPEG