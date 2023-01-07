#ifdef WOLF_MEDIA_STB

#include "w_image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using w_image = wolf::media::w_image;

//boost::leaf::result<int> w_image::save_to_img_file(_In_ const std::filesystem::path &p_path,
//                             int p_quality) {
//  if (this->_av_frame == nullptr || this->_av_frame->width == 0 ||
//      this->_av_frame->height == 0) {
//    return W_FAILURE(std::errc::invalid_argument, "bad parameters for avframe");
//  }
//
//  const auto _path = p_path.string();
//  auto _ext = p_path.extension().string();
//  std::transform(_ext.cbegin(), _ext.cend(), _ext.begin(), tolower);
//
//  const auto _comp = this->_av_frame->linesize[0] / this->_av_frame->width;
//  if (_ext == ".bmp") {
//
//    return stbi_write_bmp(_path.c_str(), this->_config.width,
//                          this->_config.height, _comp,
//                          this->_av_frame->data[0]);
//  } else if (_ext == ".png") {
//
//    return stbi_write_png(_path.c_str(), this->_config.width,
//                          this->_config.height, _comp, this->_av_frame->data[0],
//                          this->_av_frame->linesize[0]);
//  } else if (_ext == ".jpg" || _ext == ".jpeg") {
//
//    return stbi_write_jpg(_path.c_str(), this->_config.width,
//                          this->_config.height, _comp, this->_av_frame->data[0],
//                          p_quality);
//  } else {
//    return W_FAILURE(std::errc::invalid_argument,
//                     "image format not supported for " + _path);
//  }
//}

//boost::leaf::result<int> w_image::save_png() {
//  // return stbi_write_png(_path.c_str(), this->_config.width,
//  //                       this->_config.height, _comp,
//  //                       this->_av_frame->data[0],
//  //                       this->_av_frame->linesize[0]);
//
//  return 0;
//}

#endif


