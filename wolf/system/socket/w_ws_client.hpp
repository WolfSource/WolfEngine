/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_HTTP1_WS

#include <wolf.hpp>
#include "w_socket_options.hpp"

#include "DISABLE_ANALYSIS_BEGIN"
#include <boost/asio.hpp>
#include "DISABLE_ANALYSIS_END"

namespace wolf::system::socket {
//class w_ws_server {
//public:
//  /*
//   * @param p_io_context, the boost io context
//   * @param p_endpoint, the endpoint of the server
//   * @param p_socket_options, the socket options
//   * @param p_timeout, the timeout for connection
//   * @param p_on_data_callback, on data callback for session
//   * @param p_on_timeout_callback, on timeout callback for session
//   * @param p_on_error_callback, on error callback for session
//   * @returns void
//   */
//  W_API static void
//  run(boost::asio::io_context &p_io_context,
//      boost::asio::ip::tcp::endpoint &&p_endpoint,
//      std::chrono::steady_clock::duration &&p_timeout,
//      w_socket_options &&p_socket_options,
//      w_session_on_data_callback p_on_data_callback,
//      w_session_on_timeout_callback p_on_timeout_callback,
//      w_session_on_error_callback p_on_error_callback) noexcept;
//};
} // namespace wolf::system::socket

#endif