/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_MEDIA_OPENAL) && defined(WOLF_MEDIA_FFMPEG)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <media/w_openal.hpp>

BOOST_AUTO_TEST_CASE(openal_play_wav) {
  const wolf::system::w_leak_detector _detector = {};
  using w_openal = wolf::media::w_openal;

  auto _current_dir =
      std::filesystem::current_path().append("../../content/audio/sine.wav");
  std::cout << _current_dir.string();

  BOOST_REQUIRE(std::filesystem::exists(_current_dir) == true);

  // let file_name = "sine.wav";
  // let wolf_dir = current_dir.join("wolf");
  // let file_path = if wolf_dir.exists() { wolf_dir.join(file_name) }
  // else {current_dir.join(file_name)};
}

#endif