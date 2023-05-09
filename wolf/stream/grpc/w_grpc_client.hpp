/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_STREAM_GRPC

#pragma once

#include <wolf/wolf.hpp>
#include <boost/asio/steady_timer.hpp>
#include <grpcpp/create_channel.h>
#include <boost/asio/detached.hpp>

#include <grpcpp/channel.h>
#include <boost/asio/bind_executor.hpp>
#include <agrpc/asio_grpc.hpp>

#include <boost/asio/experimental/awaitable_operators.hpp>

#include <system/w_time.hpp>

namespace wolf::stream::rpc {
class w_grpc_client {
 public:
  W_API w_grpc_client() noexcept = default;
  W_API ~w_grpc_client() noexcept = default;

  W_API boost::leaf::result<int> init(_In_ const std::string_view p_server_url,
                                      _In_ const short p_port) noexcept {
    if (p_server_url.empty()) {
      return W_FAILURE(std::errc::invalid_argument, "missing grpc server url endpoint");
    }

    grpc::ChannelArguments _channel_args{};
    _channel_args.SetInt(GRPC_ARG_MAX_RECEIVE_MESSAGE_LENGTH,
                         INT_MAX);  // Set the maximum message size to the maximum value allowed by
                                    // a 32-bit signed integer
    _channel_args.SetInt(GRPC_ARG_MAX_SEND_MESSAGE_LENGTH,
                         INT_MAX);  // Set the maximum message size to the maximum value allowed by
                                    // a 32-bit signed integer

    const auto _host = wolf::format("{}:{}", p_server_url, p_port);
    this->_channel =
        grpc::CreateCustomChannel(_host, grpc::InsecureChannelCredentials(), _channel_args);

    if (this->_channel == nullptr) {
      return W_FAILURE(std::errc::operation_canceled,
                       "could not create channel to connect to grpc server");
    }

    return 0;
  }

  //  template <auto PrepareAsync, typename Stub, typename Request, typename Response>
  //  W_API boost::asio::awaitable<void> send_bidirectional(_In_ Request& request,
  //                                                        _Inout_ Response& response) {
  //    Stub stub{_channel};
  //    grpc::ClientContext client_context;
  //
  //    std::unique_ptr<grpc::ClientAsyncReaderWriter<Request, Response>> reader_writer;
  //    bool request_ok = co_await agrpc::request(PrepareAsync, stub, client_context,
  //    reader_writer); if (!request_ok) {
  //      // Channel is either permanently broken or transiently broken but with the fail-fast
  //      option. co_return;
  //    }
  //
  //    // Reads and writes can be performed simultaneously.
  //    using namespace boost::asio::experimental::awaitable_operators;
  //    auto [read_ok, write_ok] =
  //        co_await (agrpc::read(reader_writer, response) && agrpc::write(reader_writer, request));
  //
  //    // Do not forget to signal that we are done writing before finishing.
  //    co_await agrpc::writes_done(reader_writer);
  //
  //    grpc::Status status;
  //    co_await agrpc::finish(reader_writer, status);
  //  }
  //
  //  template <auto PrepareAsync, typename Stub, typename Request, typename Response>
  //  W_API boost::asio::awaitable<void> send_stream(_In_ Request& request,
  //                                                 _Inout_ Response& response) noexcept {
  //    Stub stub{_channel};
  //    grpc::ClientContext client_context;
  //
  //    std::unique_ptr<grpc::ClientAsyncWriter<Request>> writer;
  //    bool request_ok = co_await agrpc::request(PrepareAsync, stub, client_context, writer,
  //    response);
  //
  //    // Optionally read initial metadata first.
  //    bool read_ok = co_await agrpc::read_initial_metadata(writer);
  //
  //    // Send a message.
  //    bool write_ok = co_await agrpc::write(writer, request);
  //
  //    // Signal that we are done writing.
  //    bool writes_done_ok = co_await agrpc::writes_done(writer);
  //
  //    // Wait for the server to recieve all our messages.
  //    grpc::Status status;
  //    co_await agrpc::finish(writer, status);
  //
  //    co_return;
  //  }
  
  template <auto PrepareAsync, typename Stub, typename Request, typename Response>
  W_API boost::asio::awaitable<void> send_unary(_In_ Request& p_request,
                                                _Inout_ Response& p_response) noexcept {
    Stub stub{_channel};
    grpc::ClientContext client_context;

    using RPC = agrpc::RPC<PrepareAsync>;
    auto _res = co_await RPC::request(_context, stub, client_context, p_request, p_response,
                                      boost::asio::use_awaitable);
    co_return;
  }
  
  template <class Function, typename... Args>
  W_API void exec(_In_ std::chrono::steady_clock::duration&& p_timeout, _In_ Function&& p_function,
                  _In_ Args&&... p_args) noexcept {
    boost::asio::co_spawn(
        this->_context,
        [&]() -> boost::asio::template awaitable<void> {
          co_await (std::forward<Function>(p_function)(std::forward<Args>(p_args)...) ||
                    wolf::system::w_time::timeout(std::chrono::steady_clock::now() + p_timeout));
        },
        boost::asio::detached);

    this->_context.run();
  }

 private:
  agrpc::GrpcContext _context;
  std::shared_ptr<grpc::Channel> _channel;
};
}  // namespace wolf::stream::rpc

#endif
