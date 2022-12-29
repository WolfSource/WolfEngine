/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_STREAM_RIST)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <stream/rist/w_rist.hpp>

BOOST_AUTO_TEST_CASE(rist_data_block_test) {
  const wolf::system::w_leak_detector _detector = {};
  using w_rist_data_block = wolf::stream::rist::w_rist_data_block;

  std::cout << "rist_data_block_test is running" << std::endl;

  for (size_t i = 0; i < 5; i++) {
    std::string _msg = "hello";
    const auto _block = std::make_tuple((void *)_msg.data(), _msg.size());

    auto _data_block = w_rist_data_block();
    _data_block.set(_block);
    auto _new_block = _data_block.get();

    const auto new_msg = std::string_view(
        gsl::narrow_cast<const char *>(std::get<0>(_new_block)),
        std::get<1>(_new_block));
    BOOST_REQUIRE(new_msg == "hello");
  }

  std::cout << "rist_data_block_test just done" << std::endl;
}

BOOST_AUTO_TEST_CASE(rist_sender_receiver_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "rist_sender_receiver_test is running" << std::endl;

  using w_rist = wolf::stream::rist::w_rist;
  using w_rist_data_block = wolf::stream::rist::w_rist_data_block;
  using namespace std::chrono_literals;

  constexpr auto _timeout_in_sec = 5s;
  constexpr auto _mode = rist_profile::RIST_PROFILE_MAIN;
  constexpr auto _loss = 0;

  std::thread thread_sender([&]() {
    boost::leaf::try_handle_all(
        [&]() -> boost::leaf::result<void> {
          // create a rist sender
          auto _sender = w_rist(rist_ctx_mode::RIST_SENDER_MODE, _mode, _loss,
                                rist_log_level::RIST_LOG_DEBUG);
          auto _ret = _sender.init();
          BOOST_REQUIRE(_ret.has_error() == false);

          constexpr auto _url =
              "rist://"
              "127.0.0.1:1234?cname=wolf&rtt-max=10&rtt-min=1&secret="
              "12345678&aes-type=256&bandwidth=6000";
          _ret = _sender.connect(_url);
          BOOST_REQUIRE(_ret.has_error() == false);

          const auto t0 = std::chrono::high_resolution_clock::now();
          auto _index = 0;
          static std::once_flag _once_flag;

          for (;;) {

            std::string _msg =
                "hello from RIST stream: #" + std::to_string(_index++);
            const auto _block =
                std::make_tuple((void *)_msg.data(), _msg.size());

            auto _data_block = w_rist_data_block();
            _data_block.set(_block);

            auto _sent = _sender.send(_data_block);
            BOOST_REQUIRE(_sent.has_error() == false);

            const auto t1 = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0) >
                _timeout_in_sec) {
              break;
            }
            std::this_thread::sleep_for(std::chrono::duration(1ms));
          }

          return {};
        },
        [](const w_trace &p_trace) {
          std::cout << "got error :" << p_trace << std::endl;
          BOOST_REQUIRE(false);
        },
        [] {
          std::cout << "got an error!" << std::endl;
          BOOST_ERROR(false);
        });
  });

  std::thread thread_receiver([&]() {
    std::this_thread::sleep_for(std::chrono::duration(1s));

    auto _receiver = w_rist(rist_ctx_mode::RIST_RECEIVER_MODE, _mode, _loss,
                            rist_log_level::RIST_LOG_DEBUG);
    auto _ret = _receiver.init();
    BOOST_REQUIRE(_ret.has_error() == false);

    _receiver.on_receiver_data_callback =
        [](_In_ const w_rist_data_block &p_data_block) {
          std::cout << p_data_block.to_string() << std::endl;
        };

    constexpr auto url =
        "rist://@"
        "127.0.0.1:1234?cname=wolf&rtt-max=10&rtt-min=1&secret="
        "12345678&aes-type=256&bandwidth=6000";
    _ret = _receiver.connect(url);
    BOOST_REQUIRE(_ret.has_error() == false);

    const auto t0 = std::chrono::high_resolution_clock::now();
    for (;;) {
      const auto t1 = std::chrono::high_resolution_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0) >
          _timeout_in_sec) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::duration(1ms));
    }
  });

  thread_sender.join();
  thread_receiver.join();

  std::cout << "rist_sender_receiver_test just done" << std::endl;
}

