/*
    Project: Wolf Engine. Copyright � 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_STREAM_GRPC

#pragma once

#include <wolf/wolf.hpp>
#include <boost/asio.hpp>
#include <boost/asio/bind_executor.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/channel.h>
#include <agrpc/asio_grpc.hpp>
#include <grpcpp/create_channel.h>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>

class w_grpc_client {
 public:
  template <auto PrepareAsync, typename Stub, typename Request, typename Response>
  W_API boost::asio::awaitable<void> send_bidirectional(_In_ Request& request,
                                                        _Inout_ Response& response) {
    Stub stub{_channel};
    grpc::ClientContext client_context;

    std::unique_ptr<grpc::ClientAsyncReaderWriter<Request, Response>> reader_writer;
    bool request_ok = co_await agrpc::request(PrepareAsync, stub, client_context, reader_writer);
    if (!request_ok) {
      // Channel is either permanently broken or transiently broken but with the fail-fast option.
      co_return;
    }

    // Reads and writes can be performed simultaneously.
    using namespace boost::asio::experimental::awaitable_operators;
    auto [read_ok, write_ok] =
        co_await (agrpc::read(reader_writer, response) && agrpc::write(reader_writer, request));

    // Do not forget to signal that we are done writing before finishing.
    co_await agrpc::writes_done(reader_writer);

    grpc::Status status;
    co_await agrpc::finish(reader_writer, status);
  }

  template <auto PrepareAsync, typename Stub, typename Request, typename Response>
  W_API boost::asio::awaitable<void> send_stream(_In_ Request& request,
                                                 _Inout_ Response& response) noexcept {
    Stub stub{_channel};
    grpc::ClientContext client_context;

    std::unique_ptr<grpc::ClientAsyncWriter<Request>> writer;
    bool request_ok = co_await agrpc::request(PrepareAsync, stub, client_context, writer, response);

    // Optionally read initial metadata first.
    bool read_ok = co_await agrpc::read_initial_metadata(writer);

    // Send a message.
    bool write_ok = co_await agrpc::write(writer, request);

    // Signal that we are done writing.
    bool writes_done_ok = co_await agrpc::writes_done(writer);

    // Wait for the server to recieve all our messages.
    grpc::Status status;
    co_await agrpc::finish(writer, status);

    co_return;
  }

  template <auto PrepareAsync, typename Stub, typename Request, typename Response>
  W_API boost::asio::awaitable<void> send_unary(_In_ Request& request,
                                                _Inout_ Response& response) noexcept {
    Stub stub{_channel};
    grpc::ClientContext client_context;

    using RPC = agrpc::RPC<PrepareAsync>;
    grpc::Status status = co_await RPC::request(_context, stub, client_context, request, response,
                                                boost::asio::use_awaitable);
    co_return;
  }

  template <class Function, typename... Args>
  W_API void run(_In_ std::chrono::steady_clock::duration&& p_timeout, _In_ Function&& function,
                 _In_ Args&&... p_args) noexcept {
    boost::asio::co_spawn(
        _context,
        [&]() -> boost::asio::template awaitable<void> {
          co_await ((std::forward<Function>(function)(std::forward<Args>(p_args)...)) ||
                    timeout(std::chrono::steady_clock::now() + p_timeout));
        },
        boost::asio::detached);

    _context.run();
  }

  W_API boost::leaf::result<int> init(_In_ const std::string& server_url,
                                      _In_ const int port) noexcept {
    if (server_url.empty()) {
      return W_FAILURE(std::errc::invalid_argument, "missing grpc server url endpoint");
    }

    const auto host = std::string(server_url) + ":" + std::to_string(port);

    _channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());

    if (_channel.get() == nullptr) {
      return W_FAILURE(std::errc::operation_canceled,
                       "could not create channel to connect to grpc server");
    }

    return 0;
  }

 private:
  agrpc::GrpcContext _context;
  std::shared_ptr<grpc::Channel> _channel;

  static boost::asio::awaitable<std::errc> timeout(
      _In_ const std::chrono::steady_clock::time_point& p_deadline) noexcept {
    using steady_timer = boost::asio::steady_timer;
    using steady_clock = std::chrono::steady_clock;

    steady_timer _timer(co_await boost::asio::this_coro::executor);
    auto _now = steady_clock::now();

#ifdef __clang__
#pragma unroll
#endif
    while (p_deadline > _now) {
      _timer.expires_at(p_deadline);
      co_await _timer.async_wait(boost::asio::use_awaitable);
      _now = steady_clock::now();
    }
    co_return std::errc::timed_out;
  }
};

 #endif


