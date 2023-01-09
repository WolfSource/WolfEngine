#ifdef WOLF_MEDIA_STB

#include "w_image.hpp"

#include <DISABLE_ANALYSIS_BEGIN>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <DISABLE_ANALYSIS_END>

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

#endif


