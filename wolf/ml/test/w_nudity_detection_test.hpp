#ifdef WOLF_TEST

#pragma once

#ifdef WOLF_ML_NUDITY_DETECTION

// #define BOOST_TEST_MODULE ml_nudity_detection

#include <ml/nudity_detection/w_nudity_detection.hpp>

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <filesystem>
#include <wolf.hpp>

#include <media/ffmpeg/w_ffmpeg.hpp>
#include <media/w_openal.hpp>

#include <opencv2/opencv.hpp>
#include <ml/w_common.hpp>

namespace fs = std::filesystem;
fs::path nudity_detection_path = "../../../wolf/ml/test/common_test_asset/nudity_detection";

using namespace wolf::ml;
using w_nud_det = wolf::ml::nudet::w_nud_det;

BOOST_AUTO_TEST_CASE(check_stream_to_avoid_nsfw_context) {
  const wolf::system::w_leak_detector _detector = {};

  // set envirenment variables
  fs::path env_file_path = nudity_detection_path / ".check_stream_to_avoid_nsfw_context";
  set_env(env_file_path.string().c_str());

  std::cout << "nudity_detection_text is running" << std::endl;

  using w_av_set_opt = wolf::media::ffmpeg::w_av_set_opt;
  const auto _opt = std::vector<w_av_set_opt>();

  constexpr auto _url = "srt://20.216.45.101:554?mode=caller&transtype=live&timeout=5000000";

  // initial the nudity detection param
  std::string nudity_detection_model_path =
    get_env_string("NUDITY_DETECTION_MODEL_PATH");
  std::cout << "+++++++++++++++++++++++++++++++" << nudity_detection_model_path << std::endl;
  w_nud_det *nud_detector = new w_nud_det(nudity_detection_model_path);
  cv::Mat frame_image;
  cv::Mat rgb_image;
  int tranied_model_height = get_env_int("TRAINED_MODEL_IMAGE_HEIGHT");
  int trained_model_width = get_env_int("TRAINED_MODEL_IMAGE_WIDTH");

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
        wolf::media::ffmpeg::w_decoder _video_decoder = {};
        wolf::media::w_openal_config _openal_config = {};
        wolf::media::w_openal _openal = {};
        std::unique_ptr<w_av_frame> _av_video_frame;
        auto _width = 0;
        auto _height = 0;

        auto _on_frame_cb =
            [&](const w_av_packet &p_packet, const AVStream *p_audio_stream,
                           const AVStream *p_video_stream) -> bool {
          const auto _stream_index = p_packet.get_stream_index();
          if (p_video_stream) {
            std::cout << "got video frame" << std::endl;

            if (_video_initialized == false) {
              auto _codec_param = p_video_stream->codecpar;
              _width = _codec_param->width;
              _height = _codec_param->height;

              const auto _codec_id = _codec_param->codec_id;
              const auto _fmt = gsl::narrow_cast<AVPixelFormat>(_codec_param->format);

              const auto _config = w_av_config(_fmt, _width, _height);

              const auto _codec_opt = w_av_codec_opt{
                  _codec_param->bit_rate, /*bitrate*/
                  30,                     /*fps*/
                  600,                    /*gop*/
                  _codec_param->level,    /*level*/
                  2,                      /*max_b_frames*/
                  3,                      /*refs*/
                  -1,                     /*thread_count*/
              };

              auto _decoder_res = w_ffmpeg::create_decoder(_config, _codec_id, _codec_opt);
              if (_decoder_res.has_error()) {
                return false;  // close it
              }

              _video_decoder = std::move(_decoder_res.value());

              // create destination avframe for decoder
              _av_video_frame =
                  std::make_unique<w_av_frame>((w_av_config &&) std::move(_config));

              auto _decoded_frame_res = _av_video_frame->init();
              if (_decoded_frame_res.has_error()) {
                std::cout << "could not initialize decoder" << std::endl;
                return false;  // close it
              }

              _video_initialized = true;
            }

            auto _res = _video_decoder.decode(p_packet, *_av_video_frame);
            if (_res.has_error()) {
              std::cout << "could not decode" << std::endl;
            } else {
              auto _dst_config = w_av_config(AVPixelFormat::AV_PIX_FMT_BGRA, _width, _height);

              auto _rgba_frame =
                  _av_video_frame->convert_video((w_av_config&&)std::move(_dst_config));


              if (_rgba_frame.has_error()) {
                std::cout << "could not convert av video frame to rgb frame" << std::endl;
              } else {
                const auto _path = std::string("C:/src/WolfEngine/build") +
                    "/" + std::to_string(_index++) + "_rist_decoded.png";
                std::cout << "path:" << _path << std::endl;
                _rgba_frame.value().save_video_frame_to_img_file(_path);
                std::cout << "rgb frame created" << std::endl;

                
                
                auto image_dat_tuple = _rgba_frame.value().get();
                auto image_data = std::get<0>(image_dat_tuple);
                frame_image = cv::Mat(_height, _width, CV_8UC4, image_data[0]);
                cv::cvtColor(frame_image, rgb_image, cv::COLOR_RGBA2RGB);
                cv::Mat temp_rgb_image = rgb_image.clone();

                // cv::resize(rgb_image, rgb_image, cv::Size(trained_model_width, tranied_model_height), 0, 0, cv::INTER_NEAREST);
                // std::vector<float> value;
                // const auto T0 = std::chrono::high_resolution_clock::now();
                // value = nud_detector->nudity_detection(rgb_image.data, rgb_image.cols,
                //                                        rgb_image.rows, rgb_image.channels());

                // const auto T1 = std::chrono::high_resolution_clock::now();

                // const auto D = std::chrono::duration_cast<std::chrono::milliseconds>(T1 - T0);

                // for(int i=0; i<5; i++) {
                //   cv::putText(temp_rgb_image, std::to_string(value[i]), cv::Point(100,(i+1)*50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0,255,0), 2, false);
                // }
                // cv::putText(temp_rgb_image, std::to_string(D.count()), cv::Point(300,50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255,0,0), 2, false); 

                cv::imshow("It is a test!", temp_rgb_image);
                cv::waitKey(1);

                // std::cout << "+ +" << value[0] 
                //           << "   " << value[1]
                //           << "   " << value[2]
                //           << "   " << value[3]
                //           << "   " << value[4]
                //           << "   " << value[5]
                //           << "   " << value[6]
                //           << "   " << value[7]
                //           << "   " << value[8]
                //           << "   " << value[9]
                //           << std::endl;
              }
            }
          }
          const auto t1 = std::chrono::high_resolution_clock::now();
          if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0) > 100s) {
            return false;
          }
          return true;
        };

        BOOST_CHECK(w_ffmpeg::open_stream(_url, _opt, _on_frame_cb));

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

  std::cout << "nudity_detection_text is done!" << std::endl;
}

#endif // WOLF_ML_NUDITY_DETECTION

#endif // WOLF_TEST
