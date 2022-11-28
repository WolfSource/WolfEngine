#ifdef WOLF_MEDIA_FFMPEG

#include "w_av_packet.hpp"

using w_av_packet = wolf::media::ffmpeg::w_av_packet;

w_av_packet::w_av_packet() noexcept : _packet(nullptr) {}

w_av_packet::~w_av_packet() noexcept { _release(); }

boost::leaf::result<int> w_av_packet::init() noexcept {
  if (this->_packet != nullptr) {
    _release();
  }
  this->_packet = av_packet_alloc();
  if (this->_packet == nullptr) {
    return W_ERR(std::errc::not_enough_memory,
                 "could not allocate memory for avpacket");
  }
  return W_SUCCESS;
}

int w_av_packet::get_size() const noexcept { return this->_packet->size; }

void w_av_packet::_release() noexcept {
  if (this->_packet != nullptr) {
    av_packet_free(&this->_packet);
    this->_packet = nullptr;
  }
}

#endif // WOLF_MEDIA_FFMPEG
