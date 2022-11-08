/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <DISABLE_ANALYSIS_BEGIN>
#include <catch2/catch.hpp>
#include <DISABLE_ANALYSIS_END>

#include "w_gametime.hpp"
#include "w_trace.hpp"
#include <iostream>
#include <xcall_once.h>

#include <system/w_leak_detector.hpp>

#include "socket/test.hpp"

TEST_CASE("wolf::system::w_gametime", "w_gametime")
{
	wolf::system::w_leak_detector _detector = {};

	std::once_flag _flag;
	bool _done = false;

	constexpr double _stop_after_secs_1 = 3.0;
	constexpr double _stop_after_secs_2 = 5.0;

	constexpr double _target_elapsed_secs = 1.0 / 50.0; //50 fps

	using w_gametime = wolf::system::w_gametime;

	auto _gametime = w_gametime();
	_gametime.reset();
	_gametime.set_fixed_time_step(true);
	_gametime.set_target_elapsed_secs(_target_elapsed_secs);

	auto _total_seconds = _gametime.get_total_secs();
#ifdef __clang__
#pragma unroll
#endif
	while (_total_seconds < _stop_after_secs_1)
	{
		_gametime.tick([&]()
			{
				std::call_once(_flag, [&]() {
					_done = true;
					});

				std::cout << std::setprecision(3) << 
					"fixed game time step ticked on " << _total_seconds << "    " <<
					"elapsed seconds:" << _gametime.get_elapsed_secs() << "    " <<
					"fps: " << _gametime.get_fps() << "    " <<
					"frames count: " << _gametime.get_frames_count() << std::endl;
			});

		_total_seconds = _gametime.get_total_secs();
	}

	REQUIRE(_done == true);

	auto _gametime_new = std::move(_gametime);
	_gametime_new.reset();
	REQUIRE(_gametime_new.get_fps() == 0);

	REQUIRE(_gametime_new.get_is_fixed_time_step() == true);

	_gametime_new.set_fixed_time_step(false);
	while (_gametime_new.get_total_secs() < _stop_after_secs_2)
	{
		_gametime_new.tick([&]()
			{
				std::cout << std::setprecision(3) <<
					"unfixed game time step ticked on " << _total_seconds << "    " <<
					"elapsed seconds:" << _gametime_new.get_elapsed_secs() << "    " <<
					"fps: " << _gametime_new.get_fps() << "    " <<
					"frames count: " << _gametime_new.get_frames_count() << std::endl;
			});

		_total_seconds = _gametime_new.get_total_secs();
	}

	REQUIRE(_gametime_new.get_fps() > 50);

}

#ifdef WOLF_SYSYEM_SIG_SLOT

TEST_CASE("wolf::system::w_sig_slot", "w_sig_slot")
{
	wolf::system::w_leak_detector _detector = {};

	auto sig = wolf::system::w_sig_slot<void()>();
	sig.connect([]()
		{
		std::cout << "hello from slot 1" << std::endl;
		});
	sig.connect([]()
		{
		std::cout << "hello from slot 2" << std::endl;
		});

	sig();
}

#endif

TEST_CASE("wolf::system::w_trace", "w_trace")
{
	wolf::system::w_leak_detector _detector = {};

	const auto _function = []() -> boost::leaf::result<void>
	{
		auto _result = wolf::system::w_current_process_path();
		if (_result.has_error())
		{
			return _result.error();
		}
		return {};
	};

	/// how to use w_trace
	boost::leaf::try_handle_all(
		[&]() -> boost::leaf::result<void>
		{
			BOOST_LEAF_CHECK(_function());
			std::cout << "success!" << std::endl;
			return { };
		},
		[](w_trace const& p_trace)
		{
			std::cout << "caught a test exception! trace info:" << std::endl << p_trace;
		},
			[] {
			std::cout << "caught a test exception!" << std::endl;
		});
}
