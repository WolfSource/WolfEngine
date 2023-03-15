#define _DISABLE_VECTOR_ANNOTATION

extern "C" {
#include "wolf.h"
}
#include "w_leak_detector.hpp"
#include <catch2/catch_all.hpp>
#define DEFER auto _ = std::shared_ptr<void>(nullptr, [&](...)

TEST_CASE("wolf"){
  const w_leak_detector _detector = {};
  int major = 0, minor = 0, patch = 0;
  w_version(&major, &minor, &patch);
  REQUIRE(major == 3);
}

WError s_connect_close(const char *p_address) { return WError::None; }
WError s_on_message(const char *p_address, Buffer* p_buffer, double p_socket_live_time_in_sec) { return WError::None; }

TEST_CASE("media_ffmpeg") {
  const w_leak_detector _detector = {};
  auto _error = WError::Unknown;
  auto _tcp_client = w_system_socket_tcp_client_new(3, 5, "127.0.0.1", 5, 5, 8000, false, &_error);
  DEFER { w_system_socket_tcp_client_free(_tcp_client); });

  auto _on_connect_close_ptr = &s_connect_close;
  auto _s_on_message_ptr = &s_on_message;
  _error = w_system_socket_tcp_client_connect(_tcp_client, &_on_connect_close_ptr,
                                              &_s_on_message_ptr, &_on_connect_close_ptr);
}

TEST_CASE("system_tcp_client") {
  const w_leak_detector _detector = {};
  auto _error = WError::Unknown;
  auto _tcp_client = w_system_socket_tcp_client_new(3, 5, "127.0.0.1", 5, 5, 8000, false, &_error);
  DEFER { w_system_socket_tcp_client_free(_tcp_client); });

  auto _on_connect_close_ptr = &s_connect_close;
  auto _s_on_message_ptr = &s_on_message;
  _error = w_system_socket_tcp_client_connect(_tcp_client, &_on_connect_close_ptr,
                                              &_s_on_message_ptr, &_on_connect_close_ptr);
}