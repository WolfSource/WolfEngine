/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf/wolf.hpp>

extern "C" {
#include <libavcodec/packet.h>
}

namespace wolf::media::ffmpeg {

class w_decoder;
class w_encoder;
class w_ffmpeg;

class w_av_packet {
  friend w_decoder;
  friend w_encoder;
  friend w_ffmpeg;

 public:
  // default construct an av_packet
  W_API w_av_packet() noexcept = default;

  /**
   * construct an av_packet
   */
  W_API explicit w_av_packet(_In_ AVPacket *p_av_packet) noexcept;

  // move constructor.
  W_API w_av_packet(w_av_packet &&p_other) noexcept {
    _move(std::forward<w_av_packet &&>(p_other));
  }
  // move assignment operator.
  W_API w_av_packet &operator=(w_av_packet &&p_other) noexcept {
    _move(std::forward<w_av_packet &&>(p_other));
    return *this;
  }

  // destructor
  W_API virtual ~w_av_packet() noexcept { _release(); }

  /**
   * initialize the av_packet
   * @returns zero on success
   */
  W_API boost::leaf::result<int> init() noexcept;

  /**
   * initialize the av_packet from data
   * @returns zero on success
   */
  W_API boost::leaf::result<int> init(_In_ uint8_t *p_data, _In_ size_t p_data_len) noexcept;

  /**
   * initialize the av_packet
   * @returns void
   */
  W_API boost::leaf::result<int> init(_Inout_ std::vector<uint8_t> &&p_data) noexcept;

  /**
   * unref av_packet
   */
  W_API void unref() noexcept;

  // get packet data
  W_API uint8_t *get_data() const noexcept;

  // get packet size
  W_API int get_size() const noexcept;

  // get stream index
  W_API int get_stream_index() const noexcept;

 private:
  // copy constructor.
  w_av_packet(const w_av_packet &) = delete;
  // copy assignment operator.
  w_av_packet &operator=(const w_av_packet &) = delete;
  // release the resources
  void _release() noexcept;
  // move the resources
  void _move(_Inout_ w_av_packet && p_other) noexcept;

  gsl::owner<AVPacket*> _packet = {};
  std::vector<uint8_t> _own_data;
};
}  // namespace wolf::media::ffmpeg

#endif