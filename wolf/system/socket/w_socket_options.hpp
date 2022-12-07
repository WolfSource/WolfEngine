/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_SOCKET

#include <wolf.hpp>
#include <functional>
#include <boost/system/errc.hpp>

namespace wolf::system::socket {

struct w_socket_options {
  bool keep_alive = true;
  bool no_delay = true;
  bool reuse_address = true;
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