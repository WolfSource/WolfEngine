/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_STREAM_RIST)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <stream/rist/w_rist.hpp>

BOOST_AUTO_TEST_CASE(rist_data_block_test) {
  const wolf::system::w_leak_detector _detector = {};
  using w_rist_data_block = wolf::stream::rist::w_rist_data_block;

  std::cout << "rist_data_block_test is running" << std::endl;

  for (size_t i = 0; i < 5; i++) {
    std::string _msg = "hello";
    const auto _block = std::make_tuple((void *)_msg.data(), _msg.size());

    auto _data_block = w_rist_data_block();
    _data_block.set(_block);
    auto _new_block = _data_block.get();

    const auto new_msg = std::string_view(
        gsl::narrow_cast<const char *>(std::get<0>(_new_block)),
        std::get<1>(_new_block));
    BOOST_REQUIRE(new_msg == "hello");
  }

  std::cout << "rist_data_block_test just done" << std::endl;
}

BOOST_AUTO_TEST_CASE(rist_sender_receiver_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "rist_sender_receiver_test is running" << std::endl;

  using w_rist = wolf::stream::rist::w_rist;
  using w_rist_data_block = wolf::stream::rist::w_rist_data_block;
  using namespace std::chrono_literals;

  constexpr auto _timeout_in_sec = 5s;
  constexpr auto _mode = rist_profile::RIST_PROFILE_MAIN;
  constexpr auto _loss = 0;

   std::thread thread_sender([&]() {
     boost::leaf::try_handle_all(
         [&]() -> boost::leaf::result<void> {
           // create a rist sender
           auto _sender = w_rist(rist_ctx_mode::RIST_SENDER_MODE, _mode,
           _loss,
                                 rist_log_level::RIST_LOG_DEBUG);
           auto _ret = _sender.init();
           BOOST_REQUIRE(_ret.has_error() == false);

          constexpr auto _url =
              "rist://"
              "127.0.0.1:1234?cname=wolf&rtt-max=10&rtt-min=1&secret="
              "12345678&aes-type=256&bandwidth=6000";
          _ret = _sender.connect(_url);
          BOOST_REQUIRE(_ret.has_error() == false);

          const auto t0 = std::chrono::high_resolution_clock::now();
          auto _index = 0;
          static std::once_flag _once_flag;

          for (;;) {

            std::string _msg =
                "hello from RIST stream: #" + std::to_string(_index++);
            const auto _block =
                std::make_tuple((void *)_msg.data(), _msg.size());

            auto _data_block = w_rist_data_block();
            _data_block.set(_block);

            auto _sent = _sender.send(_data_block);
            BOOST_REQUIRE(_sent.has_error() == false);

            const auto t1 = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0) >
                _timeout_in_sec) {
              break;
            }
            std::this_thread::sleep_for(std::chrono::duration(1ms));
          }

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
  });

  std::thread thread_receiver([&]() {
    std::this_thread::sleep_for(std::chrono::duration(1s));

    auto _receiver = w_rist(rist_ctx_mode::RIST_RECEIVER_MODE, _mode, _loss,
                            rist_log_level::RIST_LOG_DEBUG);
    auto _ret = _receiver.init();
    BOOST_REQUIRE(_ret.has_error() == false);

    _receiver.on_receiver_data_callback =
        [](_In_ const w_rist_data_block &p_data_block) {
          std::cout << p_data_block.to_string() << std::endl;
        };

    constexpr auto url =
        "rist://@"
        "127.0.0.1:1234?cname=wolf&rtt-max=10&rtt-min=1&secret="
        "12345678&aes-type=256&bandwidth=6000";

    _ret = _receiver.connect(url);
    BOOST_REQUIRE(_ret.has_error() == false);

    const auto t0 = std::chrono::high_resolution_clock::now();
    for (;;) {
      const auto t1 = std::chrono::high_resolution_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0) >
          _timeout_in_sec) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::duration(1ms));
    }
  });

  thread_sender.join();
  thread_receiver.join();

  std::cout << "rist_sender_receiver_test just done" << std::endl;
}

#endif