/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
extern "C" {
#include <libavcodec/avcodec.h>
}
#include <DISABLE_ANALYSIS_END>

namespace wolf::media::ffmpeg {

struct w_ffmpeg_ctx {
  AVCodecContext *codec_ctx = nullptr;
  const AVCodec *codec = nullptr;
  AVCodecParserContext *parser = nullptr;

  // constructor
  W_API w_ffmpeg_ctx() = default;
  // destructor
  W_API virtual ~w_ffmpeg_ctx() noexcept;

  // move constructor.
  W_API w_ffmpeg_ctx(w_ffmpeg_ctx &&p_other) noexcept;
  // move assignment operator.
  W_API w_ffmpeg_ctx &operator=(w_ffmpeg_ctx &&p_other) noexcept;

  W_API static const std::string get_av_error_str(_In_ const int p_error_code) noexcept;

private:
  // copy constructor
  w_ffmpeg_ctx(const w_ffmpeg_ctx &) = delete;
  // copy operator
  w_ffmpeg_ctx &operator=(const w_ffmpeg_ctx &) = delete;

  void _move(w_ffmpeg_ctx &&p_other) noexcept;
  void _release() noexcept;
};
} // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG