#ifdef WOLF_MEDIA_FFMPEG

#include "w_ffmpeg_ctx.hpp"

using w_ffmpeg_ctx = wolf::media::ffmpeg::w_ffmpeg_ctx;

void w_ffmpeg_ctx::_release() noexcept {
  if (this->parser != nullptr) {
    av_parser_close(this->parser);
    this->parser = nullptr;
  }
  if (this->codec_ctx != nullptr) {
    if (avcodec_is_open(this->codec_ctx) > 0) {
      avcodec_close(this->codec_ctx);
    }
    avcodec_free_context(&this->codec_ctx);
    this->codec_ctx = nullptr;
  }
}

void w_ffmpeg_ctx::_move(w_ffmpeg_ctx &&p_other) noexcept {
  if (this == &p_other) {
    return;
  }
  this->codec_ctx = std::exchange(p_other.codec_ctx, nullptr);
  this->codec = std::exchange(p_other.codec, nullptr);
  this->parser = std::exchange(p_other.parser, nullptr);
}

std::string w_ffmpeg_ctx::get_av_error_str(_In_ int p_error_code) noexcept {
  std::array<char, W_MAX_PATH> _error[] = {'\0'};
  try {
    std::ignore = av_make_error_string(_error->data(), W_MAX_PATH, p_error_code);
    return std::string(_error->data());
  } catch (...) {
    return std::string();
  }
}

#endif // WOLF_MEDIA_FFMPEG