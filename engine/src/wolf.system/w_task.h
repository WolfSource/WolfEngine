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

#include <functional>
//#include <tbb/task.h>

#ifdef _WIN32
#include <ppltasks.h>
#else
#include <future>
#endif

namespace wolf
{
	namespace system
	{
		class w_task
		{
		public:
			static void execute(std::function<void(void)> pTaskWork)
			{
#ifdef _WIN32
				auto _task = concurrency::create_task(pTaskWork);
#else
				std::async(std::launch::async, pTaskWork);
#endif
			}
		};

	}
}

//New version of TBB not work fine on AMD
//namespace tbb
//{
//	class Task : public tbb::task
//	{
//	public:
//		Task::Task(std::function<void(void)> pTaskWork)
//		{
//			this->_task_work = pTaskWork;
//		}
//		Task::Task(std::function<void(void)> pTaskWork, tbb::priority_t pPriority)
//		{
//			this->_task_work = pTaskWork;
//			this->set_group_priority(pPriority);
//		}
//
//		tbb::task* execute()
//		{
//			if (_task_work)
//			{
//				_task_work();
//			}
//
//			return NULL;
//		}
//
//	private:
//		std::function<void(void)> _task_work;
//	};
//}

#endif