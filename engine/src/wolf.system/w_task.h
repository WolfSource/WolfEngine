/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_task.h
	Description		 : A task class based Intel tbb task
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_TASK_H__
#define __W_TASK_H__

#include <functional>

#if defined(__WIN32) || defined(__UWP)
#include <ppltasks.h>
#endif

#include <future>
#include <functional>
#include "w_system_export.h"

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
		class w_task
		{
		public:
#if defined(__WIN32) || defined(__UWP)
			WSYS_EXP static void execute_async_ppl(_In_ const std::function<void(void)>& pTaskWork, _In_ const std::function<void(void)>& pCallBack = nullptr);
#endif
			WSYS_EXP static void execute_async(_In_ const std::function<void(void)>& pTaskWork, _In_ const std::function<void(void)>& pCallBack = nullptr);
			WSYS_EXP static void execute_deferred(_In_ const std::function<void(void)>& pTaskWork);
			//wait only work for deferred task
			WSYS_EXP static std::future_status wait_for(_In_ const long long pMilliSeconds);
			//wait only work for deferred task
			template<typename _REP, typename _PER>
			static std::future_status wait_for(_In_ const std::chrono::duration<_REP, _PER>& pTime);
			//wait only work for deferred task
			WSYS_EXP static void wait();
			//get only work for deferred task
			WSYS_EXP static void get();

		private:
			static std::future<void> _deferred;
		};
	}
}

#if !defined(__ANDROID) && !defined(__iOS__)

#include <tbb/task.h>

namespace tbb
{
	class Task : public tbb::task
	{
	public:
		WSYS_EXP Task(_In_ std::function<void(void)>& pTaskWork)
		{
			this->_task_work = pTaskWork;
		}
		WSYS_EXP Task(_In_ std::function<void(void)>& pTaskWork, _In_ tbb::priority_t pPriority)
		{
			this->_task_work = pTaskWork;
			this->set_group_priority(pPriority);
		}
		WSYS_EXP Task(_In_ std::function<void(void)>& pTaskWork, _In_ std::function<void(void)>& pCallBack, _In_ tbb::priority_t pPriority)
		{
			this->_task_work = pTaskWork;
			this->_call_back = pCallBack;
			this->set_group_priority(pPriority);
		}
		
		WSYS_EXP tbb::task* execute()
		{
			if (_task_work)
			{
				_task_work();
			}

			if (_call_back)
			{
				_call_back();
			}

			return NULL;
		}

	private:
		std::function<void(void)> _task_work;
		std::function<void(void)> _call_back;
	};
}

#endif//__WIN32 || __UWP

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif //__W_TASK_H__
