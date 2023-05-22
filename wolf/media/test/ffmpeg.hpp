/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_MEDIA_FFMPEG) && defined(WOLF_MEDIA_STB)

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>

#include <media/ffmpeg/w_ffmpeg.hpp>
#include <media/ffmpeg/w_encoder.hpp>

using w_av_frame = wolf::media::ffmpeg::w_av_frame;
using w_av_codec_opt = wolf::media::ffmpeg::w_av_codec_opt;
using w_av_config = wolf::media::ffmpeg::w_av_config;
using w_av_set_opt = wolf::media::ffmpeg::w_av_set_opt;
using w_av_packet = wolf::media::ffmpeg::w_av_packet;
using w_ffmpeg = wolf::media::ffmpeg::w_ffmpeg;

static boost::leaf::result<std::tuple<w_av_packet, w_av_config, w_av_config>> s_encode(
    _In_ const std::string &p_name, _In_ std::variant<const char *, AVCodecID> &p_codec_id,
    _In_ const w_av_codec_opt &p_codec_options,
    _In_ const std::vector<w_av_set_opt> &p_av_set_options, _In_ const bool p_flush = true) {
  // namespaces
  using w_encoder = wolf::media::ffmpeg::w_encoder;

  // create av frame from img
  const auto _img_file_name = std::string("../../content/texture/rgb.png");
  const std::filesystem::path _img_path = std::filesystem::current_path().append(_img_file_name);

  BOOST_LEAF_AUTO(_src_frame, w_av_frame::load_video_frame_from_img_file(
                                  _img_path, AVPixelFormat::AV_PIX_FMT_RGBA));

  // source & destination configs
  const auto _src_config = _src_frame.get_config();
  auto _dst_config =
      w_av_config(AVPixelFormat::AV_PIX_FMT_YUV420P, _src_config.width, _src_config.height);

  // encode frame to packet
  boost::leaf::result<w_encoder> _encoder_res;
  if (p_codec_id.index() == 0) {
    auto _codec_id = std::get<0>(p_codec_id);
    _encoder_res =
        w_ffmpeg::create_encoder(_dst_config, _codec_id, p_codec_options, p_av_set_options);
  } else {
    const auto _codec_id = std::get<1>(p_codec_id);
    _encoder_res =
        w_ffmpeg::create_encoder(_dst_config, _codec_id, p_codec_options, p_av_set_options);
  }
  // check encoder
  BOOST_LEAF_AUTO(_encoder, std::move(_encoder_res));

  // convert source frame to yuv frame
  auto _dst_config_clone = w_av_config(_dst_config);
  BOOST_LEAF_AUTO(_yuv_frame, _src_frame.convert_video(std::move(_dst_config_clone)));

  auto _packet = w_av_packet();
  BOOST_LEAF_CHECK(_encoder.encode(_yuv_frame, _packet, p_flush));

  const std::filesystem::path _encoded_path =
      std::filesystem::current_path().append(wolf::format("/{}_yuv_encoded.png", p_name));
  BOOST_LEAF_CHECK(_yuv_frame.save_video_frame_to_img_file(_encoded_path));

  return std::make_tuple(std::move(_packet), _src_config, _dst_config);
}

static boost::leaf::result<void> s_decode(
    _In_ std::tuple<w_av_packet, w_av_config, w_av_config> &p_encoded_tuple,
    _In_ const std::string &p_name, _In_ std::variant<const char *, AVCodecID> &p_codec_id,
    _In_ const w_av_codec_opt &p_codec_options,
    _In_ const std::vector<w_av_set_opt> &p_av_set_options, _In_ const bool p_flush = false) {
  using w_decoder = wolf::media::ffmpeg::w_decoder;

  // create destination avframe for decoder
  auto _video_buffer = std::vector<uint8_t>();
  auto _decoded_frame = w_av_frame(std::forward<w_av_config &&>(std::get<2>(p_encoded_tuple)));
  BOOST_LEAF_CHECK(_decoded_frame.init());
  BOOST_LEAF_CHECK(_decoded_frame.set_video_frame(std::move(_video_buffer)));

  // create a decoder
  boost::leaf::result<w_decoder> _decoder_res;
  if (p_codec_id.index() == 0) {
    auto _codec_id = std::get<0>(p_codec_id);
    _decoder_res = w_ffmpeg::create_decoder(_decoded_frame.get_config(), _codec_id, p_codec_options,
                                            p_av_set_options);
  } else {
    const auto _codec_id = std::get<1>(p_codec_id);
    _decoder_res = w_ffmpeg::create_decoder(_decoded_frame.get_config(), _codec_id, p_codec_options,
                                            p_av_set_options);
  }

  // check decoder
  BOOST_LEAF_AUTO(_decoder, std::move(_decoder_res));

  // decode the packet
  BOOST_LEAF_CHECK(_decoder.decode(std::get<0>(p_encoded_tuple), _decoded_frame, p_flush));

  BOOST_LEAF_AUTO(_final_frame, _decoded_frame.convert_video(
                                    std::forward<w_av_config &&>(std::get<1>(p_encoded_tuple))));

  const std::filesystem::path _path =
      std::filesystem::current_path().append(wolf::format("/{}_rgb_decoded.png", p_name));
  BOOST_LEAF_CHECK(_decoded_frame.save_video_frame_to_img_file(_path));

  return {};
}

