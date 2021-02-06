//catch2 https://github.com/catchorg/Catch2/releases/download/v2.13.4/catch.hpp
#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include <wolf.h>
#include <chrono/w_chrono.h>
#include <chrono/w_gametime.h>
#include <compression/w_compress.h>
#include <concurrency/w_atomic.h>
#include <concurrency/w_concurrent_queue.h>
#include <concurrency/w_condition_variable.h>
#include <concurrency/w_mutex.h>
#include <concurrency/w_thread.h>
#include <concurrency/w_thread_pool.h>
#include <fiber/w_fiber.h>
#include <io/w_io.h>

#include <memory/w_string.h>

#include <shared_mutex>
#include <condition_variable>

#pragma region statics

static w_mem_pool w_init()
{
	w_mem_pool _mem_pool = nullptr;

	W_RESULT _ret = wolf_init();
	REQUIRE(_ret == W_SUCCESS);

	_ret = w_mem_pool_init(&_mem_pool);
	REQUIRE(_ret == W_SUCCESS);

	return _mem_pool;
}

static void w_fini(w_mem_pool* pMemPool)
{
	w_mem_pool_fini(pMemPool);
	wolf_fini();
}

#pragma endregion

#pragma region chrono

TEST_CASE("w_chrono")
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

TEST_CASE("w_gametime")
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

TEST_CASE("w_timespan")
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

#pragma endregion

#pragma region compression

TEST_CASE("w_compress")
{
	w_mem_pool _mem_pool = w_init();

	const char* _buffer = "xxxxxxxxxxxxxxxxxxxxthis_is_playpod_xyz_123_789_XXXYZxxxxxxxxxxxxxxxxxxxx";
	size_t _buffer_size = strlen(_buffer);

	w_compress_result _compress_lz4 = { 0 };
	_compress_lz4.size_in = _buffer_size;
	W_RESULT _ret = w_compress_lz4(_buffer, W_FAST, 1, &_compress_lz4);
	REQUIRE(_ret == W_SUCCESS);

	w_compress_result _decompress_lz4 = { 0 };
	_decompress_lz4.size_in = _compress_lz4.size_out;
	if (_compress_lz4.data)
	{
		_ret = w_decompress_lz4(_compress_lz4.data, &_decompress_lz4);
		REQUIRE(_ret == W_SUCCESS);
	}

	REQUIRE(_compress_lz4.size_in == _decompress_lz4.size_out);

#ifdef W_PLATFORM_WIN

	w_compress_result _compress_lzma = { 0 };
	_compress_lzma.size_in = _buffer_size;
	_ret = w_compress_lzma((const uint8_t*)_buffer, &_compress_lzma);
	REQUIRE(_ret == W_SUCCESS);

	w_compress_result _decompress_lzma = { 0 };
	_decompress_lzma.size_in = _compress_lzma.size_out;
	if (_compress_lzma.data)
	{
		_ret = w_decompress_lzma((const uint8_t*)_compress_lzma.data, &_decompress_lzma);
		REQUIRE(_ret == W_SUCCESS);
	}

	REQUIRE(_compress_lzma.size_in == _decompress_lzma.size_out);

#endif

	//command
	w_buffer _msg_buffer = nullptr;
	w_msgpack _msg_pack = nullptr;
	_ret = w_compress_msgpack_init(_mem_pool, &_msg_pack);
	REQUIRE(_ret == W_SUCCESS);

	//pack boolean
	_ret = w_compress_msgpack_append_boolean(_msg_pack, true);
	REQUIRE(_ret == W_SUCCESS);

	//pack boolean
	_ret = w_compress_msgpack_append_boolean(_msg_pack, false);
	REQUIRE(_ret == W_SUCCESS);

	//pack int
	_ret = w_compress_msgpack_append_int(_msg_pack, 7);
	REQUIRE(_ret == W_SUCCESS);

	//pack float
	_ret = w_compress_msgpack_append_float(_msg_pack, (float)17.0);
	REQUIRE(_ret == W_SUCCESS);

	//pack double
	_ret = w_compress_msgpack_append_double(_msg_pack, 77.0);
	REQUIRE(_ret == W_SUCCESS);

	//pack string
	const char* _str = "this is test";
	_ret = w_compress_msgpack_append_string(_msg_pack, _str, strlen(_str));
	REQUIRE(_ret == W_SUCCESS);

	//pack array
	int _a[3] = { 1,2,3 };
	w_array _array = w_array_init(_mem_pool, 3, sizeof(const int*));
	w_array_append(_array, &_a[0]);
	w_array_append(_array, &_a[1]);
	w_array_append(_array, &_a[2]);

	_ret = w_compress_msgpack_append_array(
		_msg_pack,
		_array,
		w_std_types::W_TYPE_POSITIVE_INT);
	REQUIRE(_ret == W_SUCCESS);

	//the result will be availabe in _msg_buffer
	_ret = w_compress_msgpack_fini(_mem_pool, _msg_pack, &_msg_buffer);
	REQUIRE(_ret == W_SUCCESS);

	//let's decompress _msg_buffer
	_ret = w_decompress_msgpack(_mem_pool, _msg_buffer, 
		[](w_std_types pDataType, const void* pValue)
		{
			switch (pDataType)
			{
			default:
			{
				std::cout << "undefined data type" << std::endl;
				break;
			}
			case w_std_types::W_TYPE_NULL:
			{
				std::cout << "all data have been unpacked" << std::endl;
				break;
			}
			case w_std_types::W_TYPE_BOOLEAN:
			{
				auto _value = (const bool*)pValue;
				if (*_value)
				{
					std::cout << "type is W_TYPE_BOOLEAN and value is TRUE" << std::endl;
				}
				else
				{
					std::cout << "type is W_TYPE_BOOLEAN and value is FALSE" << std::endl;
				}
				break;
			}
			case w_std_types::W_TYPE_POSITIVE_INT:
			case w_std_types::W_TYPE_NEGATIVE_INT:
			{
				auto _value = (const int*)pValue;
				if (*_value)
				{
					if (*_value >= 0)
					{
						std::cout <<
							"type is W_TYPE_POSITIVE_INT and value is " <<
							*_value <<
							std::endl;
					}
					else
					{
						std::cout <<
							"type is W_TYPE_NEGATIVE_INT and value is " <<
							*_value <<
							std::endl;
					}
				}
				break;
			}
			case w_std_types::W_TYPE_FLOAT:
			{
				auto _value = (const float*)pValue;
				if (*_value)
				{
					std::cout <<
						"type is W_TYPE_FLOAT and value is " <<
						*_value <<
						std::endl;
				}
				break;
			}
			case w_std_types::W_TYPE_DOUBLE:
			{
				auto _value = (const double*)pValue;
				if (*_value)
				{
					std::cout <<
						"type is W_TYPE_DOUBLE and value is " <<
						*_value <<
						std::endl;
				}
				break;
			}
			case w_std_types::W_TYPE_STRING:
			{
				auto _value = (w_string)pValue;
				if (_value)
				{
					std::cout <<
						"type is W_TYPE_STRING and value is " <<
						_value->data <<
						std::endl;
				}
				break;
			}
			case w_std_types::W_TYPE_ARRAY_BEGIN:
			{
				std::cout << "Begin of ARRAY " << std::endl;
				break;
			}
			case w_std_types::W_TYPE_ARRAY_END:
			{
				std::cout << "End of ARRAY " << std::endl;
				break;
			}
			}
		});
	REQUIRE(_ret == W_SUCCESS);

	w_fini(&_mem_pool);
}

