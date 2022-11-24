/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TESTS) && defined(WOLF_MEDIA_FFMPEG) && defined(WOLF_MEDIA_STB)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#pragma once

#include <media/ffmpeg/w_av_frame.hpp>
#include <media/ffmpeg/w_av_packet.hpp>

BOOST_AUTO_TEST_CASE(avframe_test) {
  const wolf::system::w_leak_detector _detector = {};
  using w_av_frame = wolf::media::ffmpeg::w_av_frame;
  using w_av_config = wolf::media::ffmpeg::w_av_config;

  // create av frame from img
  const auto _current_dir =
      std::filesystem::current_path().append("../../content/texture/rgb.png");
  auto _src_frame_res = w_av_frame::load_from_img_file(
      _current_dir, AVPixelFormat::AV_PIX_FMT_RGBA);
  BOOST_REQUIRE(_src_frame_res.has_error() == false);
  BOOST_REQUIRE(_src_frame_res.has_value() == true);

  auto _src_frame = &_src_frame_res.value();
  const auto _config = _src_frame->get_config();

  w_av_config _dst_config = {AVPixelFormat::AV_PIX_FMT_BGRA, _config.width,
                             _config.height};
  auto _dst_frame_res = _src_frame->convert(_dst_config);
  BOOST_REQUIRE(_dst_frame_res.has_error() == false);
  BOOST_REQUIRE(_dst_frame_res.has_value() == true);

  const auto _new_path =
      std::filesystem::current_path().append("/rgb_converted.png");
  const auto _dst_frame = &_dst_frame_res.value();
  auto _ret = _dst_frame->save_to_img_file(_new_path);
  BOOST_REQUIRE(_ret.has_error() == false);
}

#endif