/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_STREAM_GRPC

#pragma once

#include <wolf/wolf.hpp>

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

namespace wolf::stream::rpc {
class w_grpc_server {
 public:
  /*
   * @param p_io_context, the boost io context
   * @param p_url,the endpoint of the server
   * @param p_port, the port of the server
   * @param p_socket_options, the socket options
   * @param p_on_data_callback, on data callback for session
   * @param p_on_error_callback, on error callback for session
   * @returns void
   */
  //W_API static boost::leaf::result<int> run(
  //    _In_ boost::asio::io_context &p_io_context, _In_ const std::string_view &p_url,
  //    _In_ const short p_port, _In_ std::chrono::steady_clock::duration &&p_timeout) noexcept;
};
}  // namespace wolf::stream::rpc

#endif
