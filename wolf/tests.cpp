/*
	Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
#define BOOST_TEST_MODULE wolf
#include <boost/test/included/unit_test.hpp>
#include <DISABLE_ANALYSIS_END>

BOOST_AUTO_TEST_CASE(wolf_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "wolf_test is running" << std::endl;

  const auto _wolf_version = wolf::w_init();
  BOOST_TEST(_wolf_version.empty() == false);

  std::cout << "wolf_test just done" << std::endl;
}

#pragma region system tests

#include <system/test/compress.hpp>
#include <system/test/gamepad.hpp>
#include <system/test/gametime.hpp>
#include <system/test/log.hpp>
#include <system/test/process.hpp>
#include <system/test/signal_slot.hpp>
#include <system/test/tcp.hpp>
#include <system/test/trace.hpp>
#include <system/test/ws.hpp>

#pragma endregion

#pragma region stream tests

#include <stream/test/ffmpeg_stream.hpp>
#include <stream/test/rist.hpp>

#pragma endregion

#pragma region media tests

#include <media/test/avframe.hpp>
#include <media/test/ffmpeg.hpp>
#include <media/test/image.hpp>
#include <media/test/openal.hpp>

#pragma endregion