#pragma endregion

#pragma region concurrency

TEST_CASE("concurrency/w_atomic")
{
	W_ATOMIC_INT _atomic_int_mem = 0;
	w_atomic_set(&_atomic_int_mem, 123);

	w_atomic_inc(&_atomic_int_mem);
	REQUIRE(w_atomic_read(&_atomic_int_mem) == 124);

	w_atomic_dec(&_atomic_int_mem);
	REQUIRE(w_atomic_read(&_atomic_int_mem) == 123);
}

TEST_CASE("concurrency/w_concurrent_queue")
{
	w_mem_pool _mem_pool = w_init();

	int _data[] = { 1, 2, 3 };
	w_concurrent_queue _queue = nullptr;

	REQUIRE(w_concurrent_queue_init(_mem_pool, &_queue, 5) == W_SUCCESS);
	REQUIRE(w_concurrent_queue_push(_queue, (void*)&_data[0]) == W_SUCCESS);
	REQUIRE(w_concurrent_queue_push(_queue, (void*)&_data[1]) == W_SUCCESS);
	REQUIRE(w_concurrent_queue_trypush(_queue, (void*)&_data[2]) == W_SUCCESS);

	int _c = 0;
	int* _c_ptr = &_c;
	REQUIRE(w_concurrent_queue_size(_queue) == 3);
	REQUIRE(w_concurrent_queue_size_threadsafe(_queue) == 3);
	REQUIRE(w_concurrent_queue_interrupt_all(_queue) == W_SUCCESS);

	void* _pop_data = NULL;
	std::cout << "pop from queue" << std::endl;
	while (w_concurrent_queue_size_threadsafe(_queue))
	{
		if (w_concurrent_queue_pop(_queue, &_pop_data) == W_SUCCESS)
		{
			int* _int = (int*)_pop_data;
			if (_int)
			{
				std::cout << *_int << std::endl;
			}
		}
	}

	REQUIRE(w_concurrent_queue_fini(_queue) == W_SUCCESS);

	w_fini(&_mem_pool);
}

TEST_CASE("concurrency/w_condition_variable")
{
	w_mem_pool _mem_pool = w_init();

	w_condition_variable _cv = nullptr;

	REQUIRE(w_condition_variable_init(_mem_pool, &_cv) == W_SUCCESS);
	
	w_thread _t1 = nullptr;
	W_RESULT _ret = w_thread_init(
		_mem_pool,
		&_t1,
		[](w_thread pThread, void* pArg) -> void*
		{
			w_condition_variable _cv = (w_condition_variable)pArg;
			if (_cv)
			{
				std::cout << "condition variable is going to wait" << std::endl;
				REQUIRE(w_condition_variable_wait(_cv) == W_SUCCESS);
				std::cout << "condition variable just got signal" << std::endl;
			}
			return NULL;
		}, _cv);
	REQUIRE(_ret == W_SUCCESS);

	w_thread _t2 = nullptr;
	_ret = w_thread_init(
		_mem_pool,
		&_t2,
		[](w_thread pThread, void* pArg) -> void*
		{
			w_condition_variable _cv = (w_condition_variable)pArg;
			if (_cv)
			{
				w_thread_current_sleep_for_seconds(1);
				std::cout << "condition variable is going to send signal after two seconds" << std::endl;
				w_thread_current_sleep_for_seconds(2);
				REQUIRE(w_condition_variable_signal(_cv) == W_SUCCESS);
			}
			return NULL;
		}, _cv);
	REQUIRE(_ret == W_SUCCESS);

	w_thread_join(_t1);
	w_thread_join(_t2);
	
	w_mem_pool_fini(&_mem_pool);
	REQUIRE(_mem_pool == nullptr);

	w_fini(&_mem_pool);
}

TEST_CASE("concurrency/w_mutex")
{
	w_mem_pool _mem_pool = w_init();

	w_mutex _mutex = nullptr;

	REQUIRE(w_mutex_init(_mem_pool, &_mutex, 0) == W_SUCCESS);
	REQUIRE(w_mutex_lock(_mutex) == W_SUCCESS);
	REQUIRE(w_mutex_unlock(_mutex) == W_SUCCESS);
	REQUIRE(w_mutex_trylock(_mutex) == W_SUCCESS);
	REQUIRE(w_mutex_unlock(_mutex) == W_SUCCESS);
	REQUIRE(w_mutex_fini(&_mutex) == W_SUCCESS);

	w_fini(&_mem_pool);
}

TEST_CASE("concurrency/w_thread")
{
	w_mem_pool _mem_pool = w_init();

	w_thread _t1 = nullptr;
	W_RESULT _ret = w_thread_init(
		_mem_pool,
		&_t1,
		[](w_thread pThread, void* pArg)->void*
		{
			std::cout << "thread: t1 is going to wait for 4 seconds" << std::endl;
			w_thread_current_sleep_for_seconds(1);
			w_thread_current_sleep_for_nanoseconds(1.0 * 1000.0 * 1000.0 * 1000.0);
			w_thread_current_sleep_for_microseconds(1.0 * 1000.0 * 1000.0);
			w_thread_current_sleep_for_milliseconds(1.0 * 1000.0);
			std::cout << "thread: t1 finished" << std::endl;

			return W_SUCCESS;
		},
		NULL);
	REQUIRE(_ret == W_SUCCESS);
	REQUIRE(w_thread_join(_t1) == W_SUCCESS);

	w_thread _t2 = w_thread_get_current();
	REQUIRE(_t2 == NULL);

	w_thread_once_flag _once_flag = NULL;
	REQUIRE(w_thread_init_once_flag(_mem_pool, &_once_flag) == W_SUCCESS);

	_ret = w_thread_once_call(_once_flag, []()
		{
			std::cout << "thread once called" << std::endl;
		});
	REQUIRE(_ret == W_SUCCESS);
	
	w_fini(&_mem_pool);
}

