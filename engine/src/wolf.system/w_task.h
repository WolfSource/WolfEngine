/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_task.h
	Description		 : A task class
	Comment          :
*/

#pragma once

#include <functional>

#if defined(__WIN32) || defined(__UWP)
#include <ppltasks.h>
#endif

#include <future>
#include <functional>
#include "w_system_export.h"
#include "w_std.h"

namespace wolf::system
{
	class w_task
	{
	public:
		WSYS_EXP static void execute_async(_In_ const std::function<W_RESULT(void)>& pTaskWork, _In_ const std::function<void(W_RESULT)>& pCallBack = nullptr);
		WSYS_EXP static void execute_deferred(_In_ const std::function<W_RESULT(void)>& pTaskWork);
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
		static std::future<W_RESULT> _deferred;
	};
}

//#if !defined(__ANDROID) && !defined(__iOS__) && !defined(__PYTHON__)
//
//#include <tbb/task.h>
//
//namespace tbb
//{
//	class Task : public tbb::task
//	{
//	public:
//		WSYS_EXP Task(_In_ std::function<W_RESULT(void)>& pTaskWork)
//		{
//			this->_task_work = pTaskWork;
//		}
//		WSYS_EXP Task(_In_ std::function<W_RESULT(void)>& pTaskWork, _In_ tbb::priority_t pPriority)
//		{
//			this->_task_work = pTaskWork;
//			this->set_group_priority(pPriority);
//		}
//		WSYS_EXP Task(_In_ std::function<W_RESULT(void)>& pTaskWork, _In_ std::function<void(W_RESULT)>& pCallBack, _In_ tbb::priority_t pPriority)
//		{
//			this->_task_work = pTaskWork;
//			this->_call_back = pCallBack;
//			this->set_group_priority(pPriority);
//		}
//		
//		WSYS_EXP tbb::task* execute()
//		{
//            W_RESULT _hr;
//			if (_task_work)
//			{
//                _hr = _task_work();
//			}
//
//			if (_call_back)
//			{
//				_call_back(_hr);
//			}
//
//			return NULL;
//		}
//
//	private:
//		std::function<W_RESULT(void)> _task_work;
//		std::function<void(W_RESULT)> _call_back;
//	};
//}
//
//#endif//__WIN32 || __UWP
