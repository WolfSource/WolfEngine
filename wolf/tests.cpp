/*
        Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#include <wolf/system/w_leak_detector.hpp>
#include <wolf/wolf.hpp>

#define BOOST_TEST_MODULE wolf
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(wolf_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "entering test case 'wolf_test'" << std::endl;

  const auto _wolf_version = wolf::w_init();
  BOOST_TEST(_wolf_version.empty() == false);

  std::cout << "leaving test case 'wolf_test'" << std::endl;
}

#pragma region system tests

//#include <wolf/system/test/compress.hpp>
//#include <wolf/system/test/coroutine.hpp>
//#include <wolf/system/test/gamepad.hpp>
//#include <wolf/system/test/gametime.hpp>
//#include <wolf/system/test/log.hpp>
////#include <wolf/system/test/postgresql.hpp>
//#include <wolf/system/test/process.hpp>
//#include <wolf/system/test/signal_slot.hpp>
//#include <wolf/system/test/tcp.hpp>
//#include <wolf/system/test/trace.hpp>
//#include <wolf/system/test/ws.hpp>
//#include <wolf/system/test/lua.hpp>
//#include <wolf/system/test/python.hpp>

#pragma endregion

#pragma region stream tests

//#include <wolf/stream/test/ffmpeg_stream.hpp>
//#include <wolf/stream/test/rist.hpp>
#include <wolf/stream/test/grpc.hpp>
#include <wolf/stream/test/quic.hpp>

#pragma endregion

#pragma region media tests

//#include <wolf/media/test/avframe.hpp>
//#include <wolf/media/test/ffmpeg.hpp>
//#include <wolf/media/test/image.hpp>
//#include <wolf/media/test/openal.hpp>
//#include <wolf/media/test/gstreamer.hpp>

#pragma endregion

#pragma region ml tests

//#include <wolf/ml/test/w_image_processor_test.hpp>
//#include <wolf/ml/test/w_ocr_engine_test.hpp>
//#include <wolf/ml/test/w_referee_test.hpp>
//#include <wolf/ml/test/w_soccer_test.hpp>
//#include <wolf/ml/test/w_utilities_test.hpp>
//#include <wolf/ml/test/w_nudity_detection_test.hpp>

#pragma endregion
