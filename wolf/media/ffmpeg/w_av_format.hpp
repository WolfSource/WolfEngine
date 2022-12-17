/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
extern "C" {
#include <libavformat/avformat.h>
}
#include <DISABLE_ANALYSIS_END>

namespace wolf::media::ffmpeg {

struct w_av_format {
  /**
   * constructor the av_frame with specific config
   * @param p_config, the av frame config
   */
  W_API w_av_format() noexcept;

  // move constructor.
  W_API w_av_format(w_av_format &&p_other) noexcept;
  // move assignment operator.
  W_API w_av_format &operator=(w_av_format &&p_other) noexcept;

  // destructor
  W_API virtual ~w_av_format() noexcept;

  boost::leaf::result<int> init(_In_ int p_stream_buf_size = 32'767) noexcept;

  uint8_t *get_io_ctx_buffer() const { return this->_io_ctx->buffer; }
  int get_io_ctx_size() const { return this->_io_ctx->buffer_size; }

  std::function<int(_Inout_ uint8_t * /*p_buf*/, _In_ int /*p_buf_size*/)>
      on_read_callback;
  
private:
  // copy constructor.
  w_av_format(const w_av_format &) = delete;
  // copy assignment operator.
  w_av_format &operator=(const w_av_format &) = delete;

  // move implementation
  void _move(w_av_format &&p_other) noexcept;
  // release
  void _release() noexcept;

  uint8_t *_stream_buffer;
  AVFormatContext *_fmt_ctx;
  AVIOContext *_io_ctx;

};
} // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG
