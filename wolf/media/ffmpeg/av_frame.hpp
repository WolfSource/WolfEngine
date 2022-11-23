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
#include <libavutil/frame.h>
}
#include <DISABLE_ANALYSIS_END>

namespace wolf::media::ffmpeg {

struct w_av_config {
  // the format of av frame
  AVPixelFormat format = AVPixelFormat::AV_PIX_FMT_NONE;
  // the width of av frame
  int width = 0;
  // the height of av frame
  int height = 0;
  // data alignment
  int alignment = 1;

  /**
   * @returns the required buffer size for frame
   */
  W_API size_t get_required_buffer_size() noexcept;
};

class w_av_frame {
public:
  /**
   * constructor the av_frame with specific config
   * @param p_config, the av frame config
   */
  //
  W_API w_av_frame(_In_ const w_av_config &p_config) noexcept;

  // move constructor.
  W_API w_av_frame(w_av_frame &&p_other) noexcept;
  // move assignment operator.
  W_API w_av_frame &operator=(w_av_frame &&p_other) noexcept;

  // destructor
  W_API virtual ~w_av_frame() noexcept;

  /**
   * set the AVFrame data
   * @param p_data, the initial data of ffmpeg AVFrame
   * @param p_alignment, the alignment
   * @returns zero on success
   */
  W_API boost::leaf::result<int> set(_Inout_ uint8_t **p_data) noexcept;

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
  boost::leaf::result<w_av_frame> convert(_In_ const w_av_config &p_dst_config);

  /**
   * @returns av_config
   */
  W_API w_av_config get_config() const;

#ifdef WOLF_MEDIA_STB

  /**
   * create w_av_frame from image file path
   * @returns the AVFrame
   */
  W_API
  static boost::leaf::result<w_av_frame>
  load_from_img_file(_In_ const std::filesystem::path &p_path,
                     _In_ AVPixelFormat p_pixel_fmt);

  /**
   * save to to the image file
   * @returns zero on success
   */
  W_API
  boost::leaf::result<int>
  save_to_img_file(_In_ const std::filesystem::path &p_path);

#endif

private:
  // move implementation
  void _move(w_av_frame &&p_other) noexcept;
  // initialization
  void _init() noexcept;

  // copy constructor.
  w_av_frame(const w_av_frame &) = delete;
  // copy assignment operator.
  w_av_frame &operator=(const w_av_frame &) = delete;

  void _release() noexcept;

  // the pixel format of ffmpeg AVFrame
  w_av_config _config = {};

  // the ffmpeg AVFrame
  AVFrame *_av_frame = nullptr;
  uint8_t *_buffer = nullptr;
};
} // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG
