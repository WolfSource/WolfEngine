#ifdef WOLF_MEDIA_FFMPEG

#include "w_encoder.hpp"

using w_encoder = wolf::media::ffmpeg::w_encoder;
using w_av_packet = wolf::media::ffmpeg::w_av_packet;

boost::leaf::result<int>
w_encoder::start(_In_ const w_av_frame &p_frame,
                 _Inout_ w_av_packet &p_packet) noexcept {

  av_packet_unref(p_packet._packet);

  for (;;) {
    auto _ret = avcodec_send_frame(this->ctx.codec_ctx, p_frame._av_frame);
    if (_ret < 0) {
      return W_FAILURE(std::errc::operation_canceled,
                       "failed to send the avframe for encoding because " +
                           w_ffmpeg_ctx::get_av_error_str(_ret));
    }

    for (;;) {
      _ret = avcodec_receive_packet(this->ctx.codec_ctx, p_packet._packet);
      if (_ret == 0 || _ret == AVERROR_EOF) {
        return 0;
      } else if (_ret == AVERROR(EAGAIN)) {
        break;
      } else {
        return W_FAILURE(std::errc::operation_canceled,
                         "error happened during the encoding because " +
                             w_ffmpeg_ctx::get_av_error_str(_ret));
      }
      av_packet_unref(p_packet._packet);
    }
  }

  return p_packet._packet->size;
}

#endif // WOLF_MEDIA_FFMPEG