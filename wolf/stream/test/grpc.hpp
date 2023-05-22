///*
//    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
//    https://github.com/WolfEngine/WolfEngine
//*/
//
//#if defined(WOLF_TEST) && defined(WOLF_STREAM_GRPC)
//
//#pragma once
//
//#include <api/v1/game.grpc.pb.h>
//#include <api/v1/game.pb.h>
//// #include <hello.grpc.pb.h>
//// #include <hello.pb.h>
//
//#include <boost/test/included/unit_test.hpp>
//#include <stream/grpc/w_grpc_client.hpp>
//#include <system/w_leak_detector.hpp>
//#include <wolf.hpp>
//
//using w_grpc_client = wolf::stream::rpc::w_grpc_client;
//
//BOOST_AUTO_TEST_CASE(grpc_client_unary_test) {
//  // const wolf::system::w_leak_detector _detector = {};
//  constexpr auto _test_name = "'grpc_client_unary_test'";
//  std::cout << "entering test case " << _test_name << std::endl;
//
//  boost::leaf::try_handle_all(
//      [&]() -> boost::leaf::result<void> {
//        // create a client
//        w_grpc_client _client{};
//        BOOST_LEAF_CHECK(_client.init("http://go-proxy-shtl.alphaco.cloud",
//                                       gsl::narrow_cast<short>(9090)));
//        // BOOST_LEAF_CHECK(
//        //     _client.init("https://grpcb.in", gsl::narrow_cast<short>(9001)));
//
//        api::v1::GameServiceListRequest _req{};
//        _req.set_page_size(10);
//
//        api::v1::GameServiceListResponse _res{};
//
//        auto send_grpc_fn =
//            [&](_In_ api::v1::GameServiceListRequest request,
//                _Inout_ api::v1::GameServiceListResponse response)
//            -> boost::asio::awaitable<void> {
//          return _client.send_unary<
//              &api::v1::GameService::Stub::PrepareAsyncList,
//              api::v1::GameService::Stub, api::v1::GameServiceListRequest,
//              api::v1::GameServiceListResponse>(request, response);
//        };
//
//        auto timeout = std::chrono::seconds(5);
//        _client.exec(std::move(timeout), send_grpc_fn, _req, _res);
//
//        const auto _size = _res.games().size();
//        std::cout << "grpc client sent unary request " << _size << std::endl;
//
//        // greet::HelloRequest _req{};
//        // _req.set_greeting("hello");
//
//        // greet::HelloReply _res{};
//
//        // auto send_grpc_fn = [&](_In_ greet::HelloRequest request,
//        //                         _Inout_ greet::HelloReply response)
//        //     -> boost::asio::awaitable<void> {
//        //   return _client
//        //       .send_unary<&greet::HelloService::Stub::PrepareAsyncSayHello,
//        //                   greet::HelloService::Stub, greet::HelloRequest,
//        //                   greet::HelloReply>(request, response);
//        // };
//
//        // auto timeout = std::chrono::seconds(5);
//        // _client.exec(std::move(timeout), send_grpc_fn, _req, _res);
//
//        // std::cout << "grpc client sent unary request " << _res.reply()
//        //           << std::endl;
//
//        return {};
//      },
//      [](const w_trace& p_trace) {
//        const auto _msg = wolf::format(
//            "grpc_client_unary_test got an error: {}", p_trace.to_string());
//        BOOST_ERROR(_msg);
//      },
//      [] { BOOST_ERROR("grpc_client_unary_test got an error!"); });
//
//  std::cout << "leaving test case " << _test_name << std::endl;
//}
//
//// BOOST_AUTO_TEST_CASE(grpc_client_stream_test) {
////   const wolf::system::w_leak_detector _detector = {};
////
////   const std::string url = "localhost";
////
////   std::unique_ptr<w_grpc_client> client = std::make_unique<w_grpc_client>();
////
////   boost::leaf::try_handle_all(
////       [&]() -> boost::leaf::result<void> {
////         auto send_grpc =
////             [&](_In_ wolf::raft::RaftBootstrapReq& request,
////                 _Inout_ wolf::raft::RaftBootstrapRes& response) ->
////                 boost::asio::awaitable<void> {
////           return
////           client->send_stream<&wolf::raft::Raft::Stub::PrepareAsyncBootstrapClientStreaming,
////                                            wolf::raft::Raft::Stub,
////                                            wolf::raft::RaftBootstrapReq,
////                                            wolf::raft::RaftBootstrapRes>(request,
////                                            response);
////         };
////
////         client->init(url, 50051);
////
////         wolf::raft::RaftBootstrapReq request;
////         request.set_msg_id("client streaming");
////         request.set_number_of_nodes(6);
////         wolf::raft::RaftBootstrapRes response;
////
////         auto timeout = std::chrono::seconds(3);
////         client->run(std::move(timeout), send_grpc, request, response);
////
////         std::cout << "grpc client sent streaming request. msg_id: " <<
////         request.msg_id()
////                   << " number_of_nodes: " << request.number_of_nodes() <<
////                   std::endl;
////         return {};
////       },
////       [](const w_trace& p_trace) {
////         const auto _msg = wolf::format("grpc_client_stream_test got an error:
////         {}", p_trace.to_string()); BOOST_ERROR(_msg);
////       },
////       [] { BOOST_ERROR("grpc_client_stream_test got an error!"); });
//// }
////
//// BOOST_AUTO_TEST_CASE(grpc_client_bidirectional_test) {
////   const wolf::system::w_leak_detector _detector = {};
////
////   const std::string url = "localhost";
////   std::unique_ptr<w_grpc_client> client = std::make_unique<w_grpc_client>();
////
////   boost::leaf::try_handle_all(
////       [&]() -> boost::leaf::result<void> {
////         client->init(url, 50051);
////         wolf::raft::RaftBootstrapReq request;
////         request.set_msg_id("client bidirectional");
////         request.set_number_of_nodes(7);
////         wolf::raft::RaftBootstrapRes response;
////
////         auto send_grpc =
////             [&](_In_ wolf::raft::RaftBootstrapReq& request,
////                 _Inout_ wolf::raft::RaftBootstrapRes& response) ->
////                 boost::asio::awaitable<void> {
////           return client->send_bidirectional<
////               &wolf::raft::Raft::Stub::PrepareAsyncBootstrapBidirectionalStreaming,
////               wolf::raft::Raft::Stub, wolf::raft::RaftBootstrapReq,
////               wolf::raft::RaftBootstrapRes>( request, response);
////         };
////
////         auto timeout = std::chrono::seconds(3);
////         client->run(std::move(timeout), send_grpc, request, response);
////         std::cout << "grpc client sent bidirectional request. msg_id: " <<
////         request.msg_id()
////                   << " number_of_nodes: " << request.number_of_nodes() <<
////                   std::endl;
////         return {};
////       },
////       [](const w_trace& p_trace) {
////         const auto _msg =
////             wolf::format("grpc_client_bidirectional_test got an error: {}",
////             p_trace.to_string());
////         BOOST_ERROR(_msg);
////       },
////       [] { BOOST_ERROR("grpc_client_bidirectional_test got an error!"); });
//// }
//
//#endif