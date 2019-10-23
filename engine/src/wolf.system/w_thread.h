/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#include <thread>

#include "w_system_export.h"
#include <functional>

#if defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
#include "w_std.h"
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
            WSYS_EXP static size_t get_current_thread_id()
            {
                return std::hash<std::thread::id>{}(std::this_thread::get_id());
            }
            WSYS_EXP static void sleep_current_thread(_In_ const long long& pMilliSeconds)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(pMilliSeconds));
            }
#endif


        private:
            w_thread_pimp*                          _pimp;	
        };
	}
}

#endif //__W_THREAD_H__
