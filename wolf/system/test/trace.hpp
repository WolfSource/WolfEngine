/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST)

#pragma once

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <wolf/wolf.hpp>

BOOST_AUTO_TEST_CASE(trace_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "trace_test is running" << std::endl;

  const auto _function_1 = []() noexcept -> boost::leaf::result<void> {
    return W_FAILURE(std::errc::bad_message, "error from function 1");
  };

  const auto _function_2 = [&]() noexcept -> boost::leaf::result<void> {
    return _function_1();
  };

  // how to use w_trace
  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        BOOST_LEAF_CHECK(_function_2());
        std::cout << "success!" << std::endl;
        BOOST_ERROR(false);
        return {};
      },
      [](const w_trace &p_trace) {
        std::cout << "caught a test error! trace info:" << p_trace << std::endl;
        BOOST_REQUIRE(true);
      },
      [] {
        std::cout << "caught an error!" << std::endl;
        BOOST_ERROR(false);
      });

  std::cout << "trace_test is done" << std::endl;
}

#endif // WOLF_TESTS