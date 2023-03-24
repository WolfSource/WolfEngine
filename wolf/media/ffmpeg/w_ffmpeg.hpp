/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include "w_av_packet.hpp"
#include "w_ffmpeg_ctx.hpp"
#include "w_encoder.hpp"
#include "w_decoder.hpp"
#include <variant>
#include <functional>

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

class w_ffmpeg {
 public:
  /*
   * create ffmpeg encoder
   * @param p_config, the video config
   * @param p_id, the avcodec id
   * @param p_codec_opts, the codec settings
   * @param p_opts, the codec options
   * @returns encoder object on success
   */
  W_API static boost::leaf::result<w_encoder> create_encoder(
      _In_ const w_av_config &p_config, _In_ AVCodecID p_id,
      _In_ const w_av_codec_opt &p_codec_opts,
      _In_ const std::vector<w_av_set_opt> &p_opts = {}) noexcept;

  /*
   * create ffmpeg encoder
   * @param p_config, the video config
   * @param p_id, the avcodec id in string (e.g. "libsvtav1", "libvpx")
   * @param p_codec_opts, the codec settings
   * @param p_opts, the codec options
   * @returns encoder object on success
   */
  W_API static boost::leaf::result<w_encoder> create_encoder(
      _In_ const w_av_config &p_config, _In_ const std::string &p_id,
      _In_ const w_av_codec_opt &p_codec_opts,
      _In_ const std::vector<w_av_set_opt> &p_opts = {}) noexcept;

  /*
   * create ffmpeg decoder
   * @param p_config, the avconfig
   * @param p_id, the avcodec id in string (e.g. "libsvtav1", "libvpx")
   * @param p_codec_opts, the codec options
   * @param p_opts, the codec options
   * @returns encoder object on success
   */
  W_API static boost::leaf::result<w_decoder> create_decoder(
      _In_ const w_av_config &p_config, _In_ AVCodecID p_id,
      _In_ const w_av_codec_opt &p_codec_opts,
      _In_ const std::vector<w_av_set_opt> &p_opts = {}) noexcept;

  /*
   * create ffmpeg decoder
   * @param p_config, the avconfig
   * @param p_id, the avcodec id in string (e.g. "libsvtav1", "libvpx")
   * @param p_codec_opts, the codec settings
   * @param p_opts, the codec options
   * @returns encoder object on success
   */
  W_API static boost::leaf::result<w_decoder> create_decoder(
      _In_ const w_av_config &p_config, _In_ const std::string &p_id,
      _In_ const w_av_codec_opt &p_codec_opts,
      _In_ const std::vector<w_av_set_opt> &p_opts = {}) noexcept;

  /*
   * open and receive stream from file or url
   * @param p_url, the url
   * @param p_opts, the codec options
   * @param p_on_frame, on frame data recieved callback
   * @returns encoder object on success
   */
  W_API static boost::leaf::result<int> open_stream_receiver(
      _In_ const std::string &p_url, _In_ const std::vector<w_av_set_opt> &p_opts,
      _In_ const std::function<bool(const w_av_packet & /*p_packet*/,
                                    const std::vector<AVStream *> & /*p_streams*/,
                                    const int /*p_video_stream_index*/,
                                    const int /*p_audio_stream_index*/)> &p_on_frame) noexcept;
};
}  // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG