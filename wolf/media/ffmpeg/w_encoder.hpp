/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include "w_av_frame.hpp"
#include "w_av_packet.hpp"
#include "w_ffmpeg_ctx.hpp"
#include <variant>

namespace wolf::media::ffmpeg {

class w_encoder {
public:
  // constructor
  W_API w_encoder() = default;
  // destructor
  W_API virtual ~w_encoder() noexcept = default;

  // move constructor.
  W_API w_encoder(w_encoder &&p_other) noexcept = default;
  // move assignment operator.
  W_API w_encoder &operator=(w_encoder &&p_other) noexcept = default;

  W_API boost::leaf::result<int> encode(_In_ const w_av_frame &p_frame,
                                        _Inout_ w_av_packet &p_packet,
                                        _In_ bool p_flush = true) noexcept;

  w_ffmpeg_ctx ctx = {};

private:
  // copy constructor
  w_encoder(const w_encoder &) = delete;
  // copy operator
  w_encoder &operator=(const w_encoder &) = delete;

  boost::leaf::result<int>
  _encode_frame_to_packet(_In_ const AVFrame *p_frame,
          _Inout_ std::vector<uint8_t> &p_packet_data) const noexcept;
};
} // namespace wolf::media::ffmpeg

#endif // WOLF_MEDIA_FFMPEG