TEST_CASE("concurrency/w_thread_pool")
{
	w_mem_pool _mem_pool = w_init();

	int _owners[3] = { 1, 2, 3 };

	w_thread_pool _thread_pool = nullptr;
	size_t _min_threads = 1;
	size_t _max_threads = 4;
	REQUIRE(w_thread_pool_init(_mem_pool, &_thread_pool, _min_threads, _max_threads) == W_SUCCESS);

	//run task 2 after 2 seconds
	int64_t  _start_after_microsecs = 2 * 1000 * 1000;
	W_RESULT _hr = w_thread_pool_schedule(_thread_pool,
		[](w_thread pThread, void* pArg) -> void*
		{
			std::cout << "task 1 just launched after 5 seconds" << std::endl;
			return W_SUCCESS;
		}, nullptr, _start_after_microsecs, &_owners[0]);
	REQUIRE(_hr == W_SUCCESS);

	//run task 2 after 10 seconds, but it won't run because task 3 will be cancel it immediately
	 _start_after_microsecs = 10 * 1000 * 1000;
	_hr = w_thread_pool_schedule(_thread_pool,
		[](w_thread pThread, void* pArg) -> void*
		{
			std::cout << "task 2 just launched after 2 seconds" << std::endl;
			return nullptr;
		}, nullptr, _start_after_microsecs, & _owners[1]);
	REQUIRE(_hr == W_SUCCESS);

	//we will send pointers of thread pool and owner to task 3
	struct t_args
	{
		w_thread_pool thread_pool;
		void* owner;
	} _arg;
	_arg.thread_pool = _thread_pool;//thread pool
	_arg.owner = (void*)&_owners[1];//owner of task 2

	//run task 3 immediately
	_hr = w_thread_pool_schedule(
		_thread_pool,
		[](w_thread pThread, void* pArg) -> void*
		{
			std::cout << "task 3 just launched immediately" << std::endl;

			auto _arg = (t_args*)pArg;
			if (_arg)
			{
				if (w_thread_pool_tasks_cancel(_arg->thread_pool, _arg->owner) == W_SUCCESS)
				{
					std::cout << "task 3 just canceled task 2" << std::endl;
				}
				else
				{
					std::cout << "task 3 could not cancele task 2" << std::endl;
				}
			}
			return nullptr;
		}, &_arg, 0, & _owners[2]);
	REQUIRE(_hr == W_SUCCESS);

	std::cout <<
		"tasks have been scheduled: " << 
		w_thread_pool_scheduled_tasks_count(_thread_pool) <<
		std::endl;

	std::cout <<
		"tasks are waiting: " << 
		w_thread_pool_tasks_count(_thread_pool) <<
		std::endl;

	size_t _running_tasks = 0;
	do
	{
		w_thread_current_sleep_for_milliseconds(16);
		_running_tasks = w_thread_pool_tasks_run_count(_thread_pool);
	} while (_running_tasks != 2);

	//wait for all threads
	REQUIRE(w_thread_pool_fini(&_thread_pool) == W_SUCCESS);

	w_fini(&_mem_pool);
}

