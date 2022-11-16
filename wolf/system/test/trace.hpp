/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <gtest.hpp> 

TEST(trace, handle_all) {
  const wolf::system::w_leak_detector _detector = {};

  const auto _function_1 = []() noexcept -> boost::leaf::result<void> {
    return W_ERR(std::errc::bad_message, "error from function 1");
  };

  const auto _function_2 = [&]() noexcept -> boost::leaf::result<void> {
    return _function_1();
  };

  // how to use w_trace
  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        BOOST_LEAF_CHECK(_function_2());
        std::cout << "success!" << std::endl;
        return {};
      },
      [](const w_trace &p_trace) {
        std::cout << "caught a test error! trace info:" << p_trace << std::endl;
      },
      [] { std::cout << "caught an error!" << std::endl; });
}