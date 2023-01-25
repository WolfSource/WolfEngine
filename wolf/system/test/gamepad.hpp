/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_TEST

#pragma once

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <wolf/wolf.hpp>

#ifdef WOLF_SYSTEM_GAMEPAD_CLIENT

#include <system/gamepad/w_gamepad_client.hpp>

BOOST_AUTO_TEST_CASE(gamepad_test) {
  const wolf::system::w_leak_detector _detector = {};

  // TODO(PooyaEimandar): write test code for gamepad
}

#endif // WOLF_SYSTEM_GAMEPAD_CLIENT

#ifdef WOLF_SYSTEM_GAMEPAD_VIRTUAL

#include <system/gamepad/w_gamepad_virtual_pool.hpp>

BOOST_AUTO_TEST_CASE(gamepad_virtual_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "gamepad_virtual_test is running" << std::endl;

  using w_gamepad_virtual_pool = wolf::system::gamepad::w_gamepad_virtual_pool;

  const auto _ret = w_gamepad_virtual_pool::init();
  BOOST_REQUIRE(_ret.has_error() == false);

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        // create 4 gamepads and destroy them at the end of their lifetime
        BOOST_LEAF_AUTO(_gamepad_1, w_gamepad_virtual_pool::create());
        BOOST_LEAF_AUTO(_gamepad_2, w_gamepad_virtual_pool::create());
        BOOST_LEAF_AUTO(_gamepad_3, w_gamepad_virtual_pool::create());
        BOOST_LEAF_AUTO(_gamepad_4, w_gamepad_virtual_pool::create());

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

  w_gamepad_virtual_pool::fini();

  std::cout << "gamepad_virtual_test just done" << std::endl;
}

#endif // WOLF_SYSTEM_GAMEPAD_VIRTUAL

#endif // WOLF_TEST
