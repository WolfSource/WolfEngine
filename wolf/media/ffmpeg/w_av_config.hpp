/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf/wolf.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

namespace wolf::media::ffmpeg {
class w_av_config {
 public:
#pragma region Constructors /Destructor
  // default constructor
  W_API w_av_config() noexcept = default;

  // constructor for av video format
  W_API w_av_config(_In_ AVPixelFormat p_format, _In_ int p_width, _In_ int p_height,
                    _In_ int p_alignment = 1) noexcept;

  // constructor for av audio format
  W_API w_av_config(_In_ int p_nb_channels, _In_ AVSampleFormat p_sample_fmts,
                    _In_ int p_sample_rate) noexcept;

  // destructor
  W_API virtual ~w_av_config() noexcept = default;

  // move constructor.
  W_API w_av_config(w_av_config &&p_other) noexcept = default;
  // move assignment operator.
  W_API w_av_config &operator=(w_av_config &&p_other) noexcept = default;

  // copy constructor
  w_av_config(const w_av_config &p_other) noexcept = default;
  // copy assignment operator
  w_av_config &operator=(const w_av_config &p_other) noexcept = default;
#pragma endregion

  /**
   * @returns the required buffer size for video frame
   */
  W_API int get_required_video_buffer_size() const noexcept;

  // the format of av frame
  AVPixelFormat format = AVPixelFormat::AV_PIX_FMT_NONE;
  // the width of av frame
  int width = 0;
  // the height of av frame
  int height = 0;
  // data alignment
  int alignment = 0;
  // the sample rate of the audio
  int sample_rate = 0;
  // the sample format of the audio
  AVSampleFormat sample_fmts = AVSampleFormat::AV_SAMPLE_FMT_NONE;
  // number of channels
  int nb_channels = 0;
};
}  // namespace wolf::media::ffmpeg 

#endif  // WOLF_MEDIA_FFMPEG