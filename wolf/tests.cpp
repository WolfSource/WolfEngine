/*
        Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

//#include <wolf/system/w_leak_detector.hpp>
#include <optional>
#include <thread>

#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/signal_set.hpp>

#include <build/win-x64-release/wolf/helloworld.grpc.pb.h>
#include <stream/grpc/w_grpc.hpp>
#include <wolf.hpp>

#include "build/win-x64-release/wolf/example.grpc.pb.h"
#include "build/win-x64-release/wolf/example_ext.grpc.pb.h"

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <grpcpp/client_context.h>
#include <boost/asio/thread_pool.hpp>

#include <iostream>

namespace asio = boost::asio;

static bool flag = false;
asio::awaitable<void> handle_client_streaming_request(
    grpc::ServerContext&,
    grpc::ServerAsyncReader<example::v1::Response, example::v1::Request>& reader) {
  // Optionally send initial metadata first.
  // Since the default completion token in asio-grpc is asio::use_awaitable, this line is equivalent
  // to: co_await agrpc::send_initial_metadata(reader, asio::use_awaitable);
  bool send_ok = co_await agrpc::send_initial_metadata(reader);
  example::v1::Request request;

  bool read_ok;
  do {
    // Read from the client stream until the client has signaled `writes_done`.
    read_ok = co_await agrpc::read(reader, request);
  } while (read_ok);

  example::v1::Response response;
  co_await agrpc::finish(reader, response, grpc::Status::OK);
  std::cout << "server received: " << request.integer() << std::endl;
}

void register_client_streaming_handler(agrpc::GrpcContext& grpc_context,
                                       example::v1::Example::AsyncService& service) {
  // Register a handler for all incoming RPCs of this method (Example::ClientStreaming) until the
  // server is being shut down. An API for requesting to handle a single RPC is also available:
  // `agrpc::request(&example::v1::Example::AsyncService::RequestClientStreaming, service,
  // server_context, reader)`
  agrpc::repeatedly_request(&example::v1::Example::AsyncService::RequestClientStreaming, service,
                            asio::bind_executor(grpc_context, &handle_client_streaming_request));
}

void run_server() {
  const auto host = std::string("0.0.0.0:") + "50051";

  std::unique_ptr<grpc::Server> server;

  grpc::ServerBuilder builder;
  agrpc::GrpcContext grpc_context{builder.AddCompletionQueue()};
  builder.AddListeningPort(host, grpc::InsecureServerCredentials());
  example::v1::Example::AsyncService service;
  builder.RegisterService(&service);
  example::v1::ExampleExt::AsyncService service_ext;
  builder.RegisterService(&service_ext);
  server = builder.BuildAndStart();

  asio::thread_pool thread_pool{1};

  register_client_streaming_handler(grpc_context, service);

  flag = true;
  std::cout << "start grpc server" << std::endl;

  grpc_context.run();

  server->Shutdown();

  std::cout << "stop grpc server" << std::endl;
}

//asio::awaitable<void> run_w_grpc(agrpc::GrpcContext& grpc_context,
//                                 std::shared_ptr<grpc::Channel>& grpc_channel) {
//  asio::cancellation_signal signal;
//  agrpc::GrpcCancelSafe safe;
//
//  return w_grpc::cancel_safe<&example::v1::Example::Stub::PrepareAsyncClientStreaming,
//                      example::v1::Example::Stub, example::v1::Request, example::v1::Response>(
//      signal, safe);
//}

asio::awaitable<void> w_grpc_send(agrpc::GrpcContext& grpc_context,
                                         std::shared_ptr<grpc::Channel>& grpc_channel,
                                         example::v1::Request& request, example::v1::Response& response) {

  return w_grpc::send_async<&example::v1::Example::Stub::PrepareAsyncClientStreaming,
                             example::v1::Example::Stub, example::v1::Request,
                             example::v1::Response>(request, response);
}

void run_client() {
  const auto host = std::string("localhost:") + "50051";

  const char* url = "localhost";
  w_grpc::init(url, 50051);

  example::v1::Request request;
  request.set_integer(1);
  example::v1::Response response;
  agrpc::GrpcContext _context;
  //w_grpc::run_async(
  //    [](example::v1::Request request, example::v1::Response response) {
  //                          w_grpc::send_async<&example::v1::Example::Stub::PrepareAsyncClientStreaming,
  //                         example::v1::Example::Stub, example::v1::Request, example::v1::Response>(
  //                          request, response);
  //    },
  //    request,
  //    response);

  //w_grpc::run_async(
  //                  w_grpc::send_async<&example::v1::Example::Stub::PrepareAsyncClientStreaming,
  //                  example::v1::Example::Stub, 
  //                  example::v1::Request, 
  //                  example::v1::Response>,
  //                  request, response);

    w_grpc::run_async(
          _context,
                         &w_grpc::send_async<&example::v1::Example::Stub::PrepareAsyncClientStreaming,
                         example::v1::Example::Stub, example::v1::Request, example::v1::Response>,
                         request, response);
}

int main() {

std::thread _th = std::thread(run_server);
  _th.detach();

  while (!flag) {
    Sleep(10000);
  }

  //    std::thread _client_th = std::thread(run_client);
  //_client_th.detach();
  run_client();
  return 0;
}
    //
//#define BOOST_TEST_MODULE wolf
//#include <boost/test/included/unit_test.hpp>
//
//BOOST_AUTO_TEST_CASE(wolf_test) {
//  const wolf::system::w_leak_detector _detector = {};
//
//  std::cout << "entering test case 'wolf_test'" << std::endl;
//
//  const auto _wolf_version = wolf::w_init();
//  BOOST_TEST(_wolf_version.empty() == false);
//
//  std::cout << "leaving test case 'wolf_test'" << std::endl;
//}
//
//#pragma region system tests
//
//#include <wolf/system/test/compress.hpp>
//#include <wolf/system/test/coroutine.hpp>
//#include <wolf/system/test/gamepad.hpp>
//#include <wolf/system/test/gametime.hpp>
//#include <wolf/system/test/log.hpp>
////#include <wolf/system/test/postgresql.hpp>
//#include <wolf/system/test/process.hpp>
//#include <wolf/system/test/signal_slot.hpp>
//#include <wolf/system/test/tcp.hpp>
//#include <wolf/system/test/trace.hpp>
//#include <wolf/system/test/ws.hpp>
//#include <wolf/system/test/lua.hpp>
//
//#pragma endregion
//
//#pragma region stream tests
//
////#include <wolf/stream/test/ffmpeg_stream.hpp>
//#include <wolf/stream/test/rist.hpp>
//#include <wolf/stream/test/grpc.hpp>
//
//#pragma endregion
//
//#pragma region media tests
//
////#include <wolf/media/test/avframe.hpp>
////#include <wolf/media/test/ffmpeg.hpp>
////#include <wolf/media/test/image.hpp>
////#include <wolf/media/test/openal.hpp>
//#include <wolf/media/test/gstreamer.hpp>
//
//#pragma endregion
//
//#pragma region ml tests
//
////#include <wolf/ml/test/w_image_processor_test.hpp>
////#include <wolf/ml/test/w_ocr_engine_test.hpp>
////#include <wolf/ml/test/w_referee_test.hpp>
////#include <wolf/ml/test/w_soccer_test.hpp>
////#include <wolf/ml/test/w_utilities_test.hpp>
//
//#pragma endregion
