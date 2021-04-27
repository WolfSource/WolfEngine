#pragma once

#include "../w_test/base.hpp"

#include "w_atomic.h"
#include "w_concurrent_queue.h"
#include "w_condition_variable.h"
#include "w_mutex.h"
#include "w_thread.h"
#include "w_thread_pool.h"

TEST_CASE("w_concurrency/w_atomic")
{
	W_ATOMIC_INT _atomic_int_mem = 0;
	w_atomic_set(&_atomic_int_mem, 123);

	w_atomic_inc(&_atomic_int_mem);
	REQUIRE(w_atomic_read(&_atomic_int_mem) == 124);

	w_atomic_dec(&_atomic_int_mem);
	REQUIRE(w_atomic_read(&_atomic_int_mem) == 123);
}

TEST_CASE("w_concurrency/w_concurrent_queue")
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

TEST_CASE("w_concurrency/w_condition_variable")
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

TEST_CASE("w_concurrency/w_mutex")
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

TEST_CASE("w_concurrency/w_thread")
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

TEST_CASE("w_concurrency/w_thread_pool")
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

	//wait for two tasks to be done
	w_thread_pool_wait_for(_thread_pool, 2);

	//wait for all threads
	REQUIRE(w_thread_pool_fini(&_thread_pool) == W_SUCCESS);

	w_fini(&_mem_pool);
}