//TEST_CASE("log/w_log")
//{
//    w_log_config pConfig;
//    pConfig.app_name = "playpod";
//    pConfig.flush_level = W_LOG_INFO;
//    char* _io_dir_get_current = (char*)w_malloc(_mem_pool, sizeof(char));
//    w_io_dir_get_current(_mem_pool, &_io_dir_get_current);
//    pConfig.log_dir_path = _io_dir_get_current;
//    int _log_init = w_log_init(_mem_pool, &pConfig);
//    REQUIRE(_log_init == 0);
//
//    char log[] = "error occured";
//    LOG(W_LOG_ERROR, log);
//    LOG_EX(W_LOG_ERROR, 0, "error occured log_ex");
//    LOG_P(W_LOG_ERROR, "error occured log_p");
//    LOG_P_EX(W_LOG_ERROR, 0, "error occured log_p_ex");
//    V(1, W_LOG_ERROR, "error occured v");
//    VA(1, W_LOG_ERROR, "error occured va", "extra");
//    VALIDATE(1, W_LOG_ERROR, false, "error occured validate");
//    VALIDATE_EX(1, 0, W_LOG_ERROR, false, "error occured validate_ex");
//    W_RESULT _log_flush = w_log_flush();
//    REQUIRE(_log_flush == 0);
//
//    W_RESULT log_flush_ex = w_log_flush_ex(0);
//    REQUIRE(log_flush_ex == 0);
//
//    W_RESULT _log_fini = w_log_fini();
//    REQUIRE(_log_init == 0);
//
//
//}
//
//TEST_CASE("memory/w_array")
//{
//
//    int pInitSize = 36;
//    int pSizeOfEachElement = sizeof(int);
//    w_array array = w_array_init(_mem_pool, pInitSize, pSizeOfEachElement);
//    REQUIRE(array != 0);
//
//    int u = 1;
//    void* _array_append = w_array_append(array, (void*)&u);
//    REQUIRE(_array_append != 0x0000000000000000);
//
//    int pElementIndex = 0;
//    const void* _array_get_element = w_array_get_element(array, pElementIndex);
//    REQUIRE(_array_get_element != 0x0000000000000000);
//
//
//    void* _array_pop = w_array_pop(array);
//    REQUIRE(_array_pop != 0);
//
//    /* void* _array_remove = w_array_remove(array);
//     REQUIRE(_array_remove != 0x0000000000000000);*/
//
//    int _array_is_empty = w_array_is_empty(array);
//    REQUIRE(_array_is_empty == 1);
//}
//TEST_CASE("memory/w_hash")
//{
//    w_hash _hash_init = w_hash_init(_mem_pool);
//    REQUIRE(_hash_init != 0);
//
//    w_hash _hash_init_2 = w_hash_init(_mem_pool);
//    REQUIRE(_hash_init_2 != 0);
//
//    w_hash_custom_fn tmp = (w_hash_custom_fn)&pHashCustomFunc_tmp;
//    w_hash _hash_make_custom = w_hash_make_custom(_mem_pool, tmp);
//
//    w_hash _hash_make_custom2 = w_hash_make_custom(_mem_pool, tmp);
//
//    char pKey_1[] = "name";
//    size_t pKeyLen = strlen(pKey_1);
//    char pValue_1[] = "wolf";
//
//    char pKey_2[] = "family";
//    size_t pKeyLen_2 = strlen(pKey_2);
//    char pValue_2[] = "engine";
//
//    w_hash_set(_hash_make_custom, pKey_2, pKeyLen_2, pValue_2);
//    w_hash_set(_hash_make_custom2, pKey_2, pKeyLen_2, pValue_2);
//    w_hash_set(_hash_init, pKey_1, pKeyLen, pValue_1);
//    w_hash_set(_hash_init, pKey_2, pKeyLen_2, pValue_2);
//    w_hash_set(_hash_init_2, pKey_2, pKeyLen_2, pValue_2);
//    w_hash _hash_overlay = w_hash_overlay(_mem_pool, _hash_make_custom, _hash_make_custom2);
//
//    uint32_t _hash_size = w_hash_size(_hash_init);
//    REQUIRE(_hash_size == 2);
//
//    void* hash_get = w_hash_get(_hash_init, pKey_1, pKeyLen);
//    REQUIRE(hash_get != 0);
//
//    w_hash _hash_clone = w_hash_clone(_mem_pool, _hash_init);
//    REQUIRE(_hash_clone != 0);
//
//    w_hash_merger pHashMergerFunction = &_hash_func;
//    int  pCustomUserData = 12;
//
//
//    w_hash hash_merge = w_hash_merge(_mem_pool, _hash_init, _hash_init_2, pHashMergerFunction, &pCustomUserData);
//    w_hash_index _hash_first = w_hash_first(_mem_pool, _hash_init);
//    REQUIRE(_hash_first != 0);
//
//    w_hash_index hash_next = w_hash_next(_hash_first);
//    REQUIRE(hash_next != 0);
//
//    size_t pKeyLen_hash;
//    char* pValue_hash;
//    char* pKey_hash;
//    w_hash_this(_hash_first, (const void**)&pKey_hash, &pKeyLen_hash, (void**)&pValue_hash);
//    const void* hash_this_key = w_hash_this_key(_hash_first);
//    const char* _key_hash = "family";
//    char* key = (char*)hash_this_key;
//    int  result_key_hash = strcmp(key, _key_hash);
//    REQUIRE(result_key_hash == 0);
//
//    void* hash_this_val = w_hash_this_val(_hash_first);
//    const char* _val_hash = "engine";
//    char* val = (char*)hash_this_val;
//    int  result_val_hash = strcmp(val, _val_hash);
//    REQUIRE(result_val_hash == 0);
//
//    int pRec = 1;
//    w_hash_do_callback_fn* pHashCallbackDo = hash_do_callback_fn;
//    int _hash_do = w_hash_do(pHashCallbackDo, &pRec, _hash_init);
//    REQUIRE(_hash_do != 0);
//
//
//    size_t _hash_this_key_len = w_hash_this_key_len(_hash_first);
//    REQUIRE(_hash_this_key_len == 6);
//
//    w_apr_pool _hash_get_mem_pool = w_hash_get_mem_pool(_hash_init);
//    REQUIRE(_hash_get_mem_pool != 0);
//}
//TEST_CASE("memory/w_string")
//{
//
//    w_string pStringView = NULL;
//    w_string pDst = NULL;
//    const char* pData = "pod";
//    W_RESULT _string_init = w_string_init(_mem_pool, &pStringView, pData);
//    REQUIRE(_string_init == 0);
//
//    W_RESULT _string_dup = w_string_dup(_mem_pool, &pDst, pStringView);
//    REQUIRE(_string_dup == 0);
//
//    W_RESULT _string_is_empty = w_string_is_empty(pStringView);
//    REQUIRE(_string_is_empty == 1);
//
//    w_string_to_lower(pStringView);
//
//    W_RESULT _string_clear = w_string_clear(&pStringView);
//    REQUIRE(_string_clear == 0);
//
//
//
//}
//
//TEST_CASE("memory/w_table")
//{
//    w_table  pTable = (w_table)w_malloc(_mem_pool, sizeof(w_table));
//    W_RESULT   _table_init = w_table_init(_mem_pool, &pTable, 10);
//    REQUIRE(_table_init == 0);
//
//    char pKey[] = "name";
//    char pValue[] = "wolf";
//    W_RESULT  table_set = w_table_set(pTable, pKey, pValue, true);
//    REQUIRE(table_set == 0);
//
//    /* W_RESULT table_unset = w_table_unset(pTable, pKey);
//     REQUIRE(table_unset == 0);*/
//
//    W_RESULT  table_add = w_table_add(pTable, pKey, pValue);
//    REQUIRE(table_add == 0);
//
//    int   _get_size = w_table_get_size(pTable);
//    REQUIRE(_get_size == 2);
//
//    int _table_empty = w_table_is_empty(pTable);
//    REQUIRE(_table_empty == 0);
//
//    char pArg[] = "data";
//    w_table_do_callback* myfunc = pCallBack;
//    int _table_do = w_table_do(pTable, pCallBack, (void*)pArg);
//    REQUIRE(_table_do == 1);
//
//
//    va_list pKeys = (va_list)"name";
//    int _table_do_with_filter = w_table_do_with_filter(pTable, pCallBack, (void*)pArg, pKeys);
//    REQUIRE(_table_do_with_filter == 1);
//
//
//    const w_table_entry_iterator  _table_entry_iterator = w_table_get_entry(pTable);
//    REQUIRE(_table_entry_iterator != 0);
//    /*if befor this function you call w_table_unset() the result will be null */
//    char* _table_get_key = w_table_get_key(_table_entry_iterator, 1);
//    const char* _key = "name";
//    int  result_key = strcmp(_table_get_key, _key);
//    REQUIRE(result_key == 0);
//
//    const char* table_get_value = w_table_get_value(_table_entry_iterator, 1);
//    const char _value[] = "wolf";
//    int  result_value = strcmp(table_get_value, _value);
//    REQUIRE(result_value == 0);
//
//    size_t _table_get_key_checksum = w_table_get_key_checksum(_table_entry_iterator, 1);
//    REQUIRE(_table_get_key_checksum != 0);
//
//}
//TEST_CASE("os/w_process")
//{
//    const unsigned long pProcessID = 1212;
//    wchar_t* pProcessName = NULL;
//    wchar_t* pProcessLists_w = NULL;
//    char* pProcessLists = NULL;
//    W_RESULT _process_get_is_running = w_process_get_is_running(pProcessID);
//    W_RESULT _process_get_name_by_id = w_process_get_name_by_id(_mem_pool, pProcessID, &pProcessName);
//
//    //  size_t _process_get_count_of_instances = w_process_get_count_of_instances(pProcessName);
//    W_RESULT _process_print_allW = w_process_print_allW(_mem_pool, &pProcessLists_w);
//    REQUIRE(_process_print_allW == 0);
//
//    W_RESULT process_print_all = w_process_print_all(_mem_pool, &pProcessLists);
//    REQUIRE(process_print_all == 0);
//
//    const wchar_t* pPathToProcess = L"C:\\WINDOWS\\system32\\cmd.exe";
//    w_process_info pProcessInfo = (w_process_info)w_malloc(_mem_pool, sizeof(w_process_info));
//    pProcessInfo->process_name = pProcessName;
//
//    W_RESULT _process_create = w_process_create(_mem_pool, pPathToProcess, NULL, NULL, 0, 0, &pProcessInfo);
//    REQUIRE(_process_create == 0);
//
//    W_RESULT _process_kill_by_info = w_process_kill_by_info(pProcessInfo);
//    W_RESULT _process_kill_by_name = w_process_kill_by_name(pProcessName, false);
//    const unsigned long pProcessID2 = 1264;
//    W_RESULT _process_kill_by_id = w_process_kill_by_id(pProcessID2, false);
//    W_RESULT _process_kill_all = w_process_kill_all(_mem_pool, L"vcpkgsrv.exe", NULL);
//    W_RESULT _process_kill_by_id_handle = w_process_kill_by_id_handle(pProcessID);
//
//}
//
////TEST_CASE("script/w_lua")
////{
////    W_RESULT _lua_init = w_lua_init(_mem_pool);
////
////    char* _io_dir_get_current = (char*)w_malloc(_mem_pool, sizeof(char));
////    w_io_dir_get_current(_mem_pool, &_io_dir_get_current);
////    char _filename[] = "script.lua";
////    char _sign[] = "\\";
////    char* _path = w_strcat(_mem_pool, _io_dir_get_current, _sign, _filename, NULL);
////
////    /* W_RESULT _lua_load_stream= w_lua_load_from_stream(luacode);
////    W_RESULT _lua_run2 = w_lua_run();*/
////
////    W_RESULT _lua_load_file1 = w_lua_load_file(_path);
////
////    W_RESULT _lua_run6 = w_lua_run();
////
////    w_lua_prepare_function("tellme");
////
////    W_RESULT _lua_run2 = w_lua_run();
////
////    w_lua_prepare_function("square");
////
////    double value_1 = 6.1;
////    double value_2 = 1.1;
////    int pValueType = 3;
////    lua_State* _lua_get_state = w_lua_get_state();
////
////    W_RESULT _lua_set_parameter_function = w_lua_set_parameter_function((void*)&value_1, pValueType);
////
////    void* value = 0;
////    void* value_n = 0;
////    void* value_6 = 0;
////
////    W_RESULT lua_execute_function_with_three_results = w_lua_execute_function_with_three_results(_mem_pool, &value, &value_n, &value_6);
////    cout << "The return value of the function was " << lua_tonumber(_lua_get_state, -3) << endl;
////
////    cout << "The return value of the function was " << lua_tonumber(_lua_get_state, -2) << endl;
////    cout << "The return value of the function was " << lua_tonumber(_lua_get_state, -1) << endl;
////    W_RESULT lua_execute_function_with_two_results = w_lua_execute_function_with_two_results(_mem_pool, &value, &value_n);
////    cout << "The return value of the function was " << lua_tonumber(_lua_get_state, -2) << endl;
////    cout << "The return value of the function was " << lua_tonumber(_lua_get_state, -1) << endl;
////    W_RESULT lua_execute_function_with_one_result = w_lua_execute_function_with_one_result(_mem_pool, &value);
////
////    cout << "The return value of the function was " << lua_tonumber(_lua_get_state, -1) << endl;
////    /*w_lua_bind_to_cfunction(&displayLuaFunction, "displayLuaFunction");
////      w_lua_prepare_function("displayLuaFunction");
////      w_lua_execute_void_function();
////     W_RESULT _lua_run1 = w_lua_run();*/
////
////    const char* _lua_get_last_error = w_lua_get_last_error();
////    W_RESULT _lua_set_lua_path = w_lua_set_lua_path(_mem_pool, _path);
////    W_RESULT _lua_fini = w_lua_fini();
////
////
////}
//
//TEST_CASE("memory/w_mem_map")
//{
//    const char content[] = "hi";
//    char* _io_dir_get_current = (char*)w_malloc(_mem_pool, sizeof(char));
//    w_io_dir_get_current(_mem_pool, &_io_dir_get_current);
//    char _filename[] = "file.txt";
//    char _sign[] = "\\";
//    w_mem_pool _mem_pool2 = NULL;
//    int x = w_mem_pool_init(&_mem_pool2);
//    const char* _path = w_strcat(_mem_pool, _io_dir_get_current, _sign, _filename, NULL);
//    w_file _file = w_io_file_create(_mem_pool, _path, content, false, false, false, false, false, false, false);
//    w_mem_map pNewMemMap = (w_mem_map)w_malloc(_mem_pool2, sizeof(w_mem_map));
//    w_mem_map pNewMemMap_new = (w_mem_map)w_malloc(_mem_pool2, sizeof(w_mem_map));
//
//    w_offset flag = 1;
//    W_RESULT  _mem_map_creat = w_mem_map_create(_mem_pool2, &pNewMemMap, _file, 0, 1, 0);
//    REQUIRE(_mem_map_creat == 0);
//
//    void* pAddress = 0;
//    W_RESULT   _mem_map_offset = w_mem_map_offset(&pAddress, pNewMemMap, 1);
//    REQUIRE(_mem_map_offset == 0);
//
//    W_RESULT  _mem_map_dup = w_mem_map_dup(_mem_pool2, &pNewMemMap_new, pNewMemMap);
//    REQUIRE(_mem_map_dup == 0);
//
//    W_RESULT   _mem_map_delete = w_mem_map_delete(pNewMemMap_new);
//    REQUIRE(_mem_map_delete == 0);
//
//
//}
//TEST_CASE("memory/w_shared_mem")
//{
//    const char content[] = "hi";
//    char* _io_dir_get_current = (char*)w_malloc(_mem_pool, sizeof(char));
//    w_io_dir_get_current(_mem_pool, &_io_dir_get_current);
//    char _filename[] = "file.txt";
//    char _sign[] = "\\";
//    w_mem_pool _mem_pool2 = NULL;
//    int x = w_mem_pool_init(&_mem_pool2);
//    char* _path = w_strcat(_mem_pool, _io_dir_get_current, _sign, _filename, NULL);
//    w_file _file = w_io_file_create(_mem_pool, _path, content, false, false, false, false, false, false, false);
//    w_shared_mem pSharedMem = (w_shared_mem)w_malloc(_mem_pool, sizeof(w_shared_mem));
//    size_t pReqSize = 10;
//
//    W_RESULT   _shared_mem_create = w_shared_mem_create(_mem_pool, &pSharedMem, pReqSize, _path);
//    REQUIRE(_shared_mem_create == 0);
//
//    size_t  _shared_mem_get_size = w_shared_mem_get_size(pSharedMem);
//    REQUIRE(_shared_mem_get_size == 10);
//
//    W_RESULT _shared_mem_create_ex = w_shared_mem_create_ex(_mem_pool, &pSharedMem, pReqSize, _path, 1);
//    REQUIRE(_shared_mem_create_ex == 0);
//
//    w_apr_pool _shared_mem_get_mem_pool = w_shared_mem_get_mem_pool(pSharedMem);
//    REQUIRE(_shared_mem_get_mem_pool != 0);
//
//    void* _shared_mem_get_base_addr = w_shared_mem_get_base_addr(pSharedMem);
//    REQUIRE(_shared_mem_get_base_addr != 0);
//
//    W_RESULT   _shared_mem_remove = w_shared_mem_remove(_mem_pool, _path);
//    REQUIRE(_shared_mem_remove == 1);
//
//    /* W_RESULT shared_mem_delete = w_shared_mem_delete(pSharedMem);
//     REQUIRE(shared_mem_delete == 0);*/
//
//    W_RESULT  _shared_mem_destroy = w_shared_mem_destroy(pSharedMem);
//    REQUIRE(_shared_mem_destroy == 0);
//
//    W_RESULT _shared_mem_attach = w_shared_mem_attach(_mem_pool, &pSharedMem, _path);
//    REQUIRE(_shared_mem_attach == 0);
//
//    W_RESULT _shared_mem_attach_ex = w_shared_mem_attach_ex(_mem_pool, &pSharedMem, _path, 1);
//    REQUIRE(_shared_mem_attach_ex == 0);
//
//
//    W_RESULT  _shared_mem_detach = w_shared_mem_detach(pSharedMem);
//    REQUIRE(_shared_mem_detach == 0);
//
//
//
//}

