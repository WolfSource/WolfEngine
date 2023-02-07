/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_MEDIA_FFMPEG)

#pragma once

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <wolf.hpp>

#include <media/ffmpeg/w_ffmpeg.hpp>
#include <media/w_openal.hpp>

BOOST_AUTO_TEST_CASE(ffmpeg_stream_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "ffmpeg_stream_test is running" << std::endl;

  using w_av_set_opt = wolf::media::ffmpeg::w_av_set_opt;
  const auto _opt = std::vector<w_av_set_opt>();

  constexpr auto _url =
      "srt://127.0.0.1:554?mode=caller&transtype=live&timeout=5000000";

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        using namespace std::chrono_literals;
        using w_av_packet = wolf::media::ffmpeg::w_av_packet;
        using w_ffmpeg = wolf::media::ffmpeg::w_ffmpeg;
        using w_av_config = wolf::media::ffmpeg::w_av_config;
        using w_av_codec_opt = wolf::media::ffmpeg::w_av_codec_opt;
        using w_av_frame = wolf::media::ffmpeg::w_av_frame;
        using w_openal = wolf::media::w_openal;

        // frame index
        int _index = 0;
        // 5 seconds for io socket timeout
        const auto t0 = std::chrono::high_resolution_clock::now();
        bool _video_initialized = false;
        bool _audio_initialized = false;
        wolf::media::ffmpeg::w_decoder _video_decoder = {};
        wolf::media::ffmpeg::w_decoder _audio_decoder = {};
        wolf::media::ffmpeg::w_av_frame _av_video_frame = {};
        wolf::media::ffmpeg::w_av_frame _av_audio_frame = {};
        wolf::media::ffmpeg::w_av_config _audio_config = {};
        wolf::media::w_openal_config _openal_config = {};
        //wolf::media::w_openal _openal = {};
        auto _width = 0;
        auto _height = 0;

        auto _on_frame_cb = [&](const w_av_packet &p_packet,
                                const std::vector<AVStream *> &p_streams,
                                const int p_video_stream_index,
                                const int p_audio_stream_index) -> bool {
          const auto _stream_index = p_packet.get_stream_index();
          if (_stream_index == p_video_stream_index) {
            std::cout << "got video frame" << std::endl;

            if (_video_initialized == false) {
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

              _video_decoder = std::move(_decoder_res.value());

              // create destination avframe for decoder
              _av_video_frame = w_av_frame(_config);
              auto _decoded_frame_res = _av_video_frame.init();
              if (_decoded_frame_res.has_error()) {
                std::cout << "could not initialize decoder" << std::endl;
                return false; // close it
              }

              _video_initialized = true;
            }

            auto _res = _video_decoder.decode(p_packet, _av_video_frame);
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

              auto _rgba_frame = _av_video_frame.convert_video(_dst_config);
              if (_rgba_frame.has_error()) {
                std::cout << "could not convert av video frame to rgb frame"
                          << std::endl;
              } else {
                const auto _path = std::filesystem::current_path().append(
                    "/" + std::to_string(_index++) + "_rist_decoded.png");
               // _rgba_frame.value().save_to_img_file(_path);
                std::cout << "rgb frame created" << std::endl;
              }
            }
          } else if (_stream_index == p_audio_stream_index) {
            std::cout << "got audio frame" << std::endl;

            if (_audio_initialized == false) {
              auto _codec_param = p_streams[p_audio_stream_index]->codecpar;

              const auto _codec_id = _codec_param->codec_id;
              _audio_config.channels = _codec_param->channels;
              _audio_config.sample_rate = _codec_param->sample_rate;

              const auto _codec_opt = w_av_codec_opt{};

              auto _decoder_res = w_ffmpeg::create_decoder(
                  _audio_config, _codec_id, _codec_opt);
              if (_decoder_res.has_error()) {
                return false; // close it
              }

              _audio_config.sample_fmts = gsl::narrow_cast<AVSampleFormat>(
                  *_decoder_res->ctx.codec->sample_fmts);
              _audio_decoder = std::move(_decoder_res.value());

              // create destination avframe for decoder
              _av_audio_frame = w_av_frame(_audio_config);
              auto _decoded_frame_res = _av_audio_frame.init();
              if (_decoded_frame_res.has_error()) {
                std::cout << "could not initialize decoder" << std::endl;
                return false; // close it
              }

              _openal_config = {AL_FORMAT_STEREO16, _audio_config.sample_rate,
                                25, _audio_config.channels};

              //auto _ret = _openal.init(_openal_config);

              //if (_ret.has_error()) {
              //  std::cout << "could not initialize openal" << std::endl;
              //  return false; // close it
              //}

              _audio_initialized = true;
            }

            auto _res = _audio_decoder.decode(p_packet, _av_audio_frame);
            if (_res.has_error()) {
              std::cout << "could not decode" << std::endl;
            } else {
              auto _frame = _av_audio_frame.get();
              std::cout << "audio size: "
                        << gsl::narrow_cast<int>(*std::get<1>(_frame))
                        << std::endl;

              auto _dst_config = _audio_config;
              _dst_config.sample_fmts = AV_SAMPLE_FMT_S16;
              _dst_config.channel_layout = AV_CH_LAYOUT_STEREO;

              auto _audio_frame = _av_audio_frame.convert_audio(_dst_config);
              if (_audio_frame.has_error()) {
                std::cout << "could not convert av audio frame" << std::endl;
              } else {
                std::cout << "audio frame created" << std::endl;

                auto _content = _audio_frame.value().get();
                auto _data = std::get<0>(_content);
                auto _size = std::get<1>(_content);

                if (_data  && *_data && _size) {
                  /*const auto _ret = _openal.update(
                      gsl::narrow_cast<const uint8_t *>(*_data), *_size);
                  if (_ret.has_error()) {
                    std::cout << "could not update openal because " << std::endl;
                  }*/
                }
              }
            }
          }

          const auto t1 = std::chrono::high_resolution_clock::now();
          if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0) > 20s) {
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

  std::cout << "ffmpeg_stream_test is done" << std::endl;
}

#endif