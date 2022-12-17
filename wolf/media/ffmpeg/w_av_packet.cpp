#ifdef WOLF_MEDIA_FFMPEG

#include "w_av_packet.hpp"

using w_av_packet = wolf::media::ffmpeg::w_av_packet;

w_av_packet::w_av_packet() noexcept : _packet(nullptr) {}

w_av_packet::w_av_packet(AVPacket *p_av_packet) noexcept
    : _packet(p_av_packet) {}

w_av_packet::~w_av_packet() noexcept { _release(); }

boost::leaf::result<int> w_av_packet::init() noexcept {
  if (this->_packet != nullptr) {
    _release();
  }
  this->_packet = av_packet_alloc();
  if (this->_packet == nullptr) {
    return W_FAILURE(std::errc::not_enough_memory,
                     "could not allocate memory for avpacket");
  }
  return S_OK;
}

boost::leaf::result<int> w_av_packet::init(_In_ uint8_t *p_data,
                                           _In_ size_t p_data_len) noexcept {
  if (this->_packet != nullptr) {
    _release();
  }
  this->_packet = av_packet_alloc();
  if (this->_packet == nullptr) {
    return W_FAILURE(std::errc::not_enough_memory,
                     "could not allocate memory for avpacket");
  }
  this->_packet->data = p_data;
  this->_packet->size = gsl::narrow_cast<int>(p_data_len);
  return S_OK;
}

void w_av_packet::unref() noexcept { av_packet_unref(this->_packet); }

uint8_t *w_av_packet::get_data() const noexcept { return this->_packet->data; }

int w_av_packet::get_size() const noexcept { return this->_packet->size; }

int w_av_packet::get_stream_index() const noexcept {
  return this->_packet->stream_index;
}

void w_av_packet::_release() noexcept {
  if (this->_packet != nullptr) {
    av_packet_free(&this->_packet);
    this->_packet = nullptr;
  }
}

#endif // WOLF_MEDIA_FFMPEG
