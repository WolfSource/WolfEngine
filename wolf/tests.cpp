/*
	Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#include <gtest.hpp>
#include <wolf.hpp>
#include <system/w_leak_detector.hpp>

TEST(wolf, global) {
  const wolf::system::w_leak_detector _detector = {};
  const auto _wolf_version = wolf::w_init();
  EXPECT_EQ(_wolf_version.empty(), false);
}

#pragma region system tests

//#include <system/test/gametime.hpp>
//#include <system/test/process.hpp>
//#include <system/test/signal_slot.hpp>
//#include <system/test/trace.hpp>
//#include <system/test/tcp.hpp>
//#include <system/test/log.hpp>

#pragma endregion

#pragma region stream tests

//#include <stream/test/rist.hpp>

#pragma endregion

#pragma region media tests

#include <media/test/openal.hpp>

#pragma endregion
