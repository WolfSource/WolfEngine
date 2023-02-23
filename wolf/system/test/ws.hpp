/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_SYSTEM_SOCKET) &&                       \
    defined(WOLF_SYSTEM_HTTP1_WS)

#pragma once

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <wolf/wolf.hpp>

#include <system/socket/w_ws_client.hpp>
#include <system/socket/w_ws_server.hpp>
#include <system/w_timer.hpp>

BOOST_AUTO_TEST_CASE(ws_server_timeout_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "ws_server_timeout_test is running" << std::endl;

  using tcp = boost::asio::ip::tcp;
  using w_ws_server = wolf::system::socket::w_ws_server;
  using w_socket_options = wolf::system::socket::w_socket_options;
  using namespace std::chrono_literals;

  auto _io = boost::asio::io_context();
  w_socket_options _opts = {};
  tcp::endpoint _endpoint = {tcp::v4(), 8881};

  auto t1 = std::jthread([&]() {
    // stop websocket server after 10
    std::this_thread::sleep_for(10s);
    _io.stop();
  });

  const auto _timeout = boost::beast::websocket::stream_base::timeout{// handshake_timeout
                                                                      5s,
                                                                      // idle_timeout
                                                                      5s,
                                                                      // keep_alive_pings
                                                                      false};

  w_ws_server::run(
      _io, std::move(_endpoint), _timeout, std::move(_opts),
      [](const std::string &p_conn_id, _Inout_ w_buffer &p_buffer,
         _Inout_ bool &p_is_binary) -> auto{
        std::cout << "websocket server just got: /'" << p_buffer.to_string() << "/' and "
                  << p_buffer.used_bytes << " bytes from connection id: " << p_conn_id << std::endl;
        return boost::beast::websocket::close_code::normal;
      },
      [](const std::string &p_conn_id, const boost::system::system_error &p_error) {
        std::cout << "connection just got an error: " << p_conn_id << " because of "
                  << p_error.what() << " error code: " << p_error.code() << std::endl;
      });
  _io.run();

  std::cout << "ws_server_timeout_test is done" << std::endl;
}

BOOST_AUTO_TEST_CASE(ws_client_timeout_test) {
  const wolf::system::w_leak_detector _detector = {};

  std::cout << "ws_client_timeout_test is running" << std::endl;

  using tcp = boost::asio::ip::tcp;
  using w_ws_client = wolf::system::socket::w_ws_client;
  using w_socket_options = wolf::system::socket::w_socket_options;
  using w_timer = wolf::system::w_timer;

  auto _io = boost::asio::io_context();

  tcp::endpoint _endpoint = {tcp::v4(), 8000};

  boost::asio::co_spawn(
      _io,
      [&]() -> boost::asio::awaitable<void> {
        // create a websocket client
        auto _client = w_ws_client(_io);
        // setup a timer with 1 nanosecond interval
        auto _timer = w_timer(_io);
        _timer.expires_after(std::chrono::nanoseconds(1));

        // run a coroutine
        const auto _ret = co_await (_timer.async_wait(boost::asio::use_awaitable) ||
                                    _client.async_resolve(_endpoint));
        // expect timeout
        BOOST_REQUIRE(_ret.index() == 0);

        _io.stop();

        co_return;
      },
      boost::asio::detached);

  _io.run();

  std::cout << "ws_client_timeout_test is done" << std::endl;
}

// BOOST_AUTO_TEST_CASE(ws_read_write) {
//   const wolf::system::w_leak_detector _detector = {};
//
//   using tcp = boost::asio::ip::tcp;
//   using w_ws_client = wolf::system::socket::w_ws_client;
//   using w_ws_server = wolf::system::socket::w_ws_server;
//   using w_socket_options = wolf::system::socket::w_socket_options;
//   using w_timer = wolf::system::w_timer;
//   using namespace std::chrono_literals;
//
//   auto _io = boost::asio::io_context();
//   w_socket_options _opts = {};
//   tcp::endpoint _endpoint = {tcp::v4(), 8882};
//   const auto _timer_timeout = 10s;
//   const auto _timeout =
//       boost::beast::websocket::stream_base::timeout{// handshake_timeout
//                                                     5s,
//                                                     // idle_timeout
//                                                     5s,
//                                                     // keep_alive_pings
//                                                     false};
//   boost::asio::co_spawn(
//       _io,
//       [&]() -> boost::asio::awaitable<void> {
//         // wait for server to be initialized
//         std::this_thread::sleep_for(3s);
//         // start connecting to the server
//         w_socket_options _opts = {};
//         auto _client = w_ws_client(_io);
//         auto _timer = w_timer(_io);
//
//         _timer.expires_after(_timer_timeout);
//         auto _resolve_res =
//             co_await (_timer.async_wait(boost::asio::use_awaitable) ||
//                       _client.async_resolve("127.0.0.1", 8882));
//         // expect resolve
//         BOOST_REQUIRE(_resolve_res.index() == 1);
//
//         auto _endpoints = std::get<1>(_resolve_res);
//         BOOST_REQUIRE(_endpoints.size() != 0);
//
//         const auto _endpoint = _endpoints.cbegin()->endpoint();
//
//         auto _conn_res =
//             co_await (_timer.async_wait(boost::asio::use_awaitable) ||
//                       _client.async_connect(_endpoint, _opts));
//         // expect the connection
//         BOOST_REQUIRE(_conn_res.index() == 1);
//
//         constexpr bool _is_binary = false;
//         w_buffer _send_buffer("hello");
//         w_buffer _recv_buffer{};
//
//         for (size_t i = 0; i < 5; i++) {
//           auto _res = co_await (_timer.async_wait(boost::asio::use_awaitable)
//           ||
//                                 _client.async_write(_send_buffer,
//                                 _is_binary));
//           // expect the connection
//           BOOST_REQUIRE(_res.index() == 1);
//           BOOST_REQUIRE(std::get<1>(_res) == 5);
//
//           _res = co_await (_timer.async_wait(boost::asio::use_awaitable) ||
//                            _client.async_read(_recv_buffer));
//           // expect the connection
//           BOOST_REQUIRE(_res.index() == 1);
//           _recv_buffer.used_bytes = std::get<1>(_res);
//
//           BOOST_REQUIRE(_recv_buffer.used_bytes == 10);            //
//           hello-back BOOST_REQUIRE(_recv_buffer.to_string() == "hello-back");
//           // hello-back
//         }
//
//         _send_buffer.from_string("exit");
//         auto _res = co_await (_timer.async_wait(boost::asio::use_awaitable)
//         ||
//                               _client.async_write(_send_buffer, _is_binary));
//         // expect the connection
//         BOOST_REQUIRE(_res.index() == 1);
//         BOOST_REQUIRE(std::get<1>(_res) == 4);
//
//         _io.stop();
//
//         co_return;
//       },
//       boost::asio::detached);
//
//   // create a websocket server with 3 seconds timeout and wait for the exit
//   // process
//   w_ws_server::run(
//       _io, std::move(_endpoint), _timeout, std::move(_opts),
//       [](_In_ const std::string &p_conn_id, _Inout_ w_buffer &p_buffer,
//          _Inout_ bool &p_is_binary) -> auto{
//         // close on overflow
//         if (p_buffer.used_bytes > 1025) {
//           return boost::beast::websocket::close_code::policy_error;
//         }
//
//         auto _reply = std::string(p_buffer.buf.data(), p_buffer.used_bytes);
//
//         std::cout << "websocket server just got: \"" << _reply
//                   << "\" from connection id: " << p_conn_id << std::endl;
//
//         if (_reply == "exit") {
//           return boost::beast::websocket::close_code::normal;
//         }
//         _reply += "-back";
//         p_buffer.from_string(_reply);
//
//         return boost::beast::websocket::close_code::none;
//       },
//       [&](const std::string &p_conn_id,
//           const boost::system::system_error &p_error) {
//         std::cout << "error happened for connection: " << p_conn_id
//                   << " because of " << p_error.what()
//                   << " error code: " << p_error.code() << std::endl;
//         _io.stop();
//       });
//
//   _io.run();
// }

#endif // defined(WOLF_TEST) && defined(WOLF_SYSTEM_SOCKET) &&
       // defined(WOLF_SYSTEM_HTTP1_WS)