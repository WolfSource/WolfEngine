/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_SOCKET

#include <wolf.hpp>
#include "w_socket_options.hpp"

#include "DISABLE_ANALYSIS_BEGIN"
#include <boost/asio.hpp>
#include "DISABLE_ANALYSIS_END"

namespace wolf::system::socket {
class w_tcp_server {
public:

  /*
   * @param p_io_context, the boost io context
   * @param p_endpoint, the endpoint of the server
   * @param p_socket_options, the socket options
   * @param p_timeout, the timeout for connection
   * @param p_on_data_callback, on data callback for session
   * @param p_on_timeout_callback, on timeout callback for session
   * @param p_on_error_callback, on error callback for session
   * @returns void
   */
  W_API static boost::leaf::result<int>
  run(_In_ boost::asio::io_context& p_io_context,
      _In_ boost::asio::ip::tcp::endpoint &&p_endpoint,
      _In_ std::chrono::steady_clock::duration &&p_timeout,
      _In_ w_socket_options &&p_socket_options,
      _In_ w_session_on_data_callback p_on_data_callback,
      _In_ w_session_on_timeout_callback p_on_timeout_callback,
      _In_ w_session_on_error_callback p_on_error_callback) noexcept;
};
} // namespace wolf::system::socket

#endif