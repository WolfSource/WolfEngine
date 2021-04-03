#pragma once

#include "../w_test/base.hpp"

#include "w_chrono.h"
#include "w_gametime.h"
#include "w_timespan.h"

#include "../w_concurrency/w_thread.h"

TEST_CASE("w_chrono/w_chrono")
{
	w_timespec_t _c1_now = w_chrono_now();

	//sleep for 3 seconds
	w_thread_current_sleep_for_seconds(3);

	w_timespec_t _c2_now = w_chrono_now();

	//check the duration
	w_timespec_t _duration = w_chrono_duration(&_c1_now, &_c2_now);
	REQUIRE(_duration.tv_sec == 3);

	double _chrono_timespec_to_sec = w_chrono_timespec_to_sec(&_c2_now);
	REQUIRE(_chrono_timespec_to_sec >= 3.);

	double _chrono_timespec_to_milisec = w_chrono_timespec_to_milisec(&_c2_now);
	REQUIRE(_chrono_timespec_to_milisec >= 3000.);

	double chrono_timespec_to_microsec = w_chrono_timespec_to_microsec(&_c2_now);
	REQUIRE(chrono_timespec_to_microsec >= 3000000.);

	double _chrono_timespec_to_nanosec = w_chrono_timespec_to_nanosec(&_c2_now);
	REQUIRE(_chrono_timespec_to_nanosec >= 3000000.);

	double _nano_duration = w_chrono_duration_nanoseconds(&_c1_now, &_c2_now);
	REQUIRE(_nano_duration >= 3000000000.);

	double _micro_duration = w_chrono_duration_microseconds(&_c1_now, &_c2_now);
	REQUIRE(_micro_duration >= 3000000.);

	double _milli_duration = w_chrono_duration_milliseconds(&_c1_now, &_c2_now);
	REQUIRE(_milli_duration >= 3000.);

	double _sec_duration = w_chrono_duration_seconds(&_c1_now, &_c2_now);
	REQUIRE(_sec_duration >= 3);
}

TEST_CASE("w_chrono/w_gametime")
{
	w_mem_pool _mem_pool = w_init();

	w_gametime _gametime = nullptr;
	W_RESULT _ret = w_gametime_init(_mem_pool, &_gametime);
	REQUIRE(_ret == W_SUCCESS);
	REQUIRE(_gametime != nullptr);

	uint64_t _ticks = 100;
	w_gametime_set_target_elapsed_ticks(_gametime, _ticks);

	double _secs = 11.5;
	w_gametime_set_target_elapsed_seconds(_gametime, _secs);

	w_gametime_enable_fixed_time_step(_gametime);
	w_gametime_disable_fixed_time_step(_gametime);
	w_gametime_tick(_gametime,
		[](w_gametime pGameTime)
		{
			std::cout << "gametime ticked" << std::endl;
		});

	uint64_t _elapsed_ticks = w_gametime_get_elapsed_ticks(_gametime);
	REQUIRE(_elapsed_ticks == 0);

	double _elapsed_secs = w_gametime_get_elapsed_seconds(_gametime);
	REQUIRE(_elapsed_secs == 0);

	uint64_t _total_ticks = w_gametime_get_total_ticks(_gametime);
	REQUIRE(_total_ticks == 0);

	double _total_seconds = w_gametime_get_total_seconds(_gametime);
	REQUIRE(_total_seconds == 0);

	uint32_t _t_g__get_frame_count = w_gametime_get_frame_count(_gametime);
	REQUIRE(_t_g__get_frame_count == 1);

	uint32_t _fps = w_gametime_get_frames_per_second(_gametime);
	REQUIRE(_fps == 0);

	bool _fixed_time_step = w_gametime_get_fixed_time_step(_gametime);
	REQUIRE(_fixed_time_step == false);

	w_fini(&_mem_pool);
}

