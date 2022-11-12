/*
	Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#include <gtest.hpp>
#include <wolf.hpp>
#include <system/w_leak_detector.hpp>

TEST(Wolf, GlobalNamespace)
{
	const wolf::system::w_leak_detector _detector = {};

	const auto _wolf_version = wolf::w_init();
	EXPECT_EQ(_wolf_version.empty(), false);
}

#include <system/tests.hpp>
//#include <stream/test.hpp>
//#include <media/test.hpp>
//#include <render/test.hpp>




































	


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
