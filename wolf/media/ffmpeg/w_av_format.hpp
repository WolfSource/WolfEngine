/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf/wolf.hpp>

extern "C" {
#include <libavformat/avformat.h>
}

namespace wolf::media::ffmpeg {
class w_av_format {
 public:
#pragma region Constructors /Destructor
  W_API w_av_format() noexcept;
  W_API ~w_av_format() noexcept { _release(); };
  // move constructor.
  W_API w_av_format(w_av_format &&p_other) noexcept = default;
  // move assignment operator.
  W_API w_av_format &operator=(w_av_format &&p_other) noexcept = default;
#pragma endregion

  boost::leaf::result<int> init(_In_ int p_stream_buf_size = 32'767) noexcept;

  uint8_t *get_io_ctx_buffer() const;
  int get_io_ctx_size() const;

  std::function<int(_Inout_ uint8_t * /*p_buf*/, _In_ int /*p_buf_size*/)> on_read_callback;

 private:
  // copy constructor.
  w_av_format(const w_av_format &) = delete;
  // copy assignment operator.
  w_av_format &operator=(const w_av_format &) = delete;

  // release
  void _release() noexcept;

  std::unique_ptr<uint8_t[]> _stream_buffer = nullptr;
  std::unique_ptr<AVFormatContext> _fmt_ctx = nullptr;
  std::unique_ptr<AVIOContext> _io_ctx = nullptr;
};
}  // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG
