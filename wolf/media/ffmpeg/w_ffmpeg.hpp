/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include "w_ffmpeg_ctx.hpp"
#include "w_encoder.hpp"
#include "w_decoder.hpp"
#include <variant>

namespace wolf::media::ffmpeg {

struct w_av_codec_opt {
  int64_t bitrate;
  int fps;
  int gop;
  int level;
  int max_b_frames;
  int refs;
  int thread_count;
};

struct w_av_set_opt {
  // name of option
  std::string name;
  // value type
  std::variant<int, double, std::string> value;
};

struct w_ffmpeg {
  /*
   * create ffmpeg encoder
   * @param p_config, the avconfig
   * @param p_id, the avcodec id
   * @param p_settings, the codec settings
   * @param p_opts, the codec options
   * @returns encoder object on success
   */
  W_API static boost::leaf::result<w_encoder>
  create_encoder(_In_ const w_av_config &p_config, _In_ const std::string &p_id,
                 _In_ const w_av_codec_opt &p_settings,
                 _In_ const std::vector<w_av_set_opt> &p_opts) noexcept;

  /*
   * create ffmpeg decoder
   * @param p_config, the avconfig
   * @param p_id, the avcodec id
   * @param p_settings, the codec settings
   * @param p_opts, the codec options
   * @returns encoder object on success
   */
  W_API static boost::leaf::result<w_decoder>
  create_decoder(_In_ const w_av_config &p_config, _In_ const std::string &p_id,
                 _In_ const w_av_codec_opt &p_settings,
                 _In_ const std::vector<w_av_set_opt> &p_opts) noexcept;
};
} // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG