#include "av_frame.hpp"

#include <DISABLE_ANALYSIS_BEGIN>
extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}
#include <DISABLE_ANALYSIS_END>

using w_av_frame = wolf::media::ffmpeg::w_av_frame;
using w_av_frame_config = wolf::media::ffmpeg::w_av_frame_config;

w_av_frame::w_av_frame(_In_ w_av_frame_config &&p_config) noexcept
    : _config(std::move(p_config)) {

  if (this->_config.width > 0 && this->_config.height > 0) {
    this->_av_frame = av_frame_alloc();
    const auto _av_frame_nn = gsl::narrow_cast<AVFrame *>(this->_av_frame);
    _av_frame_nn->format = gsl::narrow_cast<int>(this->_config.format);
    _av_frame_nn->width = this->_config.width;
    _av_frame_nn->height = this->_config.height;
  }
}

w_av_frame::~w_av_frame() noexcept { _release(); }

void w_av_frame::_release() noexcept {
  if (this->_av_frame != nullptr) {
    av_frame_free(&this->_av_frame);
    this->_av_frame = nullptr;
  }
}

boost::leaf::result<int> w_av_frame::set(_In_ const uint8_t *p_data,
                                         _In_ int p_alignment) noexcept {
  const auto _width = this->_config.width;
  const auto _height = this->_config.height;

  if (_width <= 0 || _height <= 0) {
    return W_ERR(std::errc::invalid_argument,
                 "width or height of w_av_frame is zero");
  }

  const auto _av_frame_nn = gsl::narrow_cast<AVFrame *>(this->_av_frame);
  const auto _size =
      av_image_fill_arrays(gsl::narrow_cast<uint8_t **>(_av_frame_nn->data),
                           gsl::narrow_cast<int *>(_av_frame_nn->linesize),
                           p_data, this->_config.format, _av_frame_nn->width,
                           _av_frame_nn->height, p_alignment);

  return W_SUCCESS;
}

 std::tuple<uint8_t **, int *> w_av_frame::get() const noexcept {
  const auto _av_frame_nn = gsl::narrow_cast<AVFrame *>(this->_av_frame);
  return std::make_tuple(_av_frame_nn->data, _av_frame_nn->linesize);
}

size_t w_av_frame::get_required_buffer_size(_In_ AVPixelFormat p_pixel_format,
                                            _In_ int p_width, _In_ int p_height,
                                            _In_ int p_alignment) noexcept {
  auto _size =
      av_image_get_buffer_size(p_pixel_format, p_width, p_height, p_alignment);
  return _size > 0 ? gsl::narrow_cast<size_t>(_size) : 0;
}

boost::leaf::result<w_av_frame>
w_av_frame::convert(_In_ w_av_frame_config &&p_dst_config) {

  w_av_frame _dst_frame(std::move(p_dst_config));
  const auto _dst_frame_nn = gsl::narrow_cast<AVFrame *>(_dst_frame._av_frame);

  auto *_context = sws_getContext(
      this->_config.width, this->_config.height, this->_config.format,
      _dst_frame._config.width, _dst_frame._config.height,
      _dst_frame._config.format, SWS_BICUBIC, nullptr, nullptr, nullptr);

  // check the memory
  const auto _context_nn = gsl::not_null<SwsContext *>(_context);

  const auto _height = sws_scale(
      _context_nn, static_cast<const uint8_t *const *>(this->_av_frame->data),
      gsl::narrow_cast<const int *>(this->_av_frame->linesize), 0,
      this->_config.height,
      gsl::narrow_cast<uint8_t *const *>(_dst_frame_nn->data),
      gsl::narrow_cast<const int *>(_dst_frame_nn->linesize));

  // free context
  sws_freeContext(_context_nn);

  if (_height < 0) {
    return W_ERR(std::errc::invalid_argument, "w_av_frame sws_scale failed");
  }

  return _dst_frame;
}
