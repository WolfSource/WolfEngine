/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
extern "C" {
#include <libavcodec/packet.h>
}
#include <DISABLE_ANALYSIS_END>

namespace wolf::media::ffmpeg {

class w_av_packet {
public:
  /**
   * construct an an av_packet
   */
  W_API w_av_packet() noexcept;

  /**
   * construct an an av_packet
   */
  W_API boost::leaf::result<int> init() noexcept;

  // move constructor.
  W_API w_av_packet(w_av_packet &&p_other) noexcept = default;
  // move assignment operator.
  W_API w_av_packet &operator=(w_av_packet &&p_other) noexcept = default;

  // destructor
  W_API virtual ~w_av_packet() noexcept;

private:
  // copy constructor.
  w_av_packet(const w_av_packet &) = delete;
  // copy assignment operator.
  w_av_packet &operator=(const w_av_packet &) = delete;

  void _release() noexcept;
  // move implementation
  void _move(w_av_packet &&p_other) noexcept;

  std::unique_ptr<AVPacket> _packet = nullptr;
};
} // namespace wolf::media::ffmpeg

#endif