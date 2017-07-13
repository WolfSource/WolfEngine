/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_thread_pool.h
	Description		 : A cross platform thread pool class 
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_THREAD_POOL_H__
#define __W_THREAD_POOL_H__

#if  defined(__WIN32) || defined(__UWP)
#include <Windows.h>
#else
#include <thread>
#endif

#include "w_system_export.h"
#include <queue>
#include <functional>

#if defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
#include "w_std.h"
#endif

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace system
	{
		class w_thread_pool
		{
		public:
			//WSYS_EXP static void execute_async(_In_ const std::function<void(void)>& pTaskWork, _In_ const std::function<void(void)>& pCallBack = nullptr);
			//WSYS_EXP static void execute_deferred(_In_ const std::function<void(void)>& pTaskWork);
			////wait only work for deferred task
			//WSYS_EXP static std::future_status wait_for(_In_ const long long pMilliSeconds);
			////wait only work for deferred task
			//template<typename _REP, typename _PER>
			//static std::future_status wait_for(_In_ const std::chrono::duration<_REP, _PER>& pTime);
			////wait only work for deferred task
			//WSYS_EXP static void wait();
			////get only work for deferred task
			//WSYS_EXP static void get();

		private:
			static std::queue<std::function<void>> _deferred;
        };
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif //__W_TASK_H__
