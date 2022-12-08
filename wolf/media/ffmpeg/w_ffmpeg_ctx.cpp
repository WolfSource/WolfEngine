#ifdef WOLF_MEDIA_FFMPEG

#include "w_ffmpeg_ctx.hpp"

using w_ffmpeg_ctx = wolf::media::ffmpeg::w_ffmpeg_ctx;

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
  this->codec_ctx = std::exchange(p_other.codec_ctx, nullptr);
  this->parser = std::exchange(p_other.parser, nullptr);
}

void w_ffmpeg_ctx::_release() noexcept {
  if (this->parser != nullptr) {
    av_parser_close(this->parser);
    this->parser = nullptr;
  }
  if (this->codec_ctx) {
    if (avcodec_is_open(this->codec_ctx) > 0) {
      avcodec_close(this->codec_ctx);
    }
    avcodec_free_context(&this->codec_ctx);
    this->codec_ctx = nullptr;
  }
}

const std::string
w_ffmpeg_ctx::get_av_error_str(_In_ const int p_error_code) noexcept {
  char _error[W_MAX_PATH] = {'\0'};
  std::ignore = av_make_error_string(_error, W_MAX_PATH, p_error_code);
  return std::string(_error);
}

#endif // WOLF_MEDIA_FFMPEG