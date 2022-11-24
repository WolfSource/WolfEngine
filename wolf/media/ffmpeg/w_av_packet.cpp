#ifdef WOLF_MEDIA_FFMPEG

#include "w_av_packet.hpp"

using w_av_packet = wolf::media::ffmpeg::w_av_packet;

w_av_packet::w_av_packet() noexcept : _packet(nullptr) {}

w_av_packet::~w_av_packet() noexcept { _release(); }

boost::leaf::result<int> w_av_packet::init() noexcept {
  if (this->_packet != nullptr) {
    _release();
  }
  this->_packet.reset(av_packet_alloc());
  if (this->_packet == nullptr) {
    return W_ERR(std::errc::not_enough_memory,
                 "could not allocate memory for avpacket");
  }
  return W_SUCCESS;
}

void w_av_packet::_release() noexcept {
  if (this->_packet != nullptr) {
    auto _ptr = this->_packet.get();
    av_packet_free(&_ptr);
  }
}

#endif // WOLF_MEDIA_FFMPEG
