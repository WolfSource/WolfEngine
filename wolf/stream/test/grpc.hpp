/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_STREAM_GRPC)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <stream/grpc/w_grpc_client.hpp>

#include <raft.grpc.pb.h>

using w_grpc_client = wolf::stream::rpc::w_grpc_client;

BOOST_AUTO_TEST_CASE(grpc_client_unary_test) {
  // const wolf::system::w_leak_detector _detector = {};
  constexpr auto _test_name = "'grpc_client_unary_test'";
  std::cout << "entering test case " << _test_name << std::endl;

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        // create a client
        w_grpc_client _client{};
        BOOST_LEAF_CHECK(_client.init("localhost", gsl::narrow_cast<short>(50051)));

        const auto _msg_id = std::string("unary");

        wolf::raft::RaftBootstrapReq _req{};
        _req.set_msg_id(_msg_id);
        _req.set_number_of_nodes(7);

        wolf::raft::RaftBootstrapRes _res{};

        auto send_grpc_fn =
            [&](_In_ wolf::raft::RaftBootstrapReq request,
                _Inout_ wolf::raft::RaftBootstrapRes response) -> boost::asio::awaitable<void> {
          return _client
              .send_unary<&wolf::raft::Raft::Stub::PrepareAsyncBootstrap, wolf::raft::Raft::Stub,
                          wolf::raft::RaftBootstrapReq, wolf::raft::RaftBootstrapRes>(request,
                                                                                      response);
        };

        auto timeout = std::chrono::seconds(3);
        _client.exec(std::move(timeout), send_grpc_fn, _req, _res);

        std::cout << "grpc client sent unary request. msg_id: " << _req.msg_id()
                  << " number_of_nodes: " << _req.number_of_nodes() << std::endl;
        return {};
      },
      [](const w_trace& p_trace) {
        const auto _msg =
            wolf::format("grpc_client_unary_test got an error: {}", p_trace.to_string());
        BOOST_ERROR(_msg);
      },
      [] { BOOST_ERROR("grpc_client_unary_test got an error!"); });

  std::cout << "leaving test case " << _test_name << std::endl;
}

//BOOST_AUTO_TEST_CASE(grpc_client_stream_test) {
//  const wolf::system::w_leak_detector _detector = {};
//
//  const std::string url = "localhost";
//
//  std::unique_ptr<w_grpc_client> client = std::make_unique<w_grpc_client>();
//
//  boost::leaf::try_handle_all(
//      [&]() -> boost::leaf::result<void> {
//        auto send_grpc =
//            [&](_In_ wolf::raft::RaftBootstrapReq& request,
//                _Inout_ wolf::raft::RaftBootstrapRes& response) -> boost::asio::awaitable<void> {
//          return client->send_stream<&wolf::raft::Raft::Stub::PrepareAsyncBootstrapClientStreaming,
//                                           wolf::raft::Raft::Stub, wolf::raft::RaftBootstrapReq,
//                                           wolf::raft::RaftBootstrapRes>(request, response);
//        };
//
//        client->init(url, 50051);
//
//        wolf::raft::RaftBootstrapReq request;
//        request.set_msg_id("client streaming");
//        request.set_number_of_nodes(6);
//        wolf::raft::RaftBootstrapRes response;
//
//        auto timeout = std::chrono::seconds(3);
//        client->run(std::move(timeout), send_grpc, request, response);
//
//        std::cout << "grpc client sent streaming request. msg_id: " << request.msg_id()
//                  << " number_of_nodes: " << request.number_of_nodes() << std::endl;
//        return {};
//      },
//      [](const w_trace& p_trace) {
//        const auto _msg = wolf::format("grpc_client_stream_test got an error: {}", p_trace.to_string());
//        BOOST_ERROR(_msg);
//      },
//      [] { BOOST_ERROR("grpc_client_stream_test got an error!"); });
//}
//
//BOOST_AUTO_TEST_CASE(grpc_client_bidirectional_test) {
//  const wolf::system::w_leak_detector _detector = {};
//
//  const std::string url = "localhost";
//  std::unique_ptr<w_grpc_client> client = std::make_unique<w_grpc_client>();
//
//  boost::leaf::try_handle_all(
//      [&]() -> boost::leaf::result<void> {
//        client->init(url, 50051);
//        wolf::raft::RaftBootstrapReq request;
//        request.set_msg_id("client bidirectional");
//        request.set_number_of_nodes(7);
//        wolf::raft::RaftBootstrapRes response;
//
//        auto send_grpc =
//            [&](_In_ wolf::raft::RaftBootstrapReq& request,
//                _Inout_ wolf::raft::RaftBootstrapRes& response) -> boost::asio::awaitable<void> {
//          return client->send_bidirectional<
//              &wolf::raft::Raft::Stub::PrepareAsyncBootstrapBidirectionalStreaming,
//              wolf::raft::Raft::Stub, wolf::raft::RaftBootstrapReq, wolf::raft::RaftBootstrapRes>(
//              request, response);
//        };
//
//        auto timeout = std::chrono::seconds(3);
//        client->run(std::move(timeout), send_grpc, request, response);
//        std::cout << "grpc client sent bidirectional request. msg_id: " << request.msg_id()
//                  << " number_of_nodes: " << request.number_of_nodes() << std::endl;
//        return {};
//      },
//      [](const w_trace& p_trace) {
//        const auto _msg =
//            wolf::format("grpc_client_bidirectional_test got an error: {}", p_trace.to_string());
//        BOOST_ERROR(_msg);
//      },
//      [] { BOOST_ERROR("grpc_client_bidirectional_test got an error!"); });
//}

#endif