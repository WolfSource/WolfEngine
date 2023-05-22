/*
        Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#include <wolf/system/w_leak_detector.hpp>
// #include <wolf/wolf.hpp>

// #define BOOST_TEST_MODULE wolf
// #include <boost/test/included/unit_test.hpp>

// BOOST_AUTO_TEST_CASE(wolf_test) {
//   const wolf::system::w_leak_detector _detector = {};

//   std::cout << "entering test case 'wolf_test'" << std::endl;

//   const auto _wolf_version = wolf::w_init();
//   BOOST_TEST(_wolf_version.empty() == false);

//   std::cout << "leaving test case 'wolf_test'" << std::endl;
// }

#include <api/v1/game.grpc.pb.h>
#include <api/v1/game.pb.h>
// #include <hello.grpc.pb.h>
// #include <hello.pb.h>

// #include <boost/test/included/unit_test.hpp>
#include <stream/grpc/w_grpc_client.hpp>
// #include <system/w_leak_detector.hpp>
#include <grpc++/client_context.h>

#include <wolf.hpp>

#pragma region system tests

// #include <wolf/system/test/compress.hpp>
// #include <wolf/system/test/coroutine.hpp>
// #include <wolf/system/test/gamepad.hpp>
// #include <wolf/system/test/gametime.hpp>
// #include <wolf/system/test/log.hpp>
////#include <wolf/system/test/postgresql.hpp>
// #include <wolf/system/test/process.hpp>
// #include <wolf/system/test/signal_slot.hpp>
// #include <wolf/system/test/tcp.hpp>
// #include <wolf/system/test/trace.hpp>
// #include <wolf/system/test/ws.hpp>
// #include <wolf/system/test/lua.hpp>
// #include <wolf/system/test/python.hpp>

#pragma endregion

#pragma region stream tests

// #include <wolf/stream/test/ffmpeg_stream.hpp>
// #include <wolf/stream/test/rist.hpp>
// #include <wolf/stream/test/grpc.hpp>
// #include <wolf/stream/test/quic.hpp>

#pragma endregion

#pragma region media tests

// #include <wolf/media/test/avframe.hpp>
// #include <wolf/media/test/ffmpeg.hpp>
// #include <wolf/media/test/image.hpp>
// #include <wolf/media/test/openal.hpp>
// #include <wolf/media/test/gstreamer.hpp>

#pragma endregion

#pragma region ml tests

// #include <wolf/ml/test/w_image_processor_test.hpp>
// #include <wolf/ml/test/w_ocr_engine_test.hpp>
// #include <wolf/ml/test/w_referee_test.hpp>
// #include <wolf/ml/test/w_soccer_test.hpp>
// #include <wolf/ml/test/w_utilities_test.hpp>

#pragma endregion

using w_grpc_client = wolf::stream::rpc::w_grpc_client;

int main() {
  // const wolf::system::w_leak_detector _detector = {};

  boost::asio::io_context _io{};

  // create a client
  auto _client = w_grpc_client();
  auto _x = _client.init(_io, "go-proxy-shtl.alphaco.cloud:9090");

  auto _task = [&]() -> boost::asio::awaitable<void> {
    auto _timeout = std::chrono::seconds(5);
    api::v1::GameServiceListResponse _res{};
    api::v1::GameServiceListRequest _req{};
    _req.set_page_size(10);

    const auto& _status = co_await _client.send_unary<
        &api::v1::GameService::Stub::PrepareAsyncList,
        api::v1::GameService::Stub, api::v1::GameServiceListRequest,
        api::v1::GameServiceListResponse>(_req, _timeout, _res);

    if (_status.ok()) {
      for (auto& game : _res.games()) {
        std::cout << "game about is " << game.about() << std::endl;
      }
    } else {
      std::cout << "game about is " << _status.error_message() << std::endl;
    }
  };

  wolf::stream::rpc::exec(_io, _task);
  _io.run();
}
