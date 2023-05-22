/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_MEDIA_FFMPEG) && defined(WOLF_MEDIA_STB)

#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <media/ffmpeg/w_av_frame.hpp>
#include <media/ffmpeg/w_av_packet.hpp>

BOOST_AUTO_TEST_CASE(avframe_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "entering test case 'avframe_test'" << std::endl;

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        using w_av_frame = wolf::media::ffmpeg::w_av_frame;
        using w_av_config = wolf::media::ffmpeg::w_av_config;

        // create av frame from img
        const std::filesystem::path path =
            std::filesystem::current_path().append("../../content/texture/rgb.png");

        BOOST_LEAF_AUTO(_src_frame, w_av_frame::load_video_frame_from_img_file(
                                        path, AVPixelFormat::AV_PIX_FMT_RGBA));

        const auto _config = _src_frame.get_config();

        auto _dst_config =
            w_av_config(AVPixelFormat::AV_PIX_FMT_BGRA, _config.width, _config.height);
        BOOST_LEAF_AUTO(_dst_frame, _src_frame.convert_video(std::move(_dst_config)));

        const std::filesystem::path _new_path = std::filesystem::current_path().append("/rgb_converted.png");
        BOOST_LEAF_CHECK(_dst_frame.save_video_frame_to_img_file(_new_path));

        return {};
      },
      [](const w_trace &p_trace) {
        const auto _msg = wolf::format("avframe_test got an error: {}", p_trace.to_string());
        BOOST_ERROR(_msg);
      },
      [] { BOOST_ERROR("avframe_test got an error!"); });

  std::cout << "leaving test case 'avframe_test'" << std::endl;
}

#endif