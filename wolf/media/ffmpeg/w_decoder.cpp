#ifdef WOLF_MEDIA_FFMPEG

#include "w_decoder.hpp"

using w_decoder = wolf::media::ffmpeg::w_decoder;

boost::leaf::result<int>
w_decoder::start(_In_ const w_av_packet &p_packet,
                 _Inout_ w_av_frame &p_frame) noexcept {

  auto _dst_packet = av_packet_alloc();
  if (_dst_packet == nullptr) {
    return W_FAILURE(std::errc::not_enough_memory,
                     "could not allocate packet for decoding");
  }

  boost::leaf::result<int> _ret = S_OK;

  for (;;) {
    const auto _bytes = av_parser_parse2(
        this->ctx.parser, this->ctx.codec_ctx, &_dst_packet->data,
        &_dst_packet->size, p_packet._packet->data, p_packet._packet->size,
        AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

    if (_bytes == 0) {
      break;
    } else if (_bytes < 0) {
      return W_FAILURE(std::errc::operation_canceled,
                       "could not parse packet for decoding");
    }

    p_packet._packet->data += _bytes;
    p_packet._packet->size -= _bytes;

    if (_dst_packet->size > 0) {
      _ret = _get_frame_from_packet(_dst_packet, p_frame);
      if (_ret.has_error()) {
        break;
      }
    }
  }

  av_packet_free(&_dst_packet);

  return _ret;
}

boost::leaf::result<int>
w_decoder::_get_frame_from_packet(_In_ AVPacket *p_packet,
                                  _Inout_ w_av_frame &p_frame) {

  // start decoding
  auto _ret = avcodec_send_packet(this->ctx.codec_ctx, p_packet);
  if (_ret < 0) {
      return W_FAILURE(std::errc::operation_canceled,
                       "could not parse packet for decoding because of " +
                           w_ffmpeg_ctx::get_av_error_str(_ret));
  }

  for (;;) {
      _ret = avcodec_receive_frame(this->ctx.codec_ctx, p_frame._av_frame);
      if (_ret == 0 || _ret == AVERROR(EAGAIN) || _ret == AVERROR_EOF) {
        break;
      }
      if (_ret < 0) {
        return W_FAILURE(std::errc::operation_canceled,
                         "error happened during the encoding because " +
                             w_ffmpeg_ctx::get_av_error_str(_ret));
      }
  }
  return _ret;
}

#endif // WOLF_MEDIA_FFMPEG