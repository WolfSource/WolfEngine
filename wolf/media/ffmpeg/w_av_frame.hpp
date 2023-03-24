/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf/wolf.hpp>
#include "w_av_config.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

namespace wolf::media::ffmpeg {

class w_decoder;
class w_encoder;

class w_av_frame {
  friend w_decoder;
  friend w_encoder;

 public:
  /**
   * constructor the av_frame with specific config
   * @param p_config, the av audio config
   */
  W_API explicit w_av_frame(_In_ w_av_config &&p_config) noexcept;

  // destructor
  W_API virtual ~w_av_frame() noexcept { _release(); }

  // move constructor.
  W_API w_av_frame(w_av_frame &&p_other) noexcept { _move(std::forward<w_av_frame &&>(p_other)); }
  // move assignment operator.
  W_API w_av_frame &operator=(w_av_frame &&p_other) noexcept {
    _move(std::forward<w_av_frame &&>(p_other));
    return *this;
  }

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
  W_API boost::leaf::result<int> set_video_frame(_Inout_ std::vector<uint8_t> &&p_data) noexcept;

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
  boost::leaf::result<w_av_frame> convert_video(_In_ w_av_config &&p_dst_config);

  /**
   * convert the ffmpeg audio AVFrame
   * @returns the converted instance of AVFrame
   */
  // W_API
  // boost::leaf::result<w_av_frame> convert_audio(_In_ std::unique_ptr<w_av_config> p_dst_config);

  /**
   * @returns config
   */
  W_API w_av_config get_config() const noexcept;

#ifdef WOLF_MEDIA_STB

  /**
   * create w_av_frame from image file path
   * @returns the AVFrame
   */
  W_API
  static boost::leaf::result<w_av_frame> load_video_frame_from_img_file(
      _In_ const std::filesystem::path &p_path, _In_ AVPixelFormat p_pixel_fmt);

  /**
   * save to to the image file
   * @param p_quality, quality will be used only for jpeg and is between 1 and
   * 100
   * @returns zero on success
   */
  W_API
  boost::leaf::result<int> save_video_frame_to_img_file(_In_ const std::filesystem::path &p_path,
                                                        int p_quality = 100) noexcept;

#endif

 private:
  // copy constructor.
  w_av_frame(const w_av_frame &) = delete;
  // copy assignment operator.
  w_av_frame &operator=(const w_av_frame &) = delete;

  // release
  void _release() noexcept;
  // move
  void _move(w_av_frame &&p_other) noexcept;

  // the channel layout of the audio
  AVChannelLayout _channel_layout = {};
  // the AVFrame config
  w_av_config _config = {};
  // the ffmpeg AVFrame
  gsl::owner<AVFrame *> _av_frame = nullptr;
  // the ffmpeg AVFrame data
  std::vector<uint8_t> _data = {};
};
}  // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG
