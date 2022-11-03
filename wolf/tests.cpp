#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp> // NOLINT

#include <iostream>
#include "wolf.hpp"

// include all tests
#include "system/test.hpp"

TEST_CASE("wolf", "global namespace")
{
	auto _wolf_version = wolf::w_init();
	REQUIRE(_wolf_version.empty() == false);

	auto _path_res = wolf::system::w_current_process_dir();
	REQUIRE(_path_res.has_error() == false);

	auto _path_str = _path_res.value();
	REQUIRE(_path_str.string().empty() == false);
}


//#include <system/io/w_tcp_client.hpp>
//#include <system/io/w_tcp_server.hpp>

//#define _CRTDBG_MAP_ALLOC


//int main()
//{

	//_CrtMemState sOld;
	//_CrtMemState sNew;
	//_CrtMemState sDiff;
	//_CrtMemCheckpoint(&sOld); //take a snapshot

	// using namespace wolf::system::io;
	// using namespace boost::asio;
	// using tcp = boost::asio::ip::tcp;

	// auto _io = boost::asio::io_context();
	// w_socket_options _opts = {};
	// _opts.timeout = std::chrono::milliseconds(3000);

	// tcp::endpoint _endpoint = { tcp::v4(), 8080 };

	// auto t1 = std::jthread([&]()
	// 	{
	// 		//stop server
	// 		std::this_thread::sleep_for(std::chrono::seconds(20));
	// 		_io.stop();
	// 	});

	// w_tcp_server::init(_io, std::move(_endpoint), std::move(_opts),
	// 	[](std::span<char, W_MAX_BUFFER_SIZE> p_data, size_t p_read_bytes) -> wolf::w_result<boost::system::errc::errc_t>
	// 	{
	// 		std::cout << "read: " << p_read_bytes << std::endl;
	// 		return boost::system::errc::connection_aborted;
	// 	});
	// _io.run();


	//auto t2 = std::jthread([&]()
	//	{
	//		auto _c_io = std::make_shared<boost::asio::io_context>();

	//		std::this_thread::sleep_for(std::chrono::seconds(3));

	//		boost::asio::co_spawn(*_c_io.get(), [&]() -> boost::asio::awaitable<void>
	//			{
	//				auto _client = w_tcp_client(_c_io, _opts);
	//				/*	_client.on_error_callback.connect([](const boost::system::system_error& p_error_code)
	//						{
	//							std::cout << "error:" << p_error_code.what() << std::endl;
	//						});*/

	//					auto _ret = w_ret();
	//					const auto _endpoint = boost::asio::ip::tcp::endpoint();
	//					auto _results = co_await _client.resolve(_endpoint);

	//					for (auto _res : _results)
	//					{
	//						std::cout << _res.host_name() << ":" << _res.service_name() << std::endl;
	//					}

	//				/*if (_endpoints.size() == 0)
	//				{
	//					co_return;
	//				}*/

	//				//const auto _endpoint = _endpoints.cbegin();
	//				//std::cout << "endpoint just resolved " << _endpoint->host_name() << ":" 
	//					//<< _endpoint->service_name() << std::endl;

	//			},
	//			boost::asio::detached);

	//		_c_io->run();
	//		std::cout << "client just released" << std::endl;
	//	});

	//std::cout << "finished\n";

	//char* s = new char[17];
	//strcpy_s(s, 17, "stackoverflow_pb");
	//char* ss = new char[14];
	//strcpy_s(ss, 14, "stackoverflow");
	//delete[] ss;
	
	//_CrtMemCheckpoint(&sNew); //take a snapshot 
	//if (_CrtMemDifference(&sDiff, &sOld, &sNew) == TRUE) // if there is a difference
	//{
	//	OutputDebugString("-----------_CrtMemDumpStatistics ---------");
	//	_CrtMemDumpStatistics(&sDiff);
	//	OutputDebugString("-----------_CrtMemDumpAllObjectsSince ---------");
	//	_CrtMemDumpAllObjectsSince(&sOld);
	//	OutputDebugString("-----------_CrtDumpMemoryLeaks ---------");
	//	_CrtDumpMemoryLeaks();
	//}

	//return 0;
//}


//
//template <typename Scheduler, typename F>
//auto run_on(Scheduler&& s, F&& func) {
//    return unifex::then(unifex::schedule((Scheduler&&)s), (F&&)func);
//}
//
//TEST_CASE("rist", "rist sender & receiver")
//{
//    using namespace unifex;
//
//    static_thread_pool tpContext;
//    auto tp = tpContext.get_scheduler();
//    std::atomic<int> x = 0;
//
//    sync_wait(
//        when_all(
//        run_on(
//            tp,
//            [&] {
//                ++x;
//                std::printf("task 1\n");
//            }),
//        run_on(
//            tp,
//            [&] {
//                ++x;
//                std::printf("task 2\n");
//            }),
//                run_on(tp, [&] {
//                ++x;
//                std::printf("task 3\n");
//                    })));
//}
