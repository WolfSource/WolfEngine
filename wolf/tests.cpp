/*
        Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#include <wolf/system/w_leak_detector.hpp>
#include <wolf/wolf.hpp>

#include <wolf/DISABLE_ANALYSIS_BEGIN>
#define BOOST_TEST_MODULE wolf
#include <boost/test/included/unit_test.hpp> // NOLINT
#include <wolf/DISABLE_ANALYSIS_END>

BOOST_AUTO_TEST_CASE(wolf_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "wolf_test is running" << std::endl;

  const auto _wolf_version = wolf::w_init();
  BOOST_TEST(_wolf_version.empty() == false);

  std::cout << "wolf_test just done" << std::endl;
}

#pragma region system tests

#include <wolf/system/test/compress.hpp>
#include <wolf/system/test/coroutine.hpp>
#include <wolf/system/test/gamepad.hpp>
#include <wolf/system/test/gametime.hpp>
#include <wolf/system/test/log.hpp>
//#include <wolf/system/test/postgresql.hpp>
#include <wolf/system/test/process.hpp>
#include <wolf/system/test/signal_slot.hpp>
#include <wolf/system/test/tcp.hpp>
#include <wolf/system/test/trace.hpp>
#include <wolf/system/test/ws.hpp>

#pragma endregion

#pragma region stream tests

//#include <wolf/stream/test/ffmpeg_stream.hpp>
//#include <wolf/stream/test/rist.hpp>

#pragma endregion

#pragma region media tests

#include <wolf/media/test/avframe.hpp>
#include <wolf/media/test/ffmpeg.hpp>
#include <wolf/media/test/image.hpp>
//#include <wolf/media/test/openal.hpp>

#pragma endregion

#pragma region ml tests

#include <wolf/ml/test/w_utilities_test.hpp>

#pragma endregion
