/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSYEM_SIG_SLOT

#include <gtest.hpp>
#include <boost/signals2.hpp>

TEST(sigslot, sigslot_test) {
  const wolf::system::w_leak_detector _detector = {};

  auto sig = boost::signals2::signal<void()>();
  sig.connect([]() { std::cout << "hello from slot 1" << std::endl; });
  sig.connect([]() { std::cout << "hello from slot 2" << std::endl; });
  sig();
}

#endif