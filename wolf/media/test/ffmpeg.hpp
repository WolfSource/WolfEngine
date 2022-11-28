/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TESTS) && defined(WOLF_MEDIA_FFMPEG) && defined(WOLF_MEDIA_STB)

#pragma once

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>

#include <media/ffmpeg/w_ffmpeg.hpp>
#include <media/ffmpeg/w_encoder.hpp>

BOOST_AUTO_TEST_CASE(av1_encode_test) {
  const wolf::system::w_leak_detector _detector = {};

  using w_av_frame = wolf::media::ffmpeg::w_av_frame;
  using w_av_config = wolf::media::ffmpeg::w_av_config;
  using w_ffmpeg = wolf::media::ffmpeg::w_ffmpeg;
  using w_av_set_opt = wolf::media::ffmpeg::w_av_set_opt;
  using w_av_codec_opt = wolf::media::ffmpeg::w_av_codec_opt;
  using w_av_packet = wolf::media::ffmpeg::w_av_packet;

  // create av frame from img
  const auto _current_dir =
      std::filesystem::current_path().append("../../content/texture/rgb.png");
  auto _src_frame_res = w_av_frame::load_from_img_file(
      _current_dir, AVPixelFormat::AV_PIX_FMT_RGBA);
  BOOST_REQUIRE(_src_frame_res.has_error() == false);
  BOOST_REQUIRE(_src_frame_res.has_value() == true);

  auto _src_frame = &_src_frame_res.value();
  const auto _config = _src_frame->get_config();

  w_av_config _encode_config = {AVPixelFormat::AV_PIX_FMT_YUV420P,
                                _config.width, _config.height};
  auto _yuv_frame_res = _src_frame->convert(_encode_config);
  BOOST_REQUIRE(_yuv_frame_res.has_error() == false);
  BOOST_REQUIRE(_yuv_frame_res.has_value() == true);
  auto _yuv_frame = &_yuv_frame_res.value();

  const auto _codec_opt = w_av_codec_opt{
      4'000'000, /*bitrate*/
      60,        /*fps*/
      600,       /*gop*/
      -1,        /*level*/
      2,         /*max_b_frames*/
      3,         /*refs*/
      -1,        /*thread_count*/
  };

  // for more info read https://trac.ffmpeg.org/wiki/Encode/AV1
  const auto _opts = std::vector<w_av_set_opt>{
      w_av_set_opt{"preset", 12},
      w_av_set_opt{"crf", 50},
  };

  auto _encoder_res =
      w_ffmpeg::create_encoder(_encode_config, "libsvtav1", _codec_opt, _opts);
  BOOST_REQUIRE(_encoder_res.has_error() == false);
  BOOST_REQUIRE(_encoder_res.has_value() == true);

   auto _packet = w_av_packet();
   const auto _packet_init_res = _packet.init();
   BOOST_REQUIRE(_packet_init_res.has_error() == false);

   auto _encoder = &_encoder_res.value();
   auto _encode_res = _encoder->start(*_yuv_frame, _packet);
   BOOST_REQUIRE(_encode_res.has_error() == false);
   BOOST_REQUIRE(_encode_res.has_value() == true);
}

#endif