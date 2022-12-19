/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_SOCKET

#include <wolf.hpp>
#include <functional>
#include <boost/system/errc.hpp>
#include <boost/asio.hpp>

namespace wolf::system::socket {

struct w_socket_options {
  bool keep_alive = true;
  bool no_delay = true;
  bool reuse_address = true;

  void set_to_socket(_In_ boost::asio::ip::tcp::socket &p_socket) {
    // set acceptor's options
    const auto _keep_alive_option =
        boost::asio::socket_base::keep_alive(this->keep_alive);

    const auto _reuse_address_option =
        boost::asio::socket_base::reuse_address(this->reuse_address);

    const auto _no_delay_opt = boost::asio::ip::tcp::no_delay(this->no_delay);

    p_socket.set_option(_no_delay_opt);
    p_socket.set_option(_keep_alive_option);
    p_socket.set_option(_reuse_address_option);
  }
};

typedef std::function<boost::system::errc::errc_t(const std::string &p_conn_id,
                                                  w_buffer &p_mut_data)>
    w_session_on_data_callback;

typedef std::function<void(const std::string &p_conn_id)>
    w_session_on_timeout_callback;

typedef std::function<void(const std::string &p_conn_id,
                           const std::exception &p_exp)>
    w_session_on_error_callback;
} // namespace wolf::system::socket

#endif