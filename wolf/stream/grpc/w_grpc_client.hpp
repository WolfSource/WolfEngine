/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_STREAM_GRPC

#pragma once

#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/steady_timer.hpp>
#include <system/w_time.hpp>
#include <wolf/wolf.hpp>

namespace wolf::stream::rpc {

template <class Function>
inline void exec(boost::asio::io_context& p_io, _In_ Function&& p_function) noexcept {
  boost::asio::co_spawn(p_io, p_function, boost::asio::detached);
}

class w_grpc_client {
 public:
  W_API w_grpc_client() noexcept = default;
  W_API ~w_grpc_client() noexcept = default;

  W_API boost::leaf::result<int> init(_In_ boost::asio::io_context& p_io,
                                      _In_ const std::string_view p_endpoint,
                                      _In_ bool p_ssl = false,
                                      _In_ int p_max_msg_size = 0) noexcept {
    if (p_endpoint.empty()) {
      return W_FAILURE(std::errc::invalid_argument, "missing grpc server url endpoint");
    }

    grpc::ChannelArguments _channel_args{};
    if (p_max_msg_size > 0) {
      _channel_args.SetInt(GRPC_ARG_MAX_RECEIVE_MESSAGE_LENGTH, p_max_msg_size);
      _channel_args.SetInt(GRPC_ARG_MAX_SEND_MESSAGE_LENGTH, p_max_msg_size);
    }

    std::shared_ptr<grpc::ChannelCredentials> _credentials;
    if (p_ssl) {
      //_credentials = std::make_shared<grpc::ChannelCredentials>(grpc::SslCredentialsOptions());
    } else {
      _credentials = grpc::InsecureChannelCredentials();
    }

    this->_channel = grpc::CreateCustomChannel(p_endpoint.data(), _credentials, _channel_args);
    if (this->_channel == nullptr) {
      return W_FAILURE(std::errc::operation_canceled,
                       "could not create channel to connect to grpc server");
    }

    boost::asio::post(p_io, [&] {
      p_io.get_executor().on_work_finished();
      agrpc::run(this->_grpc_context, p_io);
      p_io.get_executor().on_work_started();
    });

    return 0;
  }

  //  template <auto PrepareAsync, typename Stub, typename Request, typename
  //  Response> W_API boost::asio::awaitable<void> send_bidirectional(_In_
  //  Request& request,
  //                                                        _Inout_ Response&
  //                                                        response) {
  //    Stub stub{_channel};
  //    grpc::ClientContext client_context;
  //
  //    std::unique_ptr<grpc::ClientAsyncReaderWriter<Request, Response>>
  //    reader_writer; bool request_ok = co_await agrpc::request(PrepareAsync,
  //    stub, client_context, reader_writer); if (!request_ok) {
  //      // Channel is either permanently broken or transiently broken but with
  //      the fail-fast option. co_return;
  //    }
  //
  //    // Reads and writes can be performed simultaneously.
  //    using namespace boost::asio::experimental::awaitable_operators;
  //    auto [read_ok, write_ok] =
  //        co_await (agrpc::read(reader_writer, response) &&
  //        agrpc::write(reader_writer, request));
  //
  //    // Do not forget to signal that we are done writing before finishing.
  //    co_await agrpc::writes_done(reader_writer);
  //
  //    grpc::Status status;
  //    co_await agrpc::finish(reader_writer, status);
  //  }

  // template <auto PrepareAsync, typename Stub, typename Request, typename Response>
  // W_API boost::asio::awaitable<void> send_stream(_In_ Request& request,
  //                                                _Inout_ Response& response) noexcept {
  //   Stub stub{_channel};
  //   grpc::ClientContext client_context;

  //  std::unique_ptr<grpc::ClientAsyncWriter<Request>> writer;
  //  bool request_ok = co_await agrpc::request(PrepareAsync, stub, client_context, writer,
  //  response);

  //  // Optionally read initial metadata first.
  //  bool read_ok = co_await agrpc::read_initial_metadata(writer);

  //  // Send a message.
  //  bool write_ok = co_await agrpc::write(writer, request);

  //  // Signal that we are done writing.
  //  bool writes_done_ok = co_await agrpc::writes_done(writer);

  //  // Wait for the server to recieve all our messages.
  //  grpc::Status status;
  //  co_await agrpc::finish(writer, status);

  //  co_return;
  //}

  template <auto PrepareAsyncFun, typename Stub, typename Request, typename Response>
  boost::asio::awaitable<grpc::Status> send_unary(
      _In_ const Request& p_request,
      _In_ const std::chrono::system_clock::duration& p_timeout,
      _Inout_ Response& p_response) noexcept {
    grpc::ClientContext _client_context;

    auto _deadline = std::chrono::system_clock::now() + p_timeout;
    _client_context.set_deadline(_deadline);

    Stub _stub{this->_channel};
    Response _respose{};

    using RPC = agrpc::RPC<PrepareAsyncFun>;
    auto _status =
        co_await RPC::request(this->_grpc_context, _stub, _client_context, p_request, p_response);

    co_return std::move(_status);
  }

 private:
  agrpc::GrpcContext _grpc_context;
  std::shared_ptr<grpc::Channel> _channel;
};
}  // namespace wolf::stream::rpc

#endif
