/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_MEDIA_FFMPEG) && defined(WOLF_MEDIA_STB)

#pragma once

#include <gtest.hpp> 
#include <media/ffmpeg/av_frame.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TEST(openal, play_wav) {
  const wolf::system::w_leak_detector _detector = {};
  using w_av_frame = wolf::media::ffmpeg::w_av_frame;

  int _width = 0;
  int _height = 0;
  int _comp = 0;
  auto *_raw_img_data = stbi_load("", &_width, &_height, &_comp, 0);
}

#endif