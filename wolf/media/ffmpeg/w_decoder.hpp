/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include "w_av_frame.hpp"
#include "w_av_packet.hpp"
#include "w_ffmpeg_ctx.hpp"
#include <variant>

namespace wolf::media::ffmpeg {

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

  W_API boost::leaf::result<int>
  start(_In_ const w_av_packet &p_packet, _Inout_ w_av_frame &p_frame) noexcept;

private:
  // copy constructor
  w_decoder(const w_decoder &) = delete;
  // copy operator
  w_decoder &operator=(const w_decoder &) = delete;

  boost::leaf::result<int> _get_frame_from_packet(_In_ AVPacket *p_packet,
                                                  _Inout_ w_av_frame &p_frame);
};
} // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG