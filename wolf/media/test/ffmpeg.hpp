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

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        // create av frame from img
        const auto _current_dir = std::filesystem::current_path().append(
            "../../content/texture/rgb.png");
        BOOST_LEAF_AUTO(_src_frame,
                        w_av_frame::load_from_img_file(
                            _current_dir, AVPixelFormat::AV_PIX_FMT_RGBA));

        // source & destination configs
        const auto _src_config = _src_frame.get_config();
        const auto _dst_config =
            w_av_config{AVPixelFormat::AV_PIX_FMT_YUV420P, _src_config.width,
                        _src_config.height};

        BOOST_LEAF_AUTO(_yuv_frame, _src_frame.convert(_dst_config));

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

        // encode frame to packet
        BOOST_LEAF_AUTO(_encoder,
                        w_ffmpeg::create_encoder(_dst_config, "libsvtav1",
                                                 _codec_opt, _opts));
        auto _packet = w_av_packet();
        BOOST_LEAF_AUTO(_packet_init_res, _packet.init());

        for (;;) {
          BOOST_LEAF_AUTO(_encode_result, _encoder.start(_yuv_frame, _packet));

          // break when we got the encoded packet
          if (_encode_result > 0) {
            break;
          }
        }

        // create destination avframe for decoder
        auto _decoded_frame = w_av_frame(_dst_config);
        BOOST_LEAF_AUTO(_frame_ret, _decoded_frame.init());

        // create a decoder
        BOOST_LEAF_AUTO(_decoder,
                        w_ffmpeg::create_decoder(_decoded_frame.get_config(),
                                                 "libdav1d", _codec_opt, {}));

        BOOST_LEAF_AUTO(_result, _decoder.start(_packet, _decoded_frame));

        BOOST_LEAF_AUTO(final_frame, _decoded_frame.convert(_src_config));

        const auto _new_path =
            std::filesystem::current_path().append("/rgb_decoded.png");
        BOOST_LEAF_AUTO(final_result,
                        final_frame.save_to_img_file(_new_path));

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
}

#endif