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

#include "w_thread.h"
#include <vector>

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
            WSYS_EXP w_thread_pool();
            WSYS_EXP ~w_thread_pool();

            WSYS_EXP void allocate(_In_ const size_t& pSize);
            WSYS_EXP void wait_for(_In_ const size_t& pThreadIndex);
            WSYS_EXP void wait_all();
            WSYS_EXP void release();

#pragma region Getters
            WSYS_EXP size_t get_pool_size() const;
#pragma endregion

#pragma region Setters
            WSYS_EXP void add_jobs_for_thread(_In_ const size_t& pThreadIndex, _In_ const std::vector<std::function<void()>>& pJobs);
            WSYS_EXP void add_job_for_thread(_In_ const size_t& pThreadIndex, _In_ const std::function<void()>& pJob);
#pragma endregion

        private:
            std::vector<w_thread> _threads;
        };
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif //__W_THREAD_H__