TEST_CASE("w_chrono/w_timespan")
{
	w_mem_pool _mem_pool = w_init();

	w_timespan _t_0 = w_timespan_init_from_zero(_mem_pool);
	REQUIRE(_t_0->ticks == 0);

	w_timespan _t_min = w_timespan_init_from_min_value(_mem_pool);
	REQUIRE(_t_min->overflowed == false);

	w_timespan _t_max = w_timespan_init_from_max_value(_mem_pool);
	REQUIRE(_t_max->ticks == 9223372036854775807);

	w_timespan _t_now = w_timespan_init_from_now(_mem_pool);
	REQUIRE(_t_now->ticks != 0);

	w_timespan _t_days = w_timespan_init_from_days(_mem_pool, 31);
	REQUIRE(_t_days->ticks == 2678400000000000);

	w_timespan _t_hours = w_timespan_init_from_hours(_mem_pool, 1);
	REQUIRE(_t_hours->ticks == 3600000000000);

	w_timespan _t_minu = w_timespan_init_from_minutes(_mem_pool, 2);
	REQUIRE(_t_minu->ticks == 120000000000);

	w_timespan _t_sec = w_timespan_init_from_seconds(_mem_pool, 3);
	REQUIRE(_t_sec->ticks == 3000000000);

	w_timespan _t_millisec = w_timespan_init_from_milliseconds(_mem_pool, 50);
	REQUIRE(_t_millisec->ticks == 50000000);

	w_timespan _t_tick = w_timespan_init_from_ticks(_mem_pool, 5);
	REQUIRE(_t_tick->ticks == 5);

	w_timespan _t_string = w_timespan_init_from_string(_mem_pool, "01:02:03:04:000");
	REQUIRE(_t_string->ticks == 93784000000000);

	w_timespan _t_wstring = w_timespan_init_from_wstring(_mem_pool, L"00:02:03:04:000");
	REQUIRE(_t_string->ticks != 7384000000000);

	w_timespan _t_short = w_timespan_init_from_shorttime(_mem_pool, 2, 3, 4);
	REQUIRE(_t_short->ticks == 7384000000000);

	w_timespan _t_long = w_timespan_init_from_longtime(_mem_pool, 3, 4, 5, 8, 10);
	REQUIRE(_t_long->ticks == 273908010000000);

	w_timespan _t_add = w_timespan_add(_mem_pool, _t_long, _t_short);
	REQUIRE(_t_add->ticks == 281292010000000);

	w_timespan_add_by_ref(_t_long, _t_short);

	const char* _time_to_string = w_timespan_to_string(_mem_pool, _t_short, ":");
	const char* _time_origin = "0:2:3:4:000";
	int result = strcmp(_time_to_string, _time_origin);
	REQUIRE(result == 0);

	const wchar_t* s = L":";
	const wchar_t* _time_to_wstring = w_timespan_to_wstring(_mem_pool, _t_short, s);
	const wchar_t* _time_originW = L"0:2:3:4:000";
	int w_result = wcscmp(_time_to_wstring, _time_originW);
	REQUIRE(w_result == 0);

	int _get_days = w_timespan_get_days(_t_long);
	REQUIRE(_get_days == 3);

	double _get_total_days = w_timespan_get_total_days(_t_tick);
	REQUIRE(_get_total_days == 5.7870370370370365e-14);

	int _get_hours = w_timespan_get_hours(_t_short);
	REQUIRE(_get_hours == 2);

	double _get_total_hours = w_timespan_get_total_hours(_t_short);
	REQUIRE(_get_total_hours == 2.0511111111111111);

	int _get_minutes = w_timespan_get_minutes(_t_short);
	REQUIRE(_get_minutes != 2);

	double get_total_minutes = w_timespan_get_total_minutes(_t_short);
	REQUIRE(get_total_minutes == 123.06666666666668);

	int _get_seconds = w_timespan_get_seconds(_t_short);
	REQUIRE(_get_seconds == 4);

	double _get_total_seconds = w_timespan_get_total_seconds(_t_short);
	REQUIRE(_get_total_seconds == 7384.0000000000009);

	int _get_milliseconds = w_timespan_get_milliseconds(_t_short);
	REQUIRE(_get_milliseconds == 0);

	double _get_total_milliseconds = w_timespan_get_total_milliseconds(_t_short);
	REQUIRE(_get_total_milliseconds == 7384000.0000000000);

	int _timespan_get_microseconds = w_timespan_get_microseconds(_t_short);
	REQUIRE(_timespan_get_microseconds == -592);

	double _timespan_get_total_microseconds = w_timespan_get_total_microseconds(_t_short);
	REQUIRE(_timespan_get_total_microseconds == 7384000000.0000000);

	const char* _timespan_get_current_date_time_string = w_timespan_get_current_date_time_string(_mem_pool);
	REQUIRE(_timespan_get_current_date_time_string != nullptr);

	const wchar_t* _timespan_get_current_date_time_wstring = w_timespan_get_current_date_time_wstring(_mem_pool);
	REQUIRE(_timespan_get_current_date_time_wstring != nullptr);

	w_fini(&_mem_pool);
}
