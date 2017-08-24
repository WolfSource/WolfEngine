/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_thread.h
	Description		 : A cross platform thread class 
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_THREAD_H__
#define __W_THREAD_H__

#if  defined(__WIN32) || defined(__UWP)
#include <Windows.h>
#endif

#include <thread>

#include "w_system_export.h"
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
        class w_thread_pimp;
        class w_thread
        {
        public:
            WSYS_EXP w_thread();
            WSYS_EXP ~w_thread();

            WSYS_EXP void add_job(_In_ const std::function<void()>& pJob);
            WSYS_EXP void wait();
            WSYS_EXP void release();

            //hardware_thread_contexts usually equal to number of CPU cores
            WSYS_EXP static unsigned int get_number_of_hardware_thread_contexts()
            {
                return std::thread::hardware_concurrency();
            }

#if defined(__WIN32) || defined(__UWP)
            WSYS_EXP static DWORD get_current_thread_id()
            {
                return GetCurrentThreadId();
            }
            WSYS_EXP static void sleep_current_thread(_In_ const DWORD& pMilliSecond)
            {
                Sleep(pMilliSecond);
            }
#else
            WSYS_EXP static std::thread::id get_current_thread_id()
            {
                return std::this_thread::get_id();
            }
            WSYS_EXP static void sleep_current_thread(_In_ const long long& pMilliSecond)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(pMilliSecond));
            }
#endif


        private:
            w_thread_pimp*                          _pimp;
        };
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif //__W_THREAD_H__
