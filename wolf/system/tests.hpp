/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include "test/gametime.hpp"
#include "test/process.hpp"
#include "test/signal_slots.hpp"
#include "test/trace.hpp"

#include "socket/test/tcp.hpp"

//#include <gtest.hpp> 
//#include "w_gametime.hpp"
//#include "w_leak_detector.hpp"
//#include "socket/w_tcp_server.hpp" 
//#include "socket/w_tcp_client.hpp" 
//
//TEST(WolfSystemGameTime, w_gametime_test)
//{
//	const wolf::system::w_leak_detector _detector = {};
//
//	std::once_flag _flag;
//	bool _done = false;
//
//	constexpr double _stop_after_secs_1 = 3.0;
//	constexpr double _stop_after_secs_2 = 5.0;
//
//	constexpr double _target_elapsed_secs = 1.0 / 50.0; //50 fps
//
//	using w_gametime = wolf::system::w_gametime;
//
//	auto _gametime = w_gametime();
//	_gametime.reset();
//	_gametime.set_fixed_time_step(true);
//	_gametime.set_target_elapsed_secs(_target_elapsed_secs);
//
//	auto _total_seconds = _gametime.get_total_secs();
//#ifdef __clang__
//#pragma unroll
//#endif
//	while (_total_seconds < _stop_after_secs_1)
//	{
//		_gametime.tick([&]()
//			{
//				std::call_once(_flag, [&]() {
//					_done = true;
//					});
//
//				std::cout << std::setprecision(3) << 
//					"fixed game time step ticked on " << _total_seconds << "    " <<
//					"elapsed seconds:" << _gametime.get_elapsed_secs() << "    " <<
//					"fps: " << _gametime.get_fps() << "    " <<
//					"frames count: " << _gametime.get_frames_count() << std::endl;
//			});
//
//		_total_seconds = _gametime.get_total_secs();
//	}
//
//	EXPECT_EQ(_done, true);
//
//	auto _gametime_new = std::move(_gametime);
//	_gametime_new.reset();
//	EXPECT_EQ(_gametime_new.get_fps(), 0);
//
//	EXPECT_EQ(_gametime_new.get_is_fixed_time_step(), true);
//
//	_gametime_new.set_fixed_time_step(false);
//	while (_gametime_new.get_total_secs() < _stop_after_secs_2)
//	{
//		_gametime_new.tick([&]()
//			{
//				EXPECT_TRUE(false) << std::setprecision(3) <<
//					"unfixed game time step ticked on " << _total_seconds << "    " <<
//					"elapsed seconds:" << _gametime_new.get_elapsed_secs() << "    " <<
//					"fps: " << _gametime_new.get_fps() << "    " <<
//					"frames count: " << _gametime_new.get_frames_count() << std::endl;
//			});
//
//		_total_seconds = _gametime_new.get_total_secs();
//	}
//
//	EXPECT_GT(_gametime_new.get_fps(), 50);
//
//}
//
//TEST(WolfSystemWSocket, w_tcp_server_test)
//{
//	const wolf::system::w_leak_detector _detector = {};
//
//	using namespace wolf::system::socket;
//	using namespace boost::asio;
//	using tcp = boost::asio::ip::tcp;
//
//	auto _io = boost::asio::io_context();
//	w_socket_options _opts = {};
//	_opts.timeout = std::chrono::milliseconds(3000);
//
//	tcp::endpoint _endpoint = { tcp::v4(), 8080 };
//
//	auto t1 = std::jthread([&]()
//		{
//			//stop server
//			std::this_thread::sleep_for(std::chrono::seconds(5));
//	_io.stop();
//		});
//
//	w_tcp_server::init(
//		_io,
//		std::move(_endpoint),
//		std::move(_opts),
//		[](const std::string& p_conn_id, gsl::span<char> p_data, size_t p_read_bytes) -> auto
//		{
//			EXPECT_TRUE(false) << "tcp server just read: " << p_read_bytes << " bytes" << std::endl;
//	return boost::system::errc::connection_aborted;
//		},
//		[](const std::string& p_conn_id)
//		{
//			EXPECT_TRUE(false) << "timeout for connection: " << p_conn_id << std::endl;
//		});
//	_io.run();
//}
//
//TEST(WolfSystemWTrace, w_trace_test)
//{
//	const wolf::system::w_leak_detector _detector = {};
//
//	const auto _function = []() -> boost::leaf::result<void>
//	{
//		return boost::leaf::new_error();
//	};
//
//	// how to use w_trace
//	boost::leaf::try_handle_all(
//		[&]() -> boost::leaf::result<void>
//		{
//			BOOST_LEAF_CHECK(_function());
//			std::cout << "success!" << std::endl;
//			return { };
//		},
//		[](w_trace const& p_trace)
//		{
//			EXPECT_TRUE(false) << "caught a test exception! trace info:" << std::endl << p_trace;
//		},
//		[] {
//			EXPECT_TRUE(false) << "caught a test exception!" << std::endl;
//		});
//}
//
//#ifdef WOLF_SYSYEM_SIG_SLOT
//
//TEST(WolfSystemWSigSlot, w_sig_slot_test)
//{
//	wolf::system::w_leak_detector _detector = {};
//
//	auto sig = wolf::system::w_sig_slot<void()>();
//	sig.connect([]()
//		{
//			std::cout << "hello from slot 1" << std::endl;
//		});
//	sig.connect([]()
//		{
//			std::cout << "hello from slot 2" << std::endl;
//		});
//
//	sig();
//}
//
//#endif