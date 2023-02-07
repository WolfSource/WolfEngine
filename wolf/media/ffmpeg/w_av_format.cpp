#ifdef WOLF_MEDIA_FFMPEG

#include "w_av_format.hpp"


#include "w_av_frame.hpp"

using w_av_format = wolf::media::ffmpeg::w_av_format;

w_av_format::w_av_format() noexcept
    : _fmt_ctx(nullptr), _io_ctx(nullptr), _stream_buffer(nullptr) {}

w_av_format::w_av_format(w_av_format &&p_other) noexcept {
  _move(std::move(p_other));
}

w_av_format &w_av_format::operator=(w_av_format &&p_other) noexcept {
  _move(std::move(p_other));
  return *this;
}

void w_av_format::_move(w_av_format &&p_other) noexcept {
  if (this == &p_other)
    return;

  _release();

  this->_fmt_ctx = std::exchange(p_other._fmt_ctx, nullptr);
  this->_io_ctx = std::exchange(p_other._io_ctx, nullptr);
}

w_av_format::~w_av_format() noexcept { _release(); }

void w_av_format::_release() noexcept {
  if (this->_stream_buffer != nullptr) {
    free(this->_stream_buffer);
    this->_stream_buffer = nullptr;
  }
  if (this->_fmt_ctx != nullptr) {
    avformat_close_input(&this->_fmt_ctx);
    this->_fmt_ctx = nullptr;
  }
  if (this->_io_ctx != nullptr) {
    av_free(this->_io_ctx);
    this->_io_ctx = nullptr;
  }
}

static int s_read_packet(void *opaque, uint8_t *buf, int buf_size) {
  auto _av_fmt = gsl::narrow_cast<w_av_format *>(opaque);
  if (_av_fmt) {
    if (_av_fmt->on_read_callback) {
      return _av_fmt->on_read_callback(buf, buf_size);
    }
  }
  return -1; // failed
}

boost::leaf::result<int>
w_av_format::init(_In_ int p_stream_buf_size) noexcept {

  _release();

  // alloc a buffer for the stream
  this->_stream_buffer = gsl::owner<uint8_t *>(malloc(p_stream_buf_size));
  if (this->_stream_buffer == nullptr) {
    // out of memory
    return W_FAILURE(std::errc::not_enough_memory,
                     "could not allocate memory for stream buffer");
  }

  // Get a AVContext stream
  this->_io_ctx = avio_alloc_context(
      _stream_buffer,    // buffer
      p_stream_buf_size, // buffer size
      0,                 // buffer is only readable - set to 1 for read/write
      this,              // use your specified data
      s_read_packet,     // function - reading Packets (see example)
      nullptr,           // function - write Packets
      nullptr            // function - seek to position in stream (see example)
  );
  if (_io_ctx == nullptr) {
    // out of memory
    return W_FAILURE(std::errc::not_enough_memory,
                     "could not allocate io context");
  }

  // Allocate a AVContext
  this->_fmt_ctx = avformat_alloc_context();

  // Set up the format context based on custom IO
  this->_fmt_ctx->pb = _io_ctx;
  this->_fmt_ctx->flags |= AVFMT_FLAG_CUSTOM_IO;

  // open "file" (open our custom IO)
  // empty string is where filename would go. doesn't matter since we aren't
  // reading a file NULL params are format and demuxer settings, respectively
  if (avformat_open_input(&this->_fmt_ctx, "", nullptr, nullptr) < 0) {
    // Error on opening input
    return W_FAILURE(std::errc::not_enough_memory,
                     "could not allocate io context");
  }

  if (avformat_find_stream_info(this->_fmt_ctx, nullptr) < 0) {
    // Error on finding stream info
    return W_FAILURE(std::errc::operation_canceled,
                     "could not get stream info");
  }

  const auto _ret = av_find_best_stream(this->_fmt_ctx, AVMediaType::AVMEDIA_TYPE_VIDEO, -1,
                          -1, nullptr, 0);
  if (_ret < 0) {
    // Error on finding stream info
    return W_FAILURE(std::errc::operation_canceled,
                     "could not find best stream");
  }

  return 0;
}

#endif // WOLF_MEDIA_FFMPEG