#pragma endregion

#pragma region fiber

TEST_CASE("fiber/w_fiber")
{
	w_mem_pool _mem_pool = w_init();

	w_fiber _fiber = nullptr;
	auto _ret = w_fiber_init(
		_mem_pool,
		&_fiber,
		[](void* pArgs)
		{
			std::cout << "fiber id: just started" << std::endl;
		}, nullptr);
	REQUIRE(_ret == W_SUCCESS);
	REQUIRE(_fiber);

	REQUIRE(w_fiber_is_joinable(_fiber) == W_SUCCESS);
	REQUIRE(w_fiber_join(_fiber) == W_SUCCESS);
	REQUIRE(w_fiber_fini(&_fiber) == W_SUCCESS);

	w_fini(&_mem_pool);
}

#pragma endregion

#pragma region io

TEST_CASE("io/w_io")
{
	w_mem_pool _mem_pool = w_init();

    char* _io_dir_get_current = nullptr;
	REQUIRE(w_io_dir_get_current(_mem_pool, &_io_dir_get_current) == W_SUCCESS);
	std::cout << "current directory is " << _io_dir_get_current << std::endl;

	char* _io_dir_get_current_exe = nullptr;
	REQUIRE(w_io_dir_get_current_exe(_mem_pool, &_io_dir_get_current_exe) == W_SUCCESS);
	std::cout << "current directory is " << _io_dir_get_current_exe << std::endl;

	const char* _content = "Hello from Wolf Engine";
	char* _path = w_strcat(_mem_pool, _io_dir_get_current, "//file.txt", NULL);
	w_file _file = w_io_file_create(
		_mem_pool, 
		_path, 
		_content, 
		false, 
		false, 
		false, 
		false, 
		false, 
		false, 
		false);
	REQUIRE(_file != NULL);

	auto _file_stream = w_io_file_read_full_from_path(_mem_pool, _path);
	REQUIRE(strcmp(_file_stream->buffer, _content) == 0);

    REQUIRE(
		w_io_file_save(
			_path, 
			_content, 
			false, 
			false, 
			false, 
			false, 
			false, 
			false, 
			false) == W_SUCCESS);
	_file_stream = w_io_file_read_full_from_path(_mem_pool, _path);
	REQUIRE(strcmp(_file_stream->buffer, _content) == 0);




    //const char* _basename_1 = w_io_file_get_basename_from_path(_mem_pool, _path);
    //const char* _basename_from_path = "file.txt";
    //int result__basename_1 = strcmp(_basename_from_path, _basename_1);
    //REQUIRE(result__basename_1 == 0);

    //const char* _basename_2 = w_io_file_get_basename(_mem_pool, _file);
    //const char* _basename_from_path2 = "file.txt";
    //int result__basename_2 = strcmp(_basename_from_path2, _basename_2);
    //REQUIRE(result__basename_2 == 0);

    //const char* _name_1 = w_io_file_get_basename_without_extension_from_path(_mem_pool, _path);
    //const char* _without_extension_from_path1 = "file";
    //int result___name_1 = strcmp(_without_extension_from_path1, _name_1);
    //REQUIRE(result___name_1 == 0);

    //const char* _name_2 = w_io_file_get_basename_without_extension(_mem_pool, _file);
    //const char* _without_extension = "file";
    //int result___name_2 = strcmp(_without_extension, _name_2);
    //REQUIRE(result___name_2 == 0);

    //char _filename2[] = "file";
    //char* _path2 = w_strcat(_mem_pool, _io_dir_get_current, _sign, _filename2, NULL);
    //const char* _filename_1 = w_io_file_get_name_from_path(_mem_pool, _path);
    //int result__filename_1 = strcmp(_path2, _filename_1);
    //REQUIRE(result__filename_1 == 0);


    //const char* _filename_2 = w_io_file_get_name(_mem_pool, _file);
    //int result__filename_2 = strcmp(_path2, _filename_2);
    //REQUIRE(result__filename_2 == 0);

    //w_file_info _io_file_get_info_from_path = w_io_file_get_info_from_path(_mem_pool, _path);
    //REQUIRE(_io_file_get_info_from_path != 0);

    //w_file_info _io_file_get_info = w_io_file_get_info(_mem_pool, _file);

    //const char* _io_file_get_extension_from_path = w_io_file_get_extension_from_path(_mem_pool, _path);
    //const char* _extention = "txt";
    //int result_extention = strcmp(_io_file_get_extension_from_path, _extention);
    //REQUIRE(result_extention == 0);

    //const char* _io_file_get_extension = w_io_file_get_extension(_mem_pool, _file);
    //const char* _extention1 = "txt";
    //int result_extention1 = strcmp(_io_file_get_extension, _extention1);
    //REQUIRE(result_extention1 == 0);

    //W_RESULT _io_file_check_is_file = w_io_file_check_is_file(_path);
    //REQUIRE(_io_file_check_is_file == 0);

    //w_file_istream _io_file_read_full_from_path = w_io_file_read_full_from_path(_mem_pool, _path);
    //REQUIRE(_io_file_read_full_from_path->size != 0);

    //w_file_istream _io_file_read_nbytes_from_path = w_io_file_read_nbytes_from_path(_mem_pool, _path, 0);
    //REQUIRE(_io_file_read_nbytes_from_path->size != 0);

    //w_file_istream _io_file_read_full = w_io_file_read_full(_mem_pool, _file);
    //REQUIRE(_io_file_read_full->size != 0);

    //w_file_istream _io_file_read_nbytes = w_io_file_read_nbytes(_mem_pool, _file, 0);
    //REQUIRE(_io_file_read_nbytes->size != 0);

    ////W_RESULT	_io_file_delete_from_path = w_io_file_delete_from_path(_mem_pool, _path);
    //////REQUIRE(_io_file_delete_from_path==0);
    //W_RESULT	io_dir_create = w_io_dir_create(_mem_pool, _path);
    //REQUIRE(io_dir_create == 0);

    //const char* io_dir_get_parent = w_io_dir_get_parent(_mem_pool, _path);
    //int  result_get_parent = strcmp(io_dir_get_parent, _io_dir_get_current);
    //REQUIRE(result_get_parent == 0);


    //W_RESULT _io_dir_check_is_directory = w_io_dir_check_is_dir(_mem_pool, _io_dir_get_current);
    //REQUIRE(_io_dir_check_is_directory == 0);

    //char in[] = "playpod";
    //size_t inbytes = 7;
    //uint16_t out_utf8[12];
    //size_t outwords;
    //W_RESULT  _io_utf8_to_ucs2 = w_io_utf8_to_ucs2(in, &inbytes, out_utf8, &outwords);
    //REQUIRE(_io_utf8_to_ucs2 == 0);

    //uint16_t _in = 6;
    //size_t inwords = 1;
    //char out1[12];
    //size_t outbytes;
    //W_RESULT _io_ucs2_to_utf8 = w_io_ucs2_to_utf8(&_in, &inwords, out1, &outbytes);
    //REQUIRE(_io_ucs2_to_utf8 == 0);

    //char pString4[14] = "WOLF ENGINE";
    //const char pSplit1[2] = " ";
    //w_array* pResults = (w_array*)w_malloc(_mem_pool, sizeof(w_array));
    //W_RESULT  _io_string_split = w_io_string_split(_mem_pool, pString4, pSplit1, pResults);

    //long _io_to_hex = w_io_to_hex("0ABC546");
    //REQUIRE(_io_to_hex == 11257158);

    //const char* pString2 = "HELLO";
    //const char* pEndWith2 = "H";
    //W_RESULT _io_string_has_start_with = w_io_string_has_start_with(pString2, pEndWith2);
    //REQUIRE(_io_string_has_start_with == 0);

    //const char* pString3 = "HELLO";
    //const char* pEndWith3 = "O";
    //W_RESULT _io_string_has_end_with = w_io_string_has_end_with(pString3, pEndWith3);
    //REQUIRE(_io_string_has_end_with == 1);

    //const wchar_t* pString1 = L"HELLO";
    //const wchar_t* pEndWith1 = L"H";
    //W_RESULT _io_wstring_has_start_with = w_io_wstring_has_start_with(pString1, pEndWith1);
    //REQUIRE(_io_wstring_has_start_with == 0);


    //const wchar_t* pString = L"HELLO";
    //const wchar_t* pEndWith = L"O";
    //W_RESULT _io_wstring_has_end_with = w_io_wstring_has_end_with(pString, pEndWith);
    //REQUIRE(_io_wstring_has_end_with == 1);

    //char _filename3[] = "pic.png";

    /*char* _path1 = w_strcat(_mem_pool, _io_dir_get_current, _sign, _filename3, NULL);
    if (w_io_file_check_is_file(_path1) == W_SUCCESS)
    {
        w_file_istream _istream2 = w_io_file_read_full_from_path(_mem_pool, _path1);
        W_RESULT _io_stream_is_png = w_io_stream_is_png(_istream2);
        REQUIRE(_io_stream_is_png == 0);

        W_RESULT _io_file_is_png = w_io_file_is_png(_path1);
        REQUIRE(_io_file_is_png == 0);

        int pWidth1;
        int pHeight1;
        int pNumberOfPasses1;
        uint8_t pColorType1 = 0;
        uint8_t pBitDepth1 = 0;
        uint8_t* pPixels1;
        w_file_istream _fs = w_io_file_read_full_from_path(_mem_pool, _path1);
        W_RESULT io_pixels_from_png_stream = w_io_pixels_from_png_stream(_mem_pool, _fs, RGBA_PNG, &pWidth1, &pHeight1, &pColorType1, &pBitDepth1, &pNumberOfPasses1, &pPixels1);
        REQUIRE(io_pixels_from_png_stream == 0);

        W_RESULT _io_pixels_from_png_file = w_io_pixels_from_png_file(_mem_pool, _path1, RGBA_PNG, &pWidth1, &pHeight1, &pColorType1, &pBitDepth1, &pNumberOfPasses1, &pPixels1);
        REQUIRE(_io_pixels_from_png_file == 0);
    }*/

    //char _filename4[] = "download.jpeg";
    //char* _path3 = w_strcat(_mem_pool, _io_dir_get_current, _sign, _filename4, NULL);
    //if (w_io_file_check_is_file(_path3) == W_SUCCESS)
    //{
    //    w_file_istream _istream = w_io_file_read_full_from_path(_mem_pool, _path3);
    //    char* pDestinationBuffer = (char*)w_malloc(_mem_pool, _istream->size * 2);

    //    W_RESULT _io_file_is_jpeg = w_io_file_is_jpeg(_path3);
    //    REQUIRE(_io_file_is_jpeg == 0);

    //    W_RESULT _io_stream_is_jpeg = w_io_stream_is_jpeg(_istream);
    //    REQUIRE(_io_stream_is_jpeg == 0);


    //    int pWidth;
    //    int pHeight;
    //    int pSubSample;
    //    int pColorSpace;
    //    int pNumberOfPasses;
    //    uint8_t* pPixels;
    //    W_RESULT _io_pixels_from_jpeg_stream = w_io_pixels_from_jpeg_stream(_mem_pool, (const uint8_t*)_istream->buffer, _istream->bytes_read, RGBX_JPEG, &pWidth, &pHeight, &pSubSample, &pColorSpace, &pNumberOfPasses, &pPixels);
    //    REQUIRE(_io_pixels_from_jpeg_stream == 0);

    //    W_RESULT _io_pixels_from_jpeg_file = w_io_pixels_from_jpeg_file(_mem_pool, _path3, RGB_JPEG, &pWidth, &pHeight, &pSubSample, &pColorSpace, &pNumberOfPasses, &pPixels);
    //    REQUIRE(_io_pixels_from_jpeg_file == 0);

    //    size_t _io_to_base_64 = w_io_to_base_64(&pDestinationBuffer, (char*)_istream->buffer, _istream->size, chromium);

    //}
    
	w_fini(&_mem_pool);
}