//#ifdef WOLF_MEDIA_FFMPEG
//
//#include <media/ffmpeg/w_ffmpeg.hpp>
//
//BOOST_AUTO_TEST_CASE(rist_receiver_ffmpeg_test) {
//  const wolf::system::w_leak_detector _detector = {};
//
//  std::cout << "rist_receiver_ffmpeg_test is running" << std::endl;
//
//  boost::leaf::try_handle_all(
//      [&]() -> boost::leaf::result<void> {
//        using namespace std::chrono_literals;
//        using w_av_set_opt = wolf::media::ffmpeg::w_av_set_opt;
//        using w_av_packet = wolf::media::ffmpeg::w_av_packet;
//        using w_ffmpeg = wolf::media::ffmpeg::w_ffmpeg;
//        using w_av_config = wolf::media::ffmpeg::w_av_config;
//        using w_av_codec_opt = wolf::media::ffmpeg::w_av_codec_opt;
//        using w_av_frame = wolf::media::ffmpeg::w_av_frame;
//
//        auto _opt = std::vector<w_av_set_opt>();
//        const auto t0 = std::chrono::high_resolution_clock::now();
//
//        auto _on_frame_cb = [t0](const w_av_packet &p_packet,
//                                 const std::vector<AVStream *> p_streams,
//                                 const int p_video_stream_index,
//                                 const int p_audio_stream_index) -> bool {
//          const auto _stream_index = p_packet.get_stream_index();
//          if (_stream_index == p_video_stream_index) {
//            std::cout << "got video frame" << std::endl;
//
//            auto _codec_param = p_streams[p_video_stream_index]->codecpar;
//            const auto _codec_id = _codec_param->codec_id;
//            const auto _fmt =
//                gsl::narrow_cast<AVPixelFormat>(_codec_param->format);
//            const auto _width = _codec_param->width;
//            const auto _height = _codec_param->height;
//
//            const auto _config = w_av_config{
//                // the format of av frame
//                _fmt,
//                // the width of av frame
//                _width,
//                // the height of av frame
//                _height,
//                // data alignment
//                1,
//            };
//
//            const auto _codec_opt = w_av_codec_opt{
//                _codec_param->bit_rate, /*bitrate*/
//                60,                     /*fps*/
//                600,                    /*gop*/
//                _codec_param->level,    /*level*/
//                2,                      /*max_b_frames*/
//                3,                      /*refs*/
//                -1,                     /*thread_count*/
//            };
//
//            auto _decoder_res =
//                w_ffmpeg::create_decoder(_config, _codec_id, _codec_opt);
//            if (_decoder_res.has_error()) {
//              return false; // close it
//            }
//
//            auto _decoder = &_decoder_res.value();
//
//            // create destination avframe for decoder
//            auto _decoded_frame = w_av_frame(_config);
//            auto _decoded_frame_res = _decoded_frame.init();
//            if (_decoded_frame_res.has_error()) {
//              return false; // close it
//            }
//
//            auto _res = _decoder->start(p_packet, _decoded_frame);
//            if (_res.has_error()) {
//              return false; // close it
//            }
//
//            const auto _dst_config = w_av_config{
//                // the format of av frame
//                AVPixelFormat::AV_PIX_FMT_RGBA,
//                // the width of av frame
//                _width,
//                // the height of av frame
//                _height,
//                // data alignment
//                1,
//            };
//            auto _rgba_frame = _decoded_frame.convert(_dst_config);
//            if (_rgba_frame.has_error()) {
//              return false; // close it
//            }
//
//            const auto _path =
//                std::filesystem::current_path().append("/rist_decoded.png");
//            _rgba_frame.value().save_to_img_file(_path);
//
//          } else if (_stream_index == p_audio_stream_index) {
//            std::cout << "got audio frame" << std::endl;
//          }
//
//          const auto t1 = std::chrono::high_resolution_clock::now();
//          if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0) > 5s) {
//            return false;
//          }
//          return true;
//        };
//
//        BOOST_CHECK(w_ffmpeg::open_stream_receiver(
//            "rist://@127.0.0.1:1235?cname=wolf&bandwidth=6000", _opt,
//            _on_frame_cb));
//
//        return {};
//      },
//      [](const w_trace &p_trace) {
//        std::cout << "got error :" << p_trace << std::endl;
//        BOOST_REQUIRE(false);
//      },
//      [] {
//        std::cout << "got an error!" << std::endl;
//        BOOST_ERROR(false);
//      });
//
//  std::cout << "rist_receiver_ffmpeg_test just done" << std::endl;
//}
//
//#endif

#endif