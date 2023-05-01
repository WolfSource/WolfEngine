/*
    Project: Wolf Engine. Copyright � 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_STREAM_GRPC

#pragma once

#include <wolf/wolf.hpp>
#include <agrpc/cancel_safe.hpp>
#include <boost/asio/cancellation_signal.hpp>
#include <agrpc/wait.hpp>
#include <grpcpp/alarm.h>
#include <boost/asio/bind_executor.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/channel.h>
#include <agrpc/asio_grpc.hpp>
#include <grpcpp/create_channel.h>
#include <boost/asio/detached.hpp>

class w_grpc {
 public:

  template <auto PrepareAsync, typename Stub, typename Request, typename Response>
  W_API boost::asio::awaitable<void> cancel_safe(
       _In_ agrpc::GrpcContext& _context,
       _In_ boost::asio::cancellation_signal& signal,
       _In_ agrpc::GrpcCancelSafe& safe) noexcept {
    bool done{};
    //grpc::Alarm alarm;

    //agrpc::wait(alarm, std::chrono::system_clock::now() + std::chrono::milliseconds(100),
    //            boost::asio::bind_executor(grpc_context, safe.token()));

    //grpc::ClientContext client_context;
    //client_context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

    Stub stub{_channel};
    Response response;
    Request request;
    grpc::ClientContext client_context;

    std::unique_ptr<grpc::ClientAsyncWriter<Request>> writer;
    bool request_ok =
        co_await agrpc::request(PrepareAsync, stub,
                                client_context, writer, response);

         // Optionally read initial metadata first.
     bool read_ok = co_await agrpc::read_initial_metadata(writer);

         // Send a message.
     agrpc::write(writer, request, boost::asio::bind_executor(_context, safe.token()));
     //bool write_ok = co_await agrpc::write(writer, request);
     //    safe.wait(agrpc::bind_allocator(
     //    grpc_context.get_allocator(),
     //    boost::asio::bind_cancellation_slot(
     //        signal.slot(),
     //        boost::asio::bind_executor(grpc_context, [&](auto&& ec, bool) { done = !ec; }))));

     //signal.emit(boost::asio::cancellation_type::terminal);

     bool writes_done_ok{false};

     {

        //       safe.wait(
        //           agrpc::bind_allocator(
        //grpc_context.get_allocator(),
        //           boost::asio::bind_cancellation_slot(
        //           signal.slot(), boost::asio::bind_executor(
        //                                   grpc_context, [&](auto&& ec, bool) { done = !ec; })))
        //   );
        //signal.emit(boost::asio::cancellation_type::terminal);

       if (0 == done)  // Alarm finished
       {
         // The read continues in the background.
       } else  // Read finished
       {
         // Signal that we are done writing.
         writes_done_ok = co_await agrpc::writes_done(writer);
       }
     }

     // Wait for the server to recieve all our messages.
     grpc::Status status;
     co_await agrpc::finish(writer, status);
  }

  template <auto PrepareAsync, typename Stub, typename Request, typename Response>
  W_API boost::asio::awaitable<void> send_async(
      _In_ Request& request, 
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
  }

  template <class Function, typename... Args>
  W_API void run_async(
      _In_ Function&& function,
      _In_ Args&&... p_args) noexcept {

    boost::asio::co_spawn(
        _context,
        [&]() -> boost::asio::template awaitable<void> {
          co_await(std::forward<Function>(function)(std::forward<Args>(p_args)...));
        },
        boost::asio::detached);

    _context.run();
  }

  W_API boost::leaf::result<int> init(
      _In_ const char* server_url,
      _In_ const int port) noexcept {
      
      const auto host = std::string(server_url) + ":" +
          std::to_string(port);

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
};

 #endif


