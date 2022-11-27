/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include "w_av_frame.hpp"
#include "w_av_packet.hpp"
#include <variant>

namespace wolf::media::ffmpeg {

struct w_av_codec_opt {
  int gop;
  int level;
  int max_b_frames;
  int refs;
  int thread_count;
  int fps;
  int64_t bitrate;
};

struct w_av_set_opt {
  // name of option
  std::string name;
  // value type
  std::variant<int, double, std::string> value;
};

struct w_ffmpeg_ctx {
  AVCodecID codec_id = AVCodecID::AV_CODEC_ID_NONE;
  AVCodecContext *context = nullptr;
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

private:
  // copy constructor
  w_ffmpeg_ctx(const w_ffmpeg_ctx &) = delete;
  // copy operator
  w_ffmpeg_ctx &operator=(const w_ffmpeg_ctx &) = delete;

  void _move(w_ffmpeg_ctx &&p_other) noexcept;
  void _release() noexcept;
};

struct w_encoder {
  w_ffmpeg_ctx ctx = {};

  // constructor
  W_API w_encoder() = default;
  // destructor
  W_API virtual ~w_encoder() noexcept = default;

  // move constructor.
  W_API w_encoder(w_encoder &&p_other) noexcept = default;
  // move assignment operator.
  W_API w_encoder &operator=(w_encoder &&p_other) noexcept = default;

private:
  // copy constructor
  w_encoder(const w_encoder &) = delete;
  // copy operator
  w_encoder &operator=(const w_encoder &) = delete;
};

struct w_decoder {
  w_ffmpeg_ctx ctx = {};

  // constructor
  W_API w_decoder() = default;
  // destructor
  W_API virtual ~w_decoder() noexcept = default;

  // move constructor.
  W_API w_decoder(w_decoder &&p_other) noexcept = default;
  // move assignment operator.
  W_API w_decoder &operator=(w_decoder &&p_other) noexcept = default;

private:
  // copy constructor
  w_decoder(const w_decoder &) = delete;
  // copy operator
  w_decoder &operator=(const w_decoder &) = delete;
};

struct w_ffmpeg {
  /*
   * create ffmpeg encoder
   * @param p_frame, the avframe object
   * @param p_id, the avcodec id
   * @param p_settings, the codec settings
   * @param p_opts, the codec options
   * @returns encoder object on success
   */
  W_API static boost::leaf::result<w_encoder>
  create_encoder(_In_ const w_av_frame &p_frame, _In_ const std::string &p_id,
                 _In_ const w_av_codec_opt &p_settings,
                 _In_ const std::vector<w_av_set_opt> &p_opts) noexcept;

  /*
   * create ffmpeg decoder
   * @param p_frame, the avframe object
   * @param p_id, the avcodec id
   * @param p_settings, the codec settings
   * @param p_opts, the codec options
   * @returns encoder object on success
   */
  W_API static boost::leaf::result<w_decoder>
  create_decoder(_In_ const w_av_frame &p_frame, _In_ const std::string &p_id,
                 _In_ const w_av_codec_opt &p_settings,
                 _In_ const std::vector<w_av_set_opt> &p_opts) noexcept;
};
} // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG