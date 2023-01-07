/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_MEDIA_FFMPEG)

#pragma once

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <wolf.hpp>

#include <media/ffmpeg/w_ffmpeg.hpp>

BOOST_AUTO_TEST_CASE(ffmpeg_stream_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "ffmpeg_stream_test is running" << std::endl;

  using w_av_set_opt = wolf::media::ffmpeg::w_av_set_opt;
  const auto _opt = std::vector<w_av_set_opt>();
  constexpr auto _url =
      "srt://20.71.33.86:554?mode=caller&transtype=live&timeout=5000000";

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        using namespace std::chrono_literals;
        using w_av_packet = wolf::media::ffmpeg::w_av_packet;
        using w_ffmpeg = wolf::media::ffmpeg::w_ffmpeg;
        using w_av_config = wolf::media::ffmpeg::w_av_config;
        using w_av_codec_opt = wolf::media::ffmpeg::w_av_codec_opt;
        using w_av_frame = wolf::media::ffmpeg::w_av_frame;

        // frame index
        int _index = 0;
        // 5 seconds for io socket timeout
        const auto t0 = std::chrono::high_resolution_clock::now();
        bool _initialized = false;
        wolf::media::ffmpeg::w_decoder _v_decoder = {};
        wolf::media::ffmpeg::w_av_frame _av_frame = {};
        auto _width = 0;
        auto _height = 0;

        auto _on_frame_cb = [&](const w_av_packet &p_packet,
                                const std::vector<AVStream *>& p_streams,
                                const int p_video_stream_index,
                                const int p_audio_stream_index) -> bool {
          const auto _stream_index = p_packet.get_stream_index();
          if (_stream_index == p_video_stream_index) {
            std::cout << "got video frame" << std::endl;

            if (_initialized == false) {
              auto _codec_param = p_streams[p_video_stream_index]->codecpar;
              _width = p_streams[p_video_stream_index]->codecpar->width;
              _height = p_streams[p_video_stream_index]->codecpar->height;

              const auto _codec_id = _codec_param->codec_id;
              const auto _fmt =
                  gsl::narrow_cast<AVPixelFormat>(_codec_param->format);

              const auto _config = w_av_config{
                  // the format of av frame
                  _fmt,
                  // the width of av frame
                  _width,
                  // the height of av frame
                  _height,
                  // data alignment
                  1,
              };

              const auto _codec_opt = w_av_codec_opt{
                  _codec_param->bit_rate, /*bitrate*/
                  60,                     /*fps*/
                  600,                    /*gop*/
                  _codec_param->level,    /*level*/
                  2,                      /*max_b_frames*/
                  3,                      /*refs*/
                  -1,                     /*thread_count*/
              };

              auto _decoder_res =
                  w_ffmpeg::create_decoder(_config, _codec_id, _codec_opt);
              if (_decoder_res.has_error()) {
                return false; // close it
              }

              _v_decoder = std::move(_decoder_res.value());

              // create destination avframe for decoder
              _av_frame = w_av_frame(_config);
              auto _decoded_frame_res = _av_frame.init();
              if (_decoded_frame_res.has_error()) {
                std::cout << "could not initialize decoder" << std::endl;
                return false; // close it
              }

              _initialized = true;
            }

            auto _res = _v_decoder.decode(p_packet, _av_frame);
            if (_res.has_error()) {
              std::cout << "could not decode" << std::endl;
            } else {
              const auto _dst_config = w_av_config{
                  // the format of av frame
                  AVPixelFormat::AV_PIX_FMT_RGBA,
                  // the width of av frame
                  _width,
                  // the height of av frame
                  _height,
                  // data alignment
                  1,
              };

              auto _rgba_frame = _av_frame.convert(_dst_config);
              if (_rgba_frame.has_error()) {
                std::cout << "could not convert to rgb video frame"
                          << std::endl;
              } else {
                const auto _path = std::filesystem::current_path().append(
                    "/" + std::to_string(_index++) + "_rist_decoded.png");
                _rgba_frame.value().save_to_img_file(_path);
                std::cout << "rgb frame created" << std::endl;
              }
            }
          } else if (_stream_index == p_audio_stream_index) {
            std::cout << "got audio frame" << std::endl;
          }

          const auto t1 = std::chrono::high_resolution_clock::now();
          if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0) > 5s) {
            return false;
          }
          return true;
        };

        BOOST_CHECK(w_ffmpeg::open_stream_receiver(_url, _opt, _on_frame_cb));

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

  std::cout << "ffmpeg_stream_test just done" << std::endl;
}

#endif