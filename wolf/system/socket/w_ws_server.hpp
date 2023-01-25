#if defined(WOLF_SYSTEM_HTTP1_WS) && defined(WOLF_SYSTEM_SOCKET)

#pragma once

#include "w_socket_options.hpp"
#include <wolf/wolf.hpp>

namespace wolf::system::socket {
class w_ws_server {
public:
  /*
   * @param p_io_context, the boost io context
   * @param p_endpoint, the endpoint of the server
   * @param p_timeout, the timeout for connection
   * @param p_socket_options, the socket options
   * @param p_on_data_callback, on data callback for session
   * @param p_on_timeout_callback, on timeout callback for session
   * @param p_on_error_callback, on error callback for session
   * @returns void
   */
  W_API static boost::leaf::result<int>
  run(_In_ boost::asio::io_context &p_io_context,
      _In_ const boost::asio::ip::tcp::endpoint &&p_endpoint,
      _In_ const boost::beast::websocket::stream_base::timeout &p_timeout,
      _In_ w_socket_options &&p_socket_options,
      _In_ w_session_ws_on_data_callback p_on_data_callback,
      _In_ w_session_on_error_callback p_on_error_callback) noexcept;
};
} // namespace wolf::system::socket

#endif // defined(WOLF_SYSTEM_HTTP1_WS) && defined(WOLF_SYSTEM_SOCKET)
