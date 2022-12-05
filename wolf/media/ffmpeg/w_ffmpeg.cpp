#ifdef WOLF_MEDIA_FFMPEG

#include "w_ffmpeg.hpp"

#include <DISABLE_ANALYSIS_BEGIN>
extern "C" {
#include <libavutil/opt.h>
}
#include <DISABLE_ANALYSIS_END>

using w_ffmpeg = wolf::media::ffmpeg::w_ffmpeg;
using w_ffmpeg_ctx = wolf::media::ffmpeg::w_ffmpeg_ctx;
using w_av_config = wolf::media::ffmpeg::w_av_config;
using w_av_codec_opt = wolf::media::ffmpeg::w_av_codec_opt;
using w_av_set_opt = wolf::media::ffmpeg::w_av_set_opt;
using w_encoder = wolf::media::ffmpeg::w_encoder;
using w_decoder = wolf::media::ffmpeg::w_decoder;

static boost::leaf::result<int>
create(_Inout_ w_ffmpeg_ctx &p_ctx, _In_ const w_av_config &p_config,
       _In_ const std::string_view p_id,
       _In_ const w_av_codec_opt &p_codec_opts,
       _In_ const std::vector<w_av_set_opt> &p_opts) noexcept {

  p_ctx.codec_ctx = avcodec_alloc_context3(p_ctx.codec);
  auto _context_nn = gsl::narrow_cast<AVCodecContext *>(p_ctx.codec_ctx);

  _context_nn->width = p_config.width;
  _context_nn->height = p_config.height;
  _context_nn->bit_rate = p_codec_opts.bitrate;
  _context_nn->time_base = AVRational{1, p_codec_opts.fps};
  _context_nn->framerate = AVRational{p_codec_opts.fps, 1};
  _context_nn->pix_fmt = p_config.format;

  // set gop
  if (p_codec_opts.gop >= 0) {
    _context_nn->gop_size = gsl::narrow_cast<int>(p_codec_opts.gop);
  }
  // set refs
  if (p_codec_opts.refs >= 0) {
    _context_nn->refs = gsl::narrow_cast<int>(p_codec_opts.refs);
  }
  // set frames
  if (p_codec_opts.max_b_frames >= 0) {
    _context_nn->max_b_frames =
        gsl::narrow_cast<int>(p_codec_opts.max_b_frames);
  }
  // set thread numbers
  if (p_codec_opts.thread_count >= 0) {
    _context_nn->thread_count =
        gsl::narrow_cast<int>(p_codec_opts.thread_count);
  }
  // set level
  if (p_codec_opts.level >= 0) {
    _context_nn->level = p_codec_opts.level;
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
                         _value_str + " because " +
                         w_ffmpeg_ctx::get_av_error_str(_ret));
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
                         _value_str + " because " +
                         w_ffmpeg_ctx::get_av_error_str(_ret));
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
                           _value_str + " because " +
                           w_ffmpeg_ctx::get_av_error_str(_ret));
        }
      }
    }
  }

  // open avcodec
  const auto _ret = avcodec_open2(_context_nn, _context_nn->codec, nullptr);
  if (_ret < 0) {
    return W_ERR(std::errc::operation_canceled,
                 "could not open avcodec because " +
                     w_ffmpeg_ctx::get_av_error_str(_ret));
  }
  return W_SUCCESS();
}

boost::leaf::result<w_encoder> w_ffmpeg::create_encoder(
    _In_ const w_av_config &p_config, _In_ const std::string &p_id,
    _In_ const w_av_codec_opt &p_codec_opts,
    _In_ const std::vector<w_av_set_opt> &p_opts) noexcept {

  w_encoder _encoder = {};

  _encoder.ctx.codec = avcodec_find_encoder_by_name(p_id.c_str());
  if (_encoder.ctx.codec == nullptr) {
    return W_ERR(std::errc::invalid_argument,
                 "could not find encoder codec id: " + p_id);
  }

  BOOST_LEAF_CHECK(create(_encoder.ctx, p_config, p_id, p_codec_opts, p_opts));

  return _encoder;
}

boost::leaf::result<w_decoder> w_ffmpeg::create_decoder(
    _In_ const w_av_config &p_config, _In_ const std::string &p_id,
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

  BOOST_LEAF_CHECK(create(_decoder.ctx, p_config, p_id, p_settings, p_opts));

  return _decoder;
}

#endif // WOLF_MEDIA_FFMPEG