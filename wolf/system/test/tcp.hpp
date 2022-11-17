/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_SYSTEM_SOCKET

#pragma once

#include <gtest.hpp> 
#include <system/socket/w_tcp_server.hpp>
#include <system/socket/w_tcp_client.hpp>
#include <system/w_timer.hpp>

TEST(tcp, tcp_server_timeout_test) {
  const wolf::system::w_leak_detector _detector = {};

  using tcp = boost::asio::ip::tcp;
  using w_tcp_server = wolf::system::socket::w_tcp_server;
  using w_socket_options = wolf::system::socket::w_socket_options;

  auto _io = boost::asio::io_context();
  w_socket_options _opts = {};
  tcp::endpoint _endpoint = {tcp::v4(), 8080};
  auto timeout = std::chrono::milliseconds(3000);

  auto t1 = std::jthread([&]() {
    // stop server
    std::this_thread::sleep_for(std::chrono::seconds(5));
    _io.stop();
  });

  w_tcp_server::run(
      _io, std::move(_endpoint), std::move(timeout), std::move(_opts),
      [](const std::string &p_conn_id, w_buffer &p_mut_data) -> auto{
        std::cout << "tcp server just got: " << p_mut_data.used_bytes
                  << " bytes from connection id: " << p_conn_id << std::endl;
        return boost::system::errc::connection_aborted;
      },
      [](const std::string &p_conn_id) {
        std::cout << "timeout for connection: " << p_conn_id << std::endl;
      },
      [](const std::string &p_conn_id, const std::exception &p_excp) {
        std::cout << "timeout for connection: " << p_conn_id << " because of "
                  << p_excp.what() << std::endl;
      });
  _io.run();
}

TEST(tcp, tcp_client_connect_timeout_test) {
  const wolf::system::w_leak_detector _detector = {};

  using tcp = boost::asio::ip::tcp;
  using w_tcp_client = wolf::system::socket::w_tcp_client;
  using w_socket_options = wolf::system::socket::w_socket_options;
  using w_timer = wolf::system::w_timer;

  auto _io = boost::asio::io_context();

  w_socket_options _opts = {};
  tcp::endpoint _endpoint = {tcp::v4(), 8000};

  boost::asio::co_spawn(
      _io,
      [&]() -> boost::asio::awaitable<void> {
        auto client = w_tcp_client(_io);
        auto _timer = w_timer(_io);
        _timer.expires_after(std::chrono::nanoseconds(1));

        auto _ret = co_await (_timer.async_wait(boost::asio::use_awaitable) ||
                              client.async_resolve(_endpoint));
        // expect timeout
        EXPECT_EQ(_ret.index(), 0);

        _timer.cancel();
        _timer.expires_after(std::chrono::seconds(5));
        _ret = co_await (_timer.async_wait(boost::asio::use_awaitable) ||
                         client.async_resolve("google.com", 443));
        // expect resolving
        EXPECT_EQ(_ret.index(), 1);

        _io.stop();

        co_return;
      },
      boost::asio::detached);

  _io.run();
}

TEST(tcp, tcp_read_write_test) {
  const wolf::system::w_leak_detector _detector = {};

  using tcp = boost::asio::ip::tcp;
  using w_tcp_client = wolf::system::socket::w_tcp_client;
  using w_tcp_server = wolf::system::socket::w_tcp_server;
  using w_socket_options = wolf::system::socket::w_socket_options;
  using w_timer = wolf::system::w_timer;
  using namespace std::chrono_literals;

  auto _io = boost::asio::io_context();
  w_socket_options _opts = {};
  tcp::endpoint _endpoint = {tcp::v4(), 8080};
  const auto timeout = 10s;

  boost::asio::co_spawn(
      _io,
      [&]() -> boost::asio::awaitable<void> {
        // wait for server to be initialized
        std::this_thread::sleep_for(3s);
        // start connecting to the server
        w_socket_options _opts = {};
        auto client = w_tcp_client(_io);
        auto _timer = w_timer(_io);

        _timer.expires_after(timeout);
        auto _resolve_res =
            co_await (_timer.async_wait(boost::asio::use_awaitable) ||
                      client.async_resolve("127.0.0.1", 8080));
        // expect resolve
        EXPECT_EQ(_resolve_res.index(), 1);

        auto _endpoints = std::get<1>(_resolve_res);
        EXPECT_GT(_endpoints.size(), 0);

        const auto _endpoint = _endpoints.cbegin()->endpoint();

        auto _conn_res =
            co_await (_timer.async_wait(boost::asio::use_awaitable) ||
                      client.async_connect(_endpoint, _opts));
        // expect the connection
        EXPECT_EQ(_conn_res.index(), 1);

        w_buffer _send_buffer("hello");
        w_buffer _recv_buffer{};

        for (size_t i = 0; i < 5; i++) {
          auto _res = co_await (_timer.async_wait(boost::asio::use_awaitable) ||
                                client.async_write(_send_buffer));
          // expect the connection
          EXPECT_EQ(_res.index(), 1);
          EXPECT_EQ(std::get<1>(_res), 5);

          _res = co_await (_timer.async_wait(boost::asio::use_awaitable) ||
                           client.async_read(_recv_buffer));
          // expect the connection
          EXPECT_EQ(_res.index(), 1);
          _recv_buffer.used_bytes = std::get<1>(_res);

          EXPECT_EQ(_recv_buffer.used_bytes, 10); // hello-back
          EXPECT_EQ(_recv_buffer.to_string(),
                    "hello-back"); // hello-back
        }

        _send_buffer.from_string("exit");
        auto _res = co_await (_timer.async_wait(boost::asio::use_awaitable) ||
                              client.async_write(_send_buffer));
        // expect the connection
        EXPECT_EQ(_res.index(), 1);
        EXPECT_EQ(std::get<1>(_res), 4);

        _io.stop();

        co_return;
      },
      boost::asio::detached);

  // create a tcp server with 3 seconds timeout and wait for exit
  // command from client
  w_tcp_server::run(
      _io, std::move(_endpoint), timeout, std::move(_opts),
      [](_In_ const std::string &p_conn_id,
         _Inout_ w_buffer &p_mut_data) -> auto{
        // close on overflow
        if (p_mut_data.used_bytes > 1025) {
          return boost::system::errc::connection_aborted;
        }

        auto _reply = std::string(p_mut_data.buf.data(), p_mut_data.used_bytes);

        std::cout << "tcp server just got: \"" << _reply
                  << "\" from connection id: " << p_conn_id << std::endl;

        if (_reply == "exit") {
          return boost::system::errc::connection_aborted;
        }
        _reply += "-back";
        p_mut_data.from_string(_reply);
        return boost::system::errc::success;
      },
      [](const std::string &p_conn_id) {
        std::cout << "timeout for connection: " << p_conn_id << std::endl;
      },
      [&](const std::string &p_conn_id, const std::exception &p_excp) {
        std::cout << "error happened for connection: " << p_conn_id
                  << " because of " << p_excp.what() << std::endl;
        _io.stop();
      });

  _io.run();
}

#endif