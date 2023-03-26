#ifdef WOLF_MEDIA_FFMPEG

#include "w_av_format.hpp"
#include "w_av_frame.hpp"

using w_av_format = wolf::media::ffmpeg::w_av_format;

w_av_format::w_av_format() noexcept
    : _stream_buffer(nullptr), _fmt_ctx(nullptr), _io_ctx(nullptr) {}

void w_av_format::_release() noexcept {
  if (this->_stream_buffer != nullptr) {
    auto _ptr = this->_stream_buffer.get();
    free(_ptr);
    this->_stream_buffer = nullptr;
  }
  if (this->_fmt_ctx != nullptr) {
    auto _ptr = this->_fmt_ctx.get();
    avformat_close_input(&_ptr);
    this->_stream_buffer = nullptr;
  }
  if (this->_io_ctx != nullptr) {
    auto _ptr = this->_io_ctx.get();
    av_free(_ptr);
    this->_stream_buffer = nullptr;
  }
}

static int s_read_packet(void *p_opaque, _Inout_ uint8_t *p_buf, _In_ int p_buf_size) {
  auto _av_fmt = gsl::narrow_cast<w_av_format *>(p_opaque);
  if (_av_fmt) {
    if (_av_fmt->on_read_callback) {
      return _av_fmt->on_read_callback(p_buf, p_buf_size);
    }
  }
  return -1;  // failed
}

boost::leaf::result<int> w_av_format::init(_In_ int p_stream_buf_size) noexcept {
  _release();

  // alloc a buffer for the stream
  auto _ptr = gsl::narrow_cast<uint8_t *>(malloc(p_stream_buf_size));
  if (_ptr == nullptr) {
    // out of memory
    return W_FAILURE(std::errc::not_enough_memory, "could not allocate memory for stream buffer");
  }
  this->_stream_buffer.reset(_ptr);

  // get a AVContext stream
  auto _io_ctx_ptr =
      avio_alloc_context(this->_stream_buffer.get(),  // buffer
                         p_stream_buf_size,           // buffer size
                         0,              // buffer is only readable - set to 1 for read/write
                         this,           // use your specified data
                         s_read_packet,  // function - reading Packets (see example)
                         nullptr,        // function - write Packets
                         nullptr         // function - seek to position in stream (see example)
      );
  if (_io_ctx_ptr == nullptr) {
    // out of memory
    return W_FAILURE(std::errc::not_enough_memory, "could not allocate io context");
  }
  this->_io_ctx.reset(_io_ctx_ptr);

  // allocate a AVContext
  auto _fmt_ctx_ptr = avformat_alloc_context();
  if (_fmt_ctx_ptr == nullptr) {
    // out of memory
    return W_FAILURE(std::errc::not_enough_memory, "could not allocate avformat context");
  }
  this->_fmt_ctx.reset(_fmt_ctx_ptr);

  // Set up the format context based on custom IO
  this->_fmt_ctx->pb = _io_ctx_ptr;
  this->_fmt_ctx->flags |= AVFMT_FLAG_CUSTOM_IO;

  // open "file" (open our custom IO)
  // empty string is where filename would go. doesn't matter since we aren't
  // reading a file NULL params are format and demuxer settings, respectively
  if (avformat_open_input(&_fmt_ctx_ptr, "", nullptr, nullptr) < 0) {
    // error on opening input
    return W_FAILURE(std::errc::not_enough_memory, "could not allocate io context");
  }

  // find the stream info
  if (avformat_find_stream_info(_fmt_ctx_ptr, nullptr) < 0) {
    // Error on finding stream info
    return W_FAILURE(std::errc::operation_canceled, "could not get stream info");
  }

  // find best stream
  const auto _ret =
      av_find_best_stream(_fmt_ctx_ptr, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (_ret < 0) {
    // Error on finding stream info
    return W_FAILURE(std::errc::operation_canceled, "could not find best stream");
  }

  return 0;
}

uint8_t *w_av_format::get_io_ctx_buffer() const {
  if (this->_io_ctx) {
    return this->_io_ctx->buffer;
  }
  return nullptr;
}

int w_av_format::get_io_ctx_size() const {
  if (this->_io_ctx) {
    return this->_io_ctx->buffer_size;
  }
  return -1;
}

#endif // WOLF_MEDIA_FFMPEG