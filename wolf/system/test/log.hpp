/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_SYSTEM_LOG)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <system/log/w_log.hpp>
#include <system/w_process.hpp>
#include <sstream>
#include <random> 

BOOST_AUTO_TEST_CASE(log_stress_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "log_stress_test is running" << std::endl;

  using w_log = wolf::system::log::w_log;
  using w_log_config = wolf::system::log::w_log_config;
  using w_log_sink = wolf::system::log::w_log_sink;

  auto _path = wolf::system::w_process::current_exe_path();
  BOOST_REQUIRE(_path.has_error() == false);

  auto _p = _path.value();
  BOOST_REQUIRE(_p.empty() == false);

  const auto _log_path = _p.append("/log/");
  const w_log_config _config = {
      // create an async logger
      false,
      // enable multi-threaded feature
      true,
      // the path of log file
      _log_path,
      // the log level
      spdlog::level::level_enum::debug,
      // the flush level
      spdlog::level::level_enum::debug,
  // the sinks of log
#ifdef _MSC_VER
      w_log_sink::VISUAL_STUDIO | w_log_sink::CONSOLE,
#else
      w_log_sink::CONSOLE,
#endif
      // max file size in Mb (e.g. maximum 100 log files * with size of 100 Mb
      // file)
      100 * 1048576,
      // max number of files
      10,
      // rotate file on open
      false,
      // start creation time for daily log
      0,
      // start creation time for daily log
      0};

  w_log _log(_config);
  const auto _ret = _log.init();
  BOOST_REQUIRE(_ret.has_error() == false);

  constexpr auto _max = 5;
  constexpr auto _min = 3;
  std::default_random_engine _rand_engine = {};
  std::uniform_int_distribution<size_t> _rand_gen(_min, _max);
  const auto _max_loop = _rand_gen(_rand_engine);

  std::vector<std::thread> threads;
  for (size_t i = 0; i < _max_loop; ++i) {
    threads.push_back(std::thread([&_log, i]() {
      // get thread id
      std::stringstream _sstream;
      _sstream << std::this_thread::get_id();
      const auto _thread_id = _sstream.str();

      std::default_random_engine _rand_engine = {};
      std::uniform_int_distribution<long long> _rand_gen(_min, _max);
      const auto _max_delta_in_sec = _rand_gen(_rand_engine);

      const auto time_1 = std::chrono::high_resolution_clock::now();
      size_t j = 0;
      for (;;) {
        _log.write("#{} hello #{} from thread:{}", i, j++, _thread_id);

        const auto time_2 = std::chrono::high_resolution_clock::now();
        const auto _delta =
            std::chrono::duration_cast<std::chrono::seconds>(time_2 - time_1)
                .count();
        if (_delta > _max_delta_in_sec) {
          break;
        }
      }
    }));
  }

  for (auto &t : threads) {
    t.join();
  }

  std::cout << "log_stress_test just done" << std::endl;
}

#endif