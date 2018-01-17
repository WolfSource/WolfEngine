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
#include "w_std.h"

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
			WSYS_EXP static void execute_async(_In_ const std::function<HRESULT(void)>& pTaskWork, _In_ const std::function<void(HRESULT)>& pCallBack = nullptr);
			WSYS_EXP static void execute_deferred(_In_ const std::function<HRESULT(void)>& pTaskWork);
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
			static std::future<HRESULT> _deferred;
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
		WSYS_EXP Task(_In_ std::function<HRESULT(void)>& pTaskWork)
		{
			this->_task_work = pTaskWork;
		}
		WSYS_EXP Task(_In_ std::function<HRESULT(void)>& pTaskWork, _In_ tbb::priority_t pPriority)
		{
			this->_task_work = pTaskWork;
			this->set_group_priority(pPriority);
		}
		WSYS_EXP Task(_In_ std::function<HRESULT(void)>& pTaskWork, _In_ std::function<void(HRESULT)>& pCallBack, _In_ tbb::priority_t pPriority)
		{
			this->_task_work = pTaskWork;
			this->_call_back = pCallBack;
			this->set_group_priority(pPriority);
		}
		
		WSYS_EXP tbb::task* execute()
		{
            HRESULT _hr;
			if (_task_work)
			{
                _hr = _task_work();
			}

			if (_call_back)
			{
				_call_back(_hr);
			}

			return NULL;
		}

	private:
		std::function<HRESULT(void)> _task_work;
		std::function<void(HRESULT)> _call_back;
	};
}

#endif//__WIN32 || __UWP

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif //__W_TASK_H__
