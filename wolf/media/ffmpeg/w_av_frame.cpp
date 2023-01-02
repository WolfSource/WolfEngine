#ifdef WOLF_MEDIA_FFMPEG

#include "w_av_frame.hpp"

#include <DISABLE_ANALYSIS_BEGIN>
extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#ifdef WOLF_MEDIA_STB

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#endif //WOLF_MEDIA_STB

#include <DISABLE_ANALYSIS_END>

using w_av_frame = wolf::media::ffmpeg::w_av_frame;
using w_av_config = wolf::media::ffmpeg::w_av_config;

int w_av_config::get_required_buffer_size() noexcept {
  return av_image_get_buffer_size(this->format, this->width, this->height,
                                  this->alignment);
}

w_av_frame::w_av_frame(_In_ const w_av_config &p_config) noexcept
    : _config(p_config), _av_frame(nullptr), _moved_data(nullptr) {}

w_av_frame::w_av_frame(w_av_frame &&p_other) noexcept {
  _move(std::move(p_other));
}

w_av_frame &w_av_frame::operator=(w_av_frame &&p_other) noexcept {
  _move(std::move(p_other));
  return *this;
}

void w_av_frame::_move(w_av_frame &&p_other) noexcept {
  if (this == &p_other)
    return;

  _release();

  this->_config = std::move(p_other._config);
  this->_av_frame = std::exchange(p_other._av_frame, nullptr);
  this->_moved_data = std::exchange(p_other._moved_data, nullptr);
}

w_av_frame::~w_av_frame() noexcept { _release(); }

void w_av_frame::_release() noexcept {
  if (this->_moved_data != nullptr) {
    free(this->_moved_data);
    this->_moved_data = nullptr;
  }
  if (this->_av_frame != nullptr) {
    av_frame_free(&this->_av_frame);
    this->_av_frame = nullptr;
  }
}

boost::leaf::result<int> w_av_frame::init() noexcept { return set(nullptr); }

boost::leaf::result<int> w_av_frame::set(_Inout_ uint8_t **p_data) noexcept {

  const auto _width = this->_config.width;
  const auto _height = this->_config.height;
  const auto _alignment = this->_config.alignment;

  // check for width and height
  if (_width <= 0 || _height <= 0) {
    return W_FAILURE(std::errc::invalid_argument,
                     "width or height of w_av_frame is zero");
  }

  if (this->_av_frame == nullptr) {
    this->_av_frame = av_frame_alloc();
    // check for the memory
    const auto _av_frame_nn = gsl::narrow_cast<AVFrame *>(this->_av_frame);

    _av_frame_nn->format = gsl::narrow_cast<int>(this->_config.format);
    _av_frame_nn->width = this->_config.width;
    _av_frame_nn->height = this->_config.height;
  }

  const auto _av_frame_nn = gsl::narrow_cast<AVFrame *>(this->_av_frame);

  // move the owenership of data to buffer
  if (p_data == nullptr) {
    const auto _buffer_size = this->_config.get_required_buffer_size();
    this->_moved_data = gsl::owner<uint8_t *>(malloc(_buffer_size));
  } else {
    this->_moved_data = std::exchange(*p_data, nullptr);
  }
  const auto _size =
      av_image_fill_arrays(_av_frame_nn->data, _av_frame_nn->linesize,
                           gsl::narrow_cast<const uint8_t *>(this->_moved_data),
                           this->_config.format, _av_frame_nn->width,
                           _av_frame_nn->height, _alignment);

  return S_OK;
}
  
std::tuple<uint8_t **, int *> w_av_frame::get() const noexcept {
  const auto _av_frame_nn = gsl::narrow_cast<AVFrame *>(this->_av_frame);
  return std::make_tuple(_av_frame_nn->data, _av_frame_nn->linesize);
}

w_av_config w_av_frame::get_config() const noexcept { return this->_config; }

boost::leaf::result<w_av_frame>
w_av_frame::convert(_In_ const w_av_config &p_dst_config) {

  // create buffer and dst frame
  w_av_frame _dst_frame(p_dst_config);
  _dst_frame.set(nullptr);

  const auto _dst_avframe_nn =
      gsl::narrow_cast<AVFrame *>(_dst_frame._av_frame);

  auto *_context = sws_getContext(
      this->_config.width, this->_config.height, this->_config.format,
      _dst_frame._config.width, _dst_frame._config.height,
      _dst_frame._config.format, SWS_BICUBIC, nullptr, nullptr, nullptr);

  // check the memory
  const auto _context_nn = gsl::not_null<SwsContext *>(_context);

  const auto _height =
      sws_scale(_context_nn,
                gsl::narrow_cast<const uint8_t *const *>(this->_av_frame->data),
                gsl::narrow_cast<const int *>(this->_av_frame->linesize), 0,
                this->_config.height,
                gsl::narrow_cast<uint8_t *const *>(_dst_avframe_nn->data),
                gsl::narrow_cast<const int *>(_dst_avframe_nn->linesize));

  // free context
  sws_freeContext(_context_nn);

  if (_height < 0) {
    return W_FAILURE(std::errc::invalid_argument,
                     "w_av_frame sws_scale failed");
  }

  return _dst_frame;
}

#ifdef WOLF_MEDIA_STB

boost::leaf::result<w_av_frame>
w_av_frame::load_from_img_file(_In_ const std::filesystem::path &p_path,
                               _In_ AVPixelFormat p_pixel_fmt) {

  // width, height, comp
  int _width = 0;
  int _height = 0;
  int _comp = 0;

  const auto _path = p_path.string();
  if (std::filesystem::exists(p_path) == false) {
    return W_FAILURE(std::errc::invalid_argument,
                     " path not exist for av_frame" + _path);
  }

  auto *_raw_img_data = stbi_load(_path.c_str(), &_width, &_height, &_comp, 0);

  if (_raw_img_data == nullptr) {
    return W_FAILURE(std::errc::invalid_argument,
                     "could not load image file " + _path);
  }

  w_av_config _src_config = {p_pixel_fmt, _width, _height};
  // create an av_frame from image raw data
  w_av_frame _src_frame(_src_config);
  _src_frame.set(&_raw_img_data);

  return _src_frame;
}

boost::leaf::result<int>
w_av_frame::save_to_img_file(_In_ const std::filesystem::path &p_path,
                             int p_quality) {
  if (this->_av_frame == nullptr || this->_av_frame->width == 0 ||
      this->_av_frame->height == 0) {
    return W_FAILURE(std::errc::invalid_argument, "bad parameters for avframe");
  }

  const auto _path = p_path.string();
  auto _ext = p_path.extension().string();
  std::transform(_ext.cbegin(), _ext.cend(), _ext.begin(), tolower);

  const auto _comp = this->_av_frame->linesize[0] / this->_av_frame->width;
  if (_ext == ".bmp") {

    return stbi_write_bmp(_path.c_str(), this->_config.width,
                          this->_config.height, _comp,
                          this->_av_frame->data[0]);
  } else if (_ext == ".png") {

    return stbi_write_png(_path.c_str(), this->_config.width,
                          this->_config.height, _comp, this->_av_frame->data[0],
                          this->_av_frame->linesize[0]);
  } else if (_ext == ".jpg" || _ext == ".jpeg") {

    return stbi_write_jpg(_path.c_str(), this->_config.width,
                          this->_config.height, _comp, this->_av_frame->data[0],
                          p_quality);
  } else {
    return W_FAILURE(std::errc::invalid_argument,
                     "image format not supported for " + _path);
  }
}

#endif

#endif // WOLF_MEDIA_FFMPEG