BOOST_AUTO_TEST_CASE(av1_encode_decode_test) {
  const wolf::system::w_leak_detector _detector = {};

  constexpr auto _name = "av1";
  std::cout << "entering test case '_encode_decode_test'" << std::endl;

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
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

        std::variant<const char *, AVCodecID> _encode_codec_id("libsvtav1");
        BOOST_LEAF_AUTO(_encoded_tuple, s_encode(_name, _encode_codec_id, _codec_opt, _opts));

        std::variant<const char *, AVCodecID> _decode_codec_id("libdav1d");
        BOOST_LEAF_CHECK(s_decode(_encoded_tuple, _name, _decode_codec_id, _codec_opt, {}));

        return {};
      },
      [](const w_trace &p_trace) {
        const auto _msg = wolf::format("got error: {}", p_trace.to_string());
        BOOST_ERROR(_msg);
      },
      [] {
        const auto _msg = wolf::format("got an error");
        BOOST_ERROR(_msg);
      });

  std::cout << "leaving test case 'av1_encode_decode_test'" << std::endl;
}

BOOST_AUTO_TEST_CASE(vp9_encode_decode_test) {
  const wolf::system::w_leak_detector _detector = {};

  constexpr auto _name = "vp9";
  std::cout << "entering test case 'vp9_encode_decode_test'" << std::endl;

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        const auto _codec_opt = w_av_codec_opt{
            4'000'000, /*bitrate*/
            60,        /*fps*/
            600,       /*gop*/
            -1,        /*level*/
            2,         /*max_b_frames*/
            3,         /*refs*/
            -1,        /*thread_count*/
        };

        // for more info read https://trac.ffmpeg.org/wiki/Encode/VP9
        const auto _opts = std::vector<w_av_set_opt>();

        std::variant<const char *, AVCodecID> _codec_id(AVCodecID::AV_CODEC_ID_VP9);
        BOOST_LEAF_AUTO(_encoded_tuple, s_encode(_name, _codec_id, _codec_opt, _opts));

        BOOST_LEAF_CHECK(s_decode(_encoded_tuple, _name, _codec_id, _codec_opt, {}));

        return {};
      },
      [](const w_trace &p_trace) {
        const auto _msg = wolf::format("got error: {}", p_trace.to_string());
        BOOST_ERROR(_msg);
      },
      [] {
        const auto _msg = wolf::format("got an error");
        BOOST_ERROR(_msg);
      });

  std::cout << "leaving test case 'vp9_encode_decode_test'" << std::endl;
}

BOOST_AUTO_TEST_CASE(x264_encode_decode_test) {
  const wolf::system::w_leak_detector _detector = {};

  constexpr auto _name = "x264";
  std::cout << "entering test case 'x264_encode_decode_test'" << std::endl;

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        const auto _codec_opt = w_av_codec_opt{
            4'000'000, /*bitrate*/
            60,        /*fps*/
            600,       /*gop*/
            -1,        /*level*/
            2,         /*max_b_frames*/
            3,         /*refs*/
            -1,        /*thread_count*/
        };

        // for more info read https://trac.ffmpeg.org/wiki/Encode/H.264
        const auto _opts = std::vector<w_av_set_opt>{
            w_av_set_opt{"profile", "main"}, w_av_set_opt{"preset", "veryfast"},
            w_av_set_opt{"tune", "zerolatency"}, w_av_set_opt{"crf", 22}};

        std::variant<const char *, AVCodecID> _codec_id(AVCodecID::AV_CODEC_ID_H264);
        BOOST_LEAF_AUTO(_encoded_tuple, s_encode(_name, _codec_id, _codec_opt, _opts));

        BOOST_LEAF_CHECK(s_decode(_encoded_tuple, _name, _codec_id, _codec_opt, {}, true));

        return {};
      },
      [](const w_trace &p_trace) {
        const auto _msg = wolf::format("got error: {}", p_trace.to_string());
        BOOST_ERROR(_msg);
      },
      [] {
        const auto _msg = wolf::format("got an error");
        BOOST_ERROR(_msg);
      });

  std::cout << "leaving test case 'x264_encode_decode_test'" << std::endl;
}

#endif