/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_MEDIA_OPENAL) && defined(WOLF_MEDIA_FFMPEG)

#pragma once

#include <gtest.hpp> 
#include <media/w_openal.hpp>

TEST(openal, play_wav) {
  const wolf::system::w_leak_detector _detector = {};
  using w_openal = wolf::media::w_openal;

  auto _current_dir =
      std::filesystem::current_path().append("../../content/sine.wav");
  std::cout << _current_dir.string();

  if (std::filesystem::exists(_current_dir)) {
  }

  // let file_name = "sine.wav";
  // let wolf_dir = current_dir.join("wolf");
  // let file_path = if wolf_dir.exists() { wolf_dir.join(file_name) }
  // else {current_dir.join(file_name)};
}

#endif