#pragma endregion

//#include <concurrency/w_atomic.h>
//#include <concurrency/w_mutex.h>
//#include "concurrency/w_thread.h"
//#include <concurrency/w_async.h>
//#include <concurrency/w_concurrent_queue.h>
//#include <concurrency/w_condition_variable.h>
//#include <concurrency/w_thread_pool.h>
//#include <memory/w_array.h>
//#include <memory/w_hash.h>
//#include <memory/w_table.h>
//#include <memory/w_mem_pool.h>
//#include <memory/w_mem_map.h>
//#include <memory/w_string.h>
//#include <memory/w_shared_mem.h>
//#include <os/w_process.h>
//#include <script/w_lua.h>
//#include <log/w_log.h>
//#include <io/w_io.h>
//#include <script/w_python.h>
//#include <db/w_redis.h>

////void mycallback(EV_P_ w_async_base* arg1, int arg2) { printf("%s", "ok"); }
//void* _hash_func(w_apr_pool pMemPool, const void* pKey, long long pLen, const void* pHash1Value, const void* pHash2Value, const void* pData)
//{
//    printf("%s\r\n", "ok");
//    return NULL;
//
//}
//
//int hash_do_callback_fn(void* pRec, const void* pKey, long long pLen, const void* pValue)
//{
//    printf("%s", "ok2");
//    return 1;
//}
//
//void* w_thread_job_my(w_thread arg1, void* arg2) {
//    printf("%s", "okthresd");
//    return NULL;
//}
//void mycallback_thread() { printf("%s", "okthresd"); }

//int pHashCustomFunc_tmp(const char*, long long*)
//{
//    printf("%s", "ok_hash_func");
//    return 0;
//}
//
//int pCallBack(void* rec, const char* pKey, const char* pValue) {
//    const char* label = (const char*)rec;
//    printf("callback[%s]: %s %s\n", label, pKey, pValue);
//    return TRUE;
//}
//int displayLuaFunction(lua_State* l)
//{
//    // number of input arguments
//    int argc = lua_gettop(l);
//
//    // print input arguments
//    std::cout << "[C++] Function called from Lua with " << argc
//        << " input arguments" << std::endl;
//    for (int i = 0; i < argc; i++)
//    {
//        std::cout << " input argument #" << argc - i << ": "
//            << lua_tostring(l, lua_gettop(l)) << std::endl;
//        lua_pop(l, 1);
//    }
//
//    // push to the stack the multiple return values
//    std::cout << "[C++] Returning some values" << std::endl;
//    double value_1 = 3.141592;
//    int pValueType = 3;
//    W_RESULT _lua_set_parameter_function = w_lua_set_parameter_function((void*)&value_1, pValueType);
//
//    const char* value_2 = "See you space cowboy";
//    int pValueType_2 = 4;
//    W_RESULT _lua_set_parameter_function2 = w_lua_set_parameter_function((void*)value_2, pValueType_2);
//
//    // number of return values
//    return 2;
//}
//
//#define W_STATUS_IS_NOTFOUND 70015
//using namespace std;
//W_RESULT init = wolf_init();
//
//w_mem_pool _mem_pool = NULL;
//int x = w_mem_pool_init(&_mem_pool);
//w_mem_pool _mem_pool_1 = NULL;
//int u = w_mem_pool_init(&_mem_pool_1);
//const char* pHost = "localhost";
//uint16_t pPort = 6379;
//uint32_t pMin = 0;
//uint32_t pMax = 1;
//uint32_t pTTL = 60;
//uint32_t pReadWriteTimeOut = 60;
////w_redis_server  pNewServerLocation;
//uint16_t pMaxServers = 10;
//uint32_t pFlags = 0;
//char* result;
//size_t pLen = NULL;
//char* result_2;
//size_t pLen_2 = NULL;
////w_redis_stats stats;
//int32_t pIncrementNumber = 1;
//uint32_t pNewValue = NULL;
//void* v = NULL;
//const char* key = NULL;
////w_redis pRedisClient;
//const char* prefix = "testredis";
//const char* prefix2 = "testredis2";
//char p[] = "21";
//char pp[] = "271";
//size_t len_p = strlen("21");
//size_t len_pp = strlen("271");

//TEST_CASE("w_redis")
//{
//    W_RESULT _redis_init = w_redis_init(_mem_pool, pMaxServers, pFlags, &pRedisClient);
//    REQUIRE(_redis_init == 0);
//
//    W_RESULT _redis_server_init = w_redis_server_init(_mem_pool, _mem_pool_1, pHost, pPort, pMin, pMax, pMax, pTTL, pReadWriteTimeOut, &pNewServerLocation);
//    REQUIRE(_redis_server_init == 0);
//
//    W_RESULT _redis_add_server = w_redis_add_server(pRedisClient, pNewServerLocation);
//    REQUIRE(_redis_add_server == 0);
//
//    W_RESULT _redis_get_stats = w_redis_get_stats(_mem_pool, _mem_pool_1, pNewServerLocation, &stats);
//    REQUIRE(_redis_get_stats == 0);
//    if (_redis_get_stats == W_STATUS_IS_NOTFOUND)
//    {
//        //  Printf(" unable to find the socket in the poll structure");
//    }
//
//    W_RESULT _redis_ping = w_redis_ping(pNewServerLocation);
//    REQUIRE(_redis_ping == 0);
//
//    W_RESULT _redis_setex = w_redis_setex(pRedisClient, prefix2, p, len_p, 10, 27);
//    REQUIRE(_redis_setex == 0);
//
//    W_RESULT _redis_get2 = w_redis_get(_mem_pool, pRedisClient, prefix2, &result_2, &pLen_2, NULL);
//    REQUIRE(_redis_get2 == 0);
//
//    w_redis_server _redis_find_server = w_redis_find_server(pRedisClient, pHost, pPort);
//    REQUIRE(_redis_find_server != 0);
//
//    W_RESULT redis_set = w_redis_set(pRedisClient, prefix, pp, sizeof(pp) - 1, 27);
//    REQUIRE(redis_set == 0);
//
//    W_RESULT _redis_get = w_redis_get(_mem_pool, pRedisClient, prefix, &result, &pLen, NULL);
//    REQUIRE(_redis_get == 0);
//
//    W_RESULT _redis_delete = w_redis_delete(pRedisClient, prefix, 0);
//    REQUIRE(_redis_delete == 0);
//
//
//    W_RESULT redis_disable_server = w_redis_disable_server(pRedisClient, _redis_find_server);
//    REQUIRE(redis_disable_server == 0);
//
//    W_RESULT _redis_enable_server = w_redis_enable_server(pRedisClient, _redis_find_server);
//    REQUIRE(_redis_enable_server == 0);
//}

#pragma region script

//TEST_CASE("w_python")
//{
//    w_mem_pool _pool = nullptr;
//    if (w_mem_pool_init(&_pool) == W_SUCCESS)
//    {
//        char* _current_exe_path = nullptr;
//        if (w_io_dir_get_current_exe(_pool, &_current_exe_path) == W_SUCCESS)
//        {
//            _current_exe_path = w_strcat(_pool, _current_exe_path, "/script.py", NULL);
//
//            auto _script = w_io_file_read_full_from_path(_pool, _current_exe_path);
//            if (_script)
//            {
//                w_string _errors = nullptr;
//                w_hash _hash = nullptr;
//                w_hash_init(_pool, &_hash);
//
//                constexpr auto _keys_size = 4;
//                const char* _keys[_keys_size] = { "VAR_BOOL", "VAR_INT", "VAR_DOUBLE", "VAR_STRING" };
//
//                auto _python_obj = (w_python_object)w_malloc(_pool, sizeof(w_python_object_t));
//                if (_python_obj)
//                {
//                    //VAR_BOOL is the name of python object
//                    _python_obj->type = w_python_object_type::W_PYTHON_BOOLEAN;
//                    _python_obj->data = nullptr;
//                    w_hash_set(_hash, _keys[0], strlen(_keys[3]), (const void*)_python_obj);
//                }
//
//                _python_obj = (w_python_object)w_malloc(_pool, sizeof(w_python_object_t));
//                if (_python_obj)
//                {
//                    //RESULT is the name of python object
//                    _python_obj->type = w_python_object_type::W_PYTHON_NUMBER_INT;
//                    _python_obj->data = nullptr;
//                    w_hash_set(_hash, _keys[1], strlen(_keys[1]), (const void*)_python_obj);
//                }
//
//                _python_obj = (w_python_object)w_malloc(_pool, sizeof(w_python_object_t));
//                if (_python_obj)
//                {
//                    //RESULT is the name of python object
//                    _python_obj->type = w_python_object_type::W_PYTHON_NUMBER_DOUBLE;
//                    _python_obj->data = nullptr;
//                    w_hash_set(_hash, _keys[2], strlen(_keys[2]), (const void*)_python_obj);
//                }
//
//                _python_obj = (w_python_object)w_malloc(_pool, sizeof(w_python_object_t));
//                if (_python_obj)
//                {
//                    //RESULT is the name of python object
//                    _python_obj->type = w_python_object_type::W_PYTHON_STRING;
//                    _python_obj->data = nullptr;
//                    w_hash_set(_hash, _keys[3], strlen(_keys[3]), (const void*)_python_obj);
//                }
//
//                //init python
//                w_python::init();
//                //execute python
//                auto _hr = w_python::execute(_pool, (char*)_script->buffer, _hash, &_errors);
//                REQUIRE(_hr == W_SUCCESS);
//
//                if (w_hash_size(_hash))
//                {
//                    for (auto i = 0; i < _keys_size; i++)
//                    {
//                        auto _python_obj = (w_python_object)w_hash_get(_hash, _keys[i], strlen(_keys[i]));
//                        if (_python_obj)
//                        {
//                            switch (_python_obj->type)
//                            {
//                            case w_python_object_type::W_PYTHON_BOOLEAN:
//                            {
//                                auto _bool = (const bool*)_python_obj->data;
//                                if (_bool)
//                                {
//                                    printf("the type of python object is int and the value is %s \r\n", *_bool ? "true" : "false");
//                                }
//                                break;
//                            }
//                            case w_python_object_type::W_PYTHON_NUMBER_INT:
//                            {
//                                auto _int = (const int*)_python_obj->data;
//                                if (_int)
//                                {
//                                    printf("the type of python object is int and the value is %d \r\n", *_int);
//                                }
//                                break;
//                            }
//                            case w_python_object_type::W_PYTHON_NUMBER_DOUBLE:
//                            {
//                                auto _double = (const double*)_python_obj->data;
//                                if (_double)
//                                {
//                                    printf("the type of python object is double and the value is %lf \r\n", *_double);
//                                }
//                                break;
//                            }
//                            case w_python_object_type::W_PYTHON_STRING:
//                            {
//                                auto _str = (const char*)_python_obj->data;
//                                if (_str)
//                                {
//                                    printf("the type of python object is string and the value is %s \r\n", _str);
//                                }
//                                break;
//                            }
//                            }
//                        }
//                    }
//                }
//                //finish
//                w_python::fini();
//            }
//            w_mem_pool_fini(&_pool);
//        }
//    }
//}

#pragma endregion

