/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_timer_callback.h
	Description		 : A timer with callback
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_TIMER_CALLBACK_H__
#define __W_TIMER_CALLBACK_H__

#include "w_timer.h"
#include "w_thread.h"

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
			void do_sync(int pIntervalMilliSeconds, T&& pFunc)
			{
				w_thread::sleep_current_thread(pIntervalMilliSeconds);
				pFunc();
			}

			template <class T>
			void do_async(int pIntervalMilliSeconds, T&& pFunc)
			{
				std::thread  t([pIntervalMilliSeconds, &pFunc]()
				{
					w_thread::sleep_current_thread(pIntervalMilliSeconds);
					pFunc();
				});
				t.detach();
			}
		};
	}
}

#endif //__W_TIMER_CALLBACK_H__
