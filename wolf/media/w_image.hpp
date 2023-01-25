/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_STB

#pragma once

#include "w_image_data.hpp"
#include <wolf/wolf.hpp>

namespace wolf::media {

class w_image_data {
public:
  // constructor
  W_API w_image_data() noexcept = default;

  // move constructor
  W_API w_image_data(w_image_data &&p_other) noexcept;

  // move assignment operator.
  W_API w_image_data &operator=(w_image_data &&p_other) noexcept;

  // destructor
  W_API ~w_image_data() noexcept;

  uint8_t *raw_data = nullptr;
  int width = -1;
  int height = -1;
  int comp = -1;

private:
  // copy constructor.
  w_image_data(const w_image_data &) = delete;
  // copy assignment operator.
  w_image_data &operator=(const w_image_data &) = delete;

  // move implementation
  void _move(w_image_data &&p_other) noexcept;
};

class w_image {
public:
  /**
   * load raw data from image path
   * p_path, the image path
   * p_requested_component, the requested component (default value is zero)
   * returns raw bytes on success as result format
   */
  W_API static boost::leaf::result<w_image_data>
  load(_In_ const std::filesystem::path &p_path, int p_requested_component = 0);

  /**
   * save raw data in the format of bmp
   * p_path, the destination path
   * p_image_data, the image data
   * returns zero on success as result format
   */
  W_API static boost::leaf::result<int>
  save_bmp(_In_ const std::filesystem::path &p_path,
           _In_ const w_image_data &p_image_data);

  /**
   * save raw data in the format of hdr
   * p_path, the destination path
   * p_data, the raw data of RGBA
   * p_width, the width of image
   * p_height, the height of image
   * p_comp, the component of image
   * returns zero on success as result format
   */
  W_API static boost::leaf::result<int>
  save_hdr(_In_ const std::filesystem::path &p_path,
           _In_ gsl::span<float> p_data, _In_ uint32_t p_width,
           _In_ uint32_t p_height, _In_ uint32_t p_comp);

  /**
   * save raw data in the format of jpg
   * p_path, the destination path
   * p_image_data, the image data
   * p_quality, the quality of jpeg (an integer value between 1 and 100.
        the higher quality looks better but results in a bigger image)
   * returns zero on success as result format
   */
  W_API static boost::leaf::result<int>
  save_jpg(_In_ const std::filesystem::path &p_path,
           _In_ const w_image_data &p_image_data, _In_ uint32_t p_quality);

  /**
   * save raw data in the format of png
   * p_path, the destination path
   * p_image_data, the image data
   * p_stride, the stride of data
   * returns zero on success as result format
   */
  W_API static boost::leaf::result<int>
  save_png(_In_ const std::filesystem::path &p_path,
           _In_ const w_image_data &p_image_data, _In_ uint32_t p_stride);
};
} // namespace wolf::media

#endif
