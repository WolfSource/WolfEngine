/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_MEDIA_OPENAL)

#pragma once

#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <media/w_openal.hpp>

BOOST_AUTO_TEST_CASE(openal_play_wav) {
  using wolf::media::w_openal;
  using wolf::media::w_openal_config;

  const wolf::system::w_leak_detector _detector = {};

  // TODO (pooya: openAL has memoy leak https://github.com/kcat/openal-soft/issues/782)
  auto a = w_openal();
  a.init(w_openal_config{});

  auto _current_dir =
      std::filesystem::current_path().append("../../content/audio/sine.wav");
  std::cout << _current_dir.string();

  BOOST_REQUIRE(std::filesystem::exists(_current_dir) == true);

  // let file_name = "sine.wav";
  // let wolf_dir = current_dir.join("wolf");
  // let file_path = if wolf_dir.exists() { wolf_dir.join(file_name) }
  // else {current_dir.join(file_name)};

  // make sure constructor works. later update with wav header.
  //auto openal = w_openal{
  //  w_openal_config{
  //    .format = AL_FORMAT_STEREO16,
  //    .sample_rate = 48000,
  //    .channels = 1
  //  }
  //};
}

#endif
