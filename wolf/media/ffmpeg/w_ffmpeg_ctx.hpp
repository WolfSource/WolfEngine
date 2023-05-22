/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace wolf::media::ffmpeg {

class w_ffmpeg_ctx {
 public:
  // constructor
  W_API w_ffmpeg_ctx() = default;
  // destructor
  W_API virtual ~w_ffmpeg_ctx() noexcept { _release(); }

  // move constructor.
  W_API w_ffmpeg_ctx(w_ffmpeg_ctx &&p_other) noexcept {
    _move(std::forward<w_ffmpeg_ctx &&>(p_other));
  }
  // move assignment operator.
  W_API w_ffmpeg_ctx &operator=(w_ffmpeg_ctx &&p_other) noexcept {
    _move(std::forward<w_ffmpeg_ctx &&>(p_other));
    return *this;
  }

  W_API static std::string get_av_error_str(_In_ int p_error_code) noexcept;

  gsl::owner<AVCodecContext*> codec_ctx = {};
  gsl::owner<const AVCodec*> codec = {};
  gsl::owner<AVCodecParserContext *> parser = {};

 private:
  // copy constructor
  w_ffmpeg_ctx(const w_ffmpeg_ctx &) = delete;
  // copy operator
  w_ffmpeg_ctx &operator=(const w_ffmpeg_ctx &) = delete;
  // release all resources
  void _release() noexcept;
  // move all resources
  void _move(w_ffmpeg_ctx &&p_other) noexcept;
};
}  // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG