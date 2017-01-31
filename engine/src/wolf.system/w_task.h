/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_task.h
	Description		 : A task class based Intel tbb task
	Comment          :
*/

#ifndef __W_TASK_H__
#define __W_TASK_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include <functional>

#if defined(__WIN32) || defined(__UNIVERSAL)
#include <ppltasks.h>
#endif

#include <future>
#include <functional>
#include "w_system_export.h"

namespace wolf
{
	namespace system
	{
		class w_task
		{
		public:
#if defined(__WIN32) || defined(__UNIVERSAL)
			WSYS_EXP static void execute_async_ppl(_In_ const std::function<void(void)>& pTaskWork, _In_ const std::function<void(void)>& pCallBack = nullptr);
#endif
			WSYS_EXP static void execute_async(_In_ const std::function<void(void)>& pTaskWork, _In_ const std::function<void(void)>& pCallBack = nullptr);
			WSYS_EXP static void execute_deferred(_In_ const std::function<void(void)>& pTaskWork);
			//wait only work for deferred task
			WSYS_EXP static std::future_status wait_for(_In_ const long long pMilliSeconds);
			//wait only work for deferred task
			template<typename _REP, typename _PER>
			WSYS_EXP static std::future_status wait_for(_In_ const std::chrono::duration<_REP, _PER>& pTime);
			//wait only work for deferred task
			WSYS_EXP static void wait();
			//get only work for deferred task
			WSYS_EXP static void get();

		private:
			static std::future<void> _deferred;
		};
	}
}

#if defined(__WIN32) || defined(__UNIVERSAL)

#include <tbb/task.h>

namespace tbb
{
	class Task : public tbb::task
	{
	public:
		Task::Task(std::function<void(void)>& pTaskWork)
		{
			this->_task_work = pTaskWork;
		}
		Task::Task(std::function<void(void)>& pTaskWork, tbb::priority_t pPriority)
		{
			this->_task_work = pTaskWork;
			this->set_group_priority(pPriority);
		}
		Task::Task(std::function<void(void)>& pTaskWork, std::function<void(void)>& pCallBack, tbb::priority_t pPriority)
		{
			this->_task_work = pTaskWork;
			this->_call_back = pCallBack;
			this->set_group_priority(pPriority);
		}
		
		tbb::task* execute()
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

#endif//__WIN32 || __UNIVERSAL

#endif //__W_TASK_H__