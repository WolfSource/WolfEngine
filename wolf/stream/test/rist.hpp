/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_STREAM_RIST

#pragma once

#include <gtest.hpp> 
#include <stream/rist/w_rist.hpp>

TEST(rist, data_block_test) {
  const wolf::system::w_leak_detector _detector = {};
  using w_rist_data_block = wolf::stream::rist::w_rist_data_block;

  for (size_t i = 0; i < 5; i++) {
    std::string _msg = "hello";
    const auto _block = std::make_tuple((void *)_msg.data(), _msg.size());

    auto _data_block = w_rist_data_block();
    _data_block.set(_block);
    auto _new_block = _data_block.get();

    const auto new_msg = std::string_view(
        gsl::narrow_cast<const char *>(std::get<0>(_new_block)),
        std::get<1>(_new_block));
    EXPECT_EQ(new_msg, "hello");
  }
}

TEST(rist, rist_sender_test) {
  const wolf::system::w_leak_detector _detector = {};

  using w_rist = wolf::stream::rist::w_rist;
  using w_rist_data_block = wolf::stream::rist::w_rist_data_block;
  using namespace std::chrono_literals;

  constexpr auto _mode = rist_profile::RIST_PROFILE_MAIN;
  constexpr auto _loss = 0; // any value greater than 0 will cause memory leak

  std::string _msg = "hello";
  const auto _block = std::make_tuple((void *)_msg.data(), _msg.size());

  auto _data_block = w_rist_data_block();
  _data_block.set(_block);

  auto _sender = w_rist(rist_ctx_mode::RIST_SENDER_MODE, _mode, _loss,
                        rist_log_level::RIST_LOG_DEBUG);
  auto _ret = _sender.init();
  EXPECT_EQ(_ret.has_error(), false);

  constexpr auto url = "rist://"
                       "127.0.0.1:1234?cname=wolf&rtt-max=10&rtt-min=1&secret="
                       "12345678&aes-type=256&bandwidth=6000";
  _ret = _sender.connect(url);
  EXPECT_EQ(_ret.has_error(), false);

  for (size_t i = 0; i < 5; i++) {

    auto _sent = _sender.send(_data_block);
    EXPECT_EQ(_ret.has_error(), false);

    std::this_thread::sleep_for(std::chrono::duration(1ms));
  }
}

TEST(rist, rist_receiver_test) {
  const wolf::system::w_leak_detector _detector = {};

  using w_rist = wolf::stream::rist::w_rist;
  using w_rist_data_block = wolf::stream::rist::w_rist_data_block;
  using namespace std::chrono_literals;

  constexpr auto _mode = rist_profile::RIST_PROFILE_MAIN;
  constexpr auto _loss = 0; // any value greater than 0 will cause memory leak

  auto _data_block = w_rist_data_block();

  auto _receiver = w_rist(rist_ctx_mode::RIST_RECEIVER_MODE, _mode, _loss,
                          rist_log_level::RIST_LOG_DEBUG);
  auto _ret = _receiver.init();
  EXPECT_EQ(_ret.has_error(), false);

  constexpr auto url = "rist://@"
                       "127.0.0.1:1234?cname=wolf&rtt-max=10&rtt-min=1&secret="
                       "12345678&aes-type=256&bandwidth=6000";
  _ret = _receiver.connect(url);
  EXPECT_EQ(_ret.has_error(), false);

  for (size_t i = 0; i < 5; i++) {

    auto _bytes = _receiver.receive(_data_block, 1000);
    EXPECT_EQ(_bytes.has_error(), false);

    std::this_thread::sleep_for(std::chrono::duration(1ms));
  }
}

TEST(rist, rist_send_receiver_test) {
  const wolf::system::w_leak_detector _detector = {};

  using w_rist = wolf::stream::rist::w_rist;
  using w_rist_data_block = wolf::stream::rist::w_rist_data_block;
  using namespace std::chrono_literals;

  constexpr auto _mode = rist_profile::RIST_PROFILE_MAIN;
  constexpr auto _loss = 0;

  std::thread thread_sender([&]() {
    auto _sender = w_rist(rist_ctx_mode::RIST_SENDER_MODE, _mode, _loss,
                          rist_log_level::RIST_LOG_DEBUG);
    auto _ret = _sender.init();
    EXPECT_EQ(_ret.has_error(), false);

    constexpr auto url =
        "rist://"
        "127.0.0.1:1234?cname=wolf&rtt-max=10&rtt-min=1&secret="
        "12345678&aes-type=256&bandwidth=6000";
    _ret = _sender.connect(url);
    EXPECT_EQ(_ret.has_error(), false);

    const auto t0 = std::chrono::high_resolution_clock::now();
    auto _index = 0;
    for (;;) {
      std::string _msg = "hello from RIST stream: #" + std::to_string(_index++);
      const auto _block = std::make_tuple((void *)_msg.data(), _msg.size());

      auto _data_block = w_rist_data_block();
      _data_block.set(_block);

      auto _sent = _sender.send(_data_block);
      EXPECT_EQ(_sent.has_error(), false);

      const auto t1 = std::chrono::high_resolution_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() >
          5) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::duration(1ms));
    }
  });

  std::thread thread_receiver([&]() {
    std::this_thread::sleep_for(std::chrono::duration(1s));

    auto _receiver = w_rist(rist_ctx_mode::RIST_RECEIVER_MODE, _mode, _loss,
                            rist_log_level::RIST_LOG_DEBUG);
    auto _ret = _receiver.init();
    EXPECT_EQ(_ret.has_error(), false);

    _receiver.on_receiver_data_callback =
        [](_In_ const w_rist_data_block &p_data_block) {
          std::cout << p_data_block.to_string() << std::endl;
        };

    constexpr auto url =
        "rist://@"
        "127.0.0.1:1234?cname=wolf&rtt-max=10&rtt-min=1&secret="
        "12345678&aes-type=256&bandwidth=6000";
    _ret = _receiver.connect(url);
    EXPECT_EQ(_ret.has_error(), false);

    const auto t0 = std::chrono::high_resolution_clock::now();
    for (;;) {
      const auto t1 = std::chrono::high_resolution_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count() >
          5) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::duration(1ms));
    }
  });

  thread_sender.join();
  thread_receiver.join();
}

#endif