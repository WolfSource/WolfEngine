/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_timer.h
	Description		 : A timer based on QueryPerformanceCounter
	Comment          :
*/

#ifndef __W_TIMER_CALLBACK_H__
#define __W_TIMER_CALLBACK_H__

#include "w_timer.h"
#include <thread>

namespace wolf
{
	namespace system
	{
		class w_timer_callback
		{
		public:
			w_timer_callback() {};
			~w_timer_callback() {};

			template <class T>
			void do_sync(int pIntervalMS, T&& pFunc)
			{
				w_timer _timer;
				_timer.start();
				while (_timer.getTotalMilliSeconds() < pIntervalMS)
				{
					_timer.update();
				}
				pFunc();
			}

			template <class T>
			void do_async(int pIntervalMS, T&& pFunc)
			{
				std::thread  t([pIntervalMS, pFunc]()
				{
					w_timer _timer;
					_timer.start();
					while (_timer.getTotalMilliSeconds() < pIntervalMS)
					{
						_timer.update();
					}
					pFunc();
				});
				t.detach();
			}

			//if (async)
			//{
			//	std::thread([after, task]() {
			//		std::this_thread::sleep_for(std::chrono::milliseconds(after));
			//		task();
			//	}).detach();
			//}
			//else
			//{
			//	std::this_thread::sleep_for(std::chrono::milliseconds(after));
			//	task();
			//}
		};
	}
}

#endif