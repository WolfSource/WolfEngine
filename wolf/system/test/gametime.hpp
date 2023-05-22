/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_TEST

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <wolf/wolf.hpp>

#include <mutex>
#include <system/w_gametime.hpp>

BOOST_AUTO_TEST_CASE(gametime_fixed_time_test) {
  const wolf::system::w_leak_detector _detector = {};
  using w_gametime = wolf::system::w_gametime;

  std::cout << "entering test case 'gametime_fixed_time_test'" << std::endl;

  std::once_flag _flag;
  bool _ticked = false;

  constexpr double _stop_after_secs = 3.0;
  constexpr double _target_elapsed_secs = 1.0 / 50.0;  // 50 fps

  auto _gametime = w_gametime();
  _gametime.reset();
  _gametime.set_fixed_time_step(true);
  _gametime.set_target_elapsed_secs(_target_elapsed_secs);

  auto _total_seconds = _gametime.get_total_secs();
#ifdef __clang__
#pragma unroll
#endif
  while (_total_seconds < _stop_after_secs) {
    _gametime.tick([&]() { std::call_once(_flag, [&]() { _ticked = true; }); });

    _total_seconds = _gametime.get_total_secs();
  }

  std::cout << "leaving test case 'gametime_fixed_time_test'" << std::endl;
}

BOOST_AUTO_TEST_CASE(gametime_unfixed_time_test) {
  const wolf::system::w_leak_detector _detector = {};

  using w_gametime = wolf::system::w_gametime;

  std::cout << "entering test case 'gametime_unfixed_time_test'" << std::endl;

  constexpr double _stop_after_secs = 5.0;
  constexpr double _target_elapsed_secs = 1.0 / 50.0;  // 50 fps

  auto _gametime = w_gametime();
  _gametime.reset();
  _gametime.set_fixed_time_step(false);
  _gametime.set_target_elapsed_secs(_target_elapsed_secs);

  BOOST_REQUIRE(_gametime.get_fps() == 0);

  auto _total_seconds = _gametime.get_total_secs();
  while (_total_seconds < _stop_after_secs) {
    _gametime.tick();
    _total_seconds = _gametime.get_total_secs();
  }

  BOOST_REQUIRE(_gametime.get_fps() >= 30);

  std::cout << "leaving test case 'gametime_unfixed_time_test'" << std::endl;
}

#endif // WOLF_TESTS