/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf/wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
}
#include <DISABLE_ANALYSIS_END>

namespace wolf::media::ffmpeg {

class w_av_config {
 public:
  W_API w_av_config(_In_ AVPixelFormat p_format = AVPixelFormat::AV_PIX_FMT_NONE,
                    _In_ int p_width = 0, _In_ int p_height = 0) noexcept
      : format(p_format), width(p_width), height(p_height) {
    av_channel_layout_default(&this->channel_layout, 2);
  }

  W_API ~w_av_config() noexcept { av_channel_layout_uninit(&this->channel_layout); }

  /**
   * @returns the required buffer size for frame
   */
  W_API int get_required_buffer_size() noexcept;

  // the format of av frame
  AVPixelFormat format = AVPixelFormat::AV_PIX_FMT_NONE;
  // the width of av frame
  int width = 0;
  // the height of av frame
  int height = 0;
  // data alignment
  int alignment = 1;
  // the sample rate of the audio
  int sample_rate = 44100;
  // the sample format of the audio
  AVSampleFormat sample_fmts = AV_SAMPLE_FMT_S16;
  // the channel layout of the audio
  AVChannelLayout channel_layout;
};

class w_decoder;
class w_encoder;

class w_av_frame {
  friend w_decoder;
  friend w_encoder;

public:
  /**
   * constructor the av_frame with default config
   * @param p_config, the av frame config
   */
  //
  W_API w_av_frame() noexcept = default;

  /**
   * constructor the av_frame with specific config
   * @param p_config, the av frame config
   */
  //
  W_API explicit w_av_frame(_In_ const w_av_config &p_config) noexcept;

  // move constructor.
  W_API w_av_frame(_Inout_ w_av_frame &&p_other) noexcept;
  // move assignment operator.
  W_API w_av_frame &operator=(w_av_frame &&p_other) noexcept;

  // destructor
  W_API virtual ~w_av_frame() noexcept;

  /**
   * initialize the avframe
   * @returns zero on success
   */
  W_API
  boost::leaf::result<int> init() noexcept;

  /**
   * set the AVFrame data
   * @param p_data, the initial data of ffmpeg AVFrame
   * @param p_alignment, the alignment
   * @returns zero on success
   */
  W_API boost::leaf::result<int> set(_Inout_ std::optional<uint8_t **> p_data = {}) noexcept;

  /**
   * set the AVFrame's pts
   * @param p_pts, the pts data
   * @returns void
   */
  W_API void set_pts(_In_ int64_t p_pts) noexcept;

  /**
   * get data and linesize as a tuple
   * @returns tuple<int*[8], int[8]>
   */
  W_API
  std::tuple<uint8_t **, int *> get() const noexcept;

  /**
   * convert the ffmpeg video AVFrame
   * @returns the converted instance of AVFrame
   */
  W_API
  boost::leaf::result<w_av_frame>
  convert_video(_In_ const w_av_config &p_dst_config);

  /**
   * convert the ffmpeg audio AVFrame
   * @returns the converted instance of AVFrame
   */
  W_API
  boost::leaf::result<w_av_frame>
  convert_audio(_In_ w_av_config &p_dst_config);

  /**
   * @returns av_config
   */
  W_API w_av_config get_config() const noexcept;

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
   * @param p_quality, quality will be used only for jpeg and is between 1 and
   * 100
   * @returns zero on success
   */
  W_API
  boost::leaf::result<int>
  save_to_img_file(_In_ const std::filesystem::path &p_path,
                   int p_quality = 100);

private:
  // copy constructor.
  w_av_frame(const w_av_frame &) = delete;
  // copy assignment operator.
  w_av_frame &operator=(const w_av_frame &) = delete;

  // move implementation
  void _move(w_av_frame &&p_other) noexcept;
  // release
  void _release() noexcept;

  // the pixel format of ffmpeg AVFrame
  w_av_config _config = {};

  // the ffmpeg AVFrame
  AVFrame *_av_frame = nullptr;
  uint8_t *_moved_data = nullptr;
};
} // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG
