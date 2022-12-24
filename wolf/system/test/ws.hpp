/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_SYSTEM_SOCKET) && defined(WOLF_SYSTEM_HTTP1_WS)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <system/socket/w_ws_server.hpp>

BOOST_AUTO_TEST_CASE(ws_server_timeout) {
  const wolf::system::w_leak_detector _detector = {};

  using tcp = boost::asio::ip::tcp;
  using w_ws_server = wolf::system::socket::w_ws_server;
  using w_socket_options = wolf::system::socket::w_socket_options;

  auto _io = boost::asio::io_context();
  w_socket_options _opts = {};
  tcp::endpoint _endpoint = {tcp::v4(), 8881};

  auto t1 = std::jthread([&]() {
    // stop server
    std::this_thread::sleep_for(std::chrono::seconds(20));
    _io.stop();
  });

  auto timeout =
      boost::beast::websocket::stream_base::timeout{// handshake_timeout
                                                    std::chrono::seconds(5),
                                                    // idle_timeout
                                                    std::chrono::seconds(5),
                                                    // keep_alive_pings
                                                    false};

  w_ws_server::run(
      _io, std::move(_endpoint), timeout, std::move(_opts),
      [](const std::string &p_conn_id,
         _In_ const boost::beast::flat_buffer &p_i_data,
         _Inout_ boost::beast::flat_buffer &p_o_mut_data) -> auto{
        std::cout << "ws server just got: " << p_i_data.size()
                  << " bytes from connection id: " << p_conn_id << std::endl;
        return boost::system::errc::connection_aborted;
      },
      [](const std::string &p_conn_id,
         const boost::system::system_error &p_error) {
        std::cout << "connection just got an error: " << p_conn_id
                  << " because of " << p_error.what()
                  << " error code: " << p_error.code() << std::endl;
      });
  _io.run();
}

#endif // defined(WOLF_TEST) && defined(WOLF_SYSTEM_SOCKET) && defined(WOLF_SYSTEM_HTTP1_WS)