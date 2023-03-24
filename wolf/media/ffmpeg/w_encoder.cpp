#ifdef WOLF_MEDIA_FFMPEG

#include "w_encoder.hpp"

using w_encoder = wolf::media::ffmpeg::w_encoder;
using w_av_packet = wolf::media::ffmpeg::w_av_packet;

boost::leaf::result<int> w_encoder::_encode_frame_to_packet(
    _In_ const AVFrame *p_frame, _Inout_ std::vector<uint8_t> &p_packet_data) const noexcept {
  auto _packet = w_av_packet();
  _packet.init();

  for (;;) {
    auto _ret = avcodec_send_frame(this->ctx.codec_ctx, p_frame);
    if (_ret < 0) {
      return W_FAILURE(std::errc::operation_canceled,
                       "failed to send the avframe for encoding because:\"" +
                           w_ffmpeg_ctx::get_av_error_str(_ret) + "\"");
    }

    for (;;) {
      _ret = avcodec_receive_packet(this->ctx.codec_ctx, _packet._packet);
      if (_ret == 0 || _ret == AVERROR_EOF) {
        if (_packet._packet->size) {
          std::copy(_packet._packet->data, _packet._packet->data + _packet._packet->size,
                    std::back_inserter(p_packet_data));
        }
        return 0;
      }

      _packet.unref();

      if (_ret == AVERROR(EAGAIN)) {
        break;
      }
      return W_FAILURE(std::errc::operation_canceled,
                       "error happened during the encoding because:\"" +
                           w_ffmpeg_ctx::get_av_error_str(_ret) + "\"");
    }
  }
  return 0;
}

boost::leaf::result<int> w_encoder::encode(_In_ const w_av_frame &p_frame,
                                           _Inout_ w_av_packet &p_packet,
                                           _In_ bool p_flush) noexcept {
  std::vector<uint8_t> _packet_data;

  // encode frame to packet
  BOOST_LEAF_CHECK(_encode_frame_to_packet(p_frame._av_frame, _packet_data));
  if (p_flush) {
    // flush
    BOOST_LEAF_CHECK(_encode_frame_to_packet(nullptr, _packet_data));
  }

  // init packet
  p_packet.init(std::move(_packet_data));

  return {};
}

#endif // WOLF_MEDIA_FFMPEG