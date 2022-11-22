/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <DISABLE_ANALYSIS_END>

namespace wolf::media::ffmpeg {

struct w_av_frame_config {
  // the pixel format of ffmpeg AVFrame
  AVPixelFormat format;
  // the width of ffmpeg AVFrame
  int width;
  // the height of ffmpeg AVFrame
  int height;
};

class w_av_frame {
public:
  /**
   * constructor the av_frame with specific config
   * @param p_config, the AVFrame config
   */
  //
  W_API w_av_frame(_In_ w_av_frame_config &&p_config) noexcept;

  // move constructor.
  W_API w_av_frame(w_av_frame &&p_other) noexcept = default;
  // move assignment operator.
  W_API w_av_frame &operator=(w_av_frame &&p_other) noexcept = default;

  // destructor
  W_API virtual ~w_av_frame() noexcept;

  /**
   * set the AVFrame data
   * @param p_data, the initial data of ffmpeg AVFrame
   * @param p_alignment, the alignment
   * @returns zero on success
   */
  W_API boost::leaf::result<int> set(_In_ const uint8_t *p_data,
                                     _In_ int p_alignment) noexcept;

  /**
   * get data and linesize as a tuple
   * @returns tuple<int*[8], int[8]>
   */
  W_API
  std::tuple<uint8_t **, int *> get() const noexcept;

  /**
   * convert the ffmpeg AVFrame
   * @returns the converted instance of AVFrame
   */
  W_API
  boost::leaf::result<w_av_frame>
  convert(_In_ w_av_frame_config &&p_dst_config);

  /**
   * p_pixel_format, av pixle format
   * p_width, the width of frame
   * p_height, the height of frame
   * p_alignment, the alignment
   * @returns the required buffer size for frame
   */
  W_API
  static size_t get_required_buffer_size(_In_ AVPixelFormat p_pixel_format,
                                         _In_ int p_width, _In_ int p_height,
                                         _In_ int p_alignment) noexcept;

private:
  // copy constructor.
  w_av_frame(const w_av_frame &) = delete;
  // copy assignment operator.
  w_av_frame &operator=(const w_av_frame &) = delete;

  void _release() noexcept;

  w_av_frame_config _config;
  AVFrame *_av_frame;
};
} // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG
