#ifdef WOLF_MEDIA_STB

#include "w_image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#pragma region w_image_data

using w_image_data = wolf::media::w_image_data;

w_image_data::w_image_data(w_image_data &&p_other) noexcept {
  _move(std::move(p_other));
}

// move assignment operator.
w_image_data &w_image_data::operator=(w_image_data &&p_other) noexcept {
  _move(std::move(p_other));
  return *this;
}

w_image_data::~w_image_data() noexcept {
  this->width = 0;
  this->height = 0;
  this->comp = 0;
  if (this->raw_data != nullptr) {
    stbi_image_free(this->raw_data);
    this->raw_data = nullptr;
  }
}

void w_image_data::_move(w_image_data &&p_other) noexcept {
  this->width = p_other.width;
  this->height = p_other.height;
  this->comp = p_other.comp;
  this->raw_data = std::exchange(p_other.raw_data, nullptr);
}

#pragma endregion

using w_image = wolf::media::w_image;

boost::leaf::result<w_image_data>
w_image::load(_In_ const std::filesystem::path &p_path,
              int p_requested_component) {
  if (p_path.empty()) {
    return W_FAILURE(std::errc::invalid_argument, "could not load image");
  }

  const auto _path_str = p_path.string();

  w_image_data _data = {};
  _data.raw_data = stbi_load(_path_str.c_str(), &_data.width, &_data.height,
                             &_data.comp, p_requested_component);
  if (_data.raw_data == nullptr) {
    return W_FAILURE(std::errc::operation_canceled,
                     "could not load image from file: " + _path_str);
  }

  return _data;
}

boost::leaf::result<int>
w_image::save_bmp(_In_ const std::filesystem::path &p_path,
                  _In_ const w_image_data &p_image_data) {
  if (p_path.empty() || !std::filesystem::exists(p_path.parent_path())) {
    return W_FAILURE(std::errc::invalid_argument,
                     "could not save bmp file to the following path: " +
                         p_path.string());
  }
  return stbi_write_bmp(p_path.string().c_str(), p_image_data.width,
                        p_image_data.height, p_image_data.comp,
                        p_image_data.raw_data);
}

boost::leaf::result<int>
w_image::save_hdr(_In_ const std::filesystem::path &p_path,
                  _In_ const gsl::span<float> p_data, _In_ uint32_t p_width,
                  _In_ uint32_t p_height, _In_ uint32_t p_comp) {

  if (p_path.empty() || !std::filesystem::exists(p_path.parent_path())) {
    return W_FAILURE(std::errc::invalid_argument,
                     "could not save hdr file to the the following path: " +
                         p_path.string());
  }
  return stbi_write_hdr(p_path.string().c_str(), p_width, p_height, p_comp,
                        p_data.data());
}

boost::leaf::result<int>
w_image::save_jpg(_In_ const std::filesystem::path &p_path,
                  _In_ const w_image_data &p_image_data,
                  _In_ uint32_t p_quality) {
  if (p_path.empty() || !std::filesystem::exists(p_path.parent_path())) {
    return W_FAILURE(std::errc::invalid_argument,
                     "could not save jpg file to the following path: " +
                         p_path.string());
  }
  return stbi_write_jpg(p_path.string().c_str(), p_image_data.width,
                        p_image_data.height, p_image_data.comp,
                        p_image_data.raw_data, p_quality);
}

boost::leaf::result<int>
w_image::save_png(_In_ const std::filesystem::path &p_path,
                  _In_ const w_image_data &p_image_data,
                  _In_ uint32_t p_stride) {
  if (p_path.empty() || !std::filesystem::exists(p_path.parent_path())) {
    return W_FAILURE(std::errc::invalid_argument,
                     "could not save png file to the following path: " +
                         p_path.string());
  }
  return stbi_write_png(p_path.string().c_str(), p_image_data.width,
                        p_image_data.height, p_image_data.comp,
                        p_image_data.raw_data, p_stride);
}

#endif


