/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#if defined(WOLF_TESTS) && defined(WOLF_SYSYEM_SIG_SLOT)

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <boost/signals2.hpp>

BOOST_AUTO_TEST_CASE(w_sigslot_test) {
  const wolf::system::w_leak_detector _detector = {};

  auto sig = boost::signals2::signal<void()>();
  sig.connect([]() { std::cout << "hello from slot 1" << std::endl; });
  sig.connect([]() { std::cout << "hello from slot 2" << std::endl; });
  sig();

  BOOST_REQUIRE(true);
}

#endif // defined(WOLF_TESTS) && defined(WOLF_SYSYEM_SIG_SLOT)