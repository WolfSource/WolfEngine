/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#if defined(WOLF_TEST) && defined(WOLF_STREAM_GRPC)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <stream/grpc/w_grpc.hpp>

#include <raft.grpc.pb.h>

BOOST_AUTO_TEST_CASE(grpc_client_unary_test) {
    const wolf::system::w_leak_detector _detector = {};
    
    const std::string url = "localhost";
    std::unique_ptr<w_grpc_client> client = std::make_unique<w_grpc_client>();

    boost::leaf::try_handle_all(
        [&]() -> boost::leaf::result<void> {

          client->init(url, 50051);
          wolf::raft::RaftBootstrapReq request;
          request.set_msg_id("unary");
          request.set_number_of_nodes(7);
          wolf::raft::RaftBootstrapRes response;

          auto send_grpc =
              [&](_In_ wolf::raft::RaftBootstrapReq request,
                  _Inout_ wolf::raft::RaftBootstrapRes response) -> boost::asio::awaitable<void> {
            return client
                ->send_unary<&wolf::raft::Raft::Stub::PrepareAsyncBootstrap,
                                            wolf::raft::Raft::Stub, wolf::raft::RaftBootstrapReq,
                                            wolf::raft::RaftBootstrapRes>(request, response);
          };

          client->run(send_grpc, request, response);
          std::cout << "grpc client sent unary request. msg_id: " << 
              request.msg_id() 
              << " number_of_nodes: "
              << request.number_of_nodes()
              << std::endl;
          return {}; 
        },
        [](const w_trace& p_trace) {
          const auto _msg = wolf::format("grpc_client_unary_test got an error: {}",
                                         p_trace.to_string());
          BOOST_ERROR(_msg);
        },
        [] { BOOST_ERROR("grpc_client_unary_test got an error!"); 
    });
}

BOOST_AUTO_TEST_CASE(grpc_client_stream_test) {
  const wolf::system::w_leak_detector _detector = {};

  const std::string url = "localhost";

  std::unique_ptr<w_grpc_client> client = std::make_unique<w_grpc_client>();

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        auto send_grpc =
            [&](_In_ wolf::raft::RaftBootstrapReq& request,
                _Inout_ wolf::raft::RaftBootstrapRes& response) -> boost::asio::awaitable<void> {
          return client->send_stream<&wolf::raft::Raft::Stub::PrepareAsyncBootstrapClientStreaming,
                                           wolf::raft::Raft::Stub, wolf::raft::RaftBootstrapReq,
                                           wolf::raft::RaftBootstrapRes>(request, response);
        };

        client->init(url, 50051);

        wolf::raft::RaftBootstrapReq request;
        request.set_msg_id("client streaming");
        request.set_number_of_nodes(6);
        wolf::raft::RaftBootstrapRes response;

        client->run(send_grpc, request, response);

        std::cout << "grpc client sent streaming request. msg_id: " << request.msg_id()
                  << " number_of_nodes: " << request.number_of_nodes() << std::endl;
        return {};
      },
      [](const w_trace& p_trace) {
        const auto _msg = wolf::format("grpc_client_stream_test got an error: {}", p_trace.to_string());
        BOOST_ERROR(_msg);
      },
      [] { BOOST_ERROR("grpc_client_stream_test got an error!"); });
}

BOOST_AUTO_TEST_CASE(grpc_client_bidirectional_test) {
  const wolf::system::w_leak_detector _detector = {};

  const std::string url = "localhost";
  std::unique_ptr<w_grpc_client> client = std::make_unique<w_grpc_client>();

  boost::leaf::try_handle_all(
      [&]() -> boost::leaf::result<void> {
        client->init(url, 50051);
        wolf::raft::RaftBootstrapReq request;
        request.set_msg_id("client bidirectional");
        request.set_number_of_nodes(7);
        wolf::raft::RaftBootstrapRes response;

        auto send_grpc =
            [&](_In_ wolf::raft::RaftBootstrapReq& request,
                _Inout_ wolf::raft::RaftBootstrapRes& response) -> boost::asio::awaitable<void> {
          return client->send_bidirectional<
              &wolf::raft::Raft::Stub::PrepareAsyncBootstrapBidirectionalStreaming,
              wolf::raft::Raft::Stub,
              wolf::raft::RaftBootstrapReq, 
              wolf::raft::RaftBootstrapRes>(request,
                                           response);
        };

        client->run(send_grpc, request, response);
        std::cout << "grpc client sent bidirectional request. msg_id: " << request.msg_id()
                  << " number_of_nodes: " << request.number_of_nodes() << std::endl;
        return {};
      },
      [](const w_trace& p_trace) {
        const auto _msg = wolf::format("grpc_client_bidirectional_test got an error: {}", p_trace.to_string());
        BOOST_ERROR(_msg);
      },
      [] { BOOST_ERROR("grpc_client_bidirectional_test got an error!"); });
}

#endif