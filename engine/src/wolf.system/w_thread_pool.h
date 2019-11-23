/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_thread_pool.h
	Description		 : A cross platform thread pool class 
	Comment          :
*/

#pragma once

#include "w_thread.h"
#include <vector>

namespace wolf::system
{
	class w_thread_pool
	{
	public:
		WSYS_EXP w_thread_pool();
		WSYS_EXP ~w_thread_pool();

		//allocate thread pool with number of threads
		WSYS_EXP void allocate(_In_ const size_t& pNumberOfThreads);
		//wait for specific thread to be done
		WSYS_EXP void wait_for(_In_ const size_t& pThreadIndex);
		//wait for all threads
		WSYS_EXP void wait_all();
		//release all resources
		WSYS_EXP void release();

#pragma region Getters
		WSYS_EXP size_t get_pool_size() const;
#pragma endregion

#pragma region Setters
		//add jobs for specific thread
		WSYS_EXP void add_jobs_for_thread(_In_ const size_t& pThreadIndex, _In_ const std::vector<std::function<void()>>& pJobs);
		//add a job for specific thread
		WSYS_EXP void add_job_for_thread(_In_ const size_t& pThreadIndex, _In_ const std::function<void()>& pJob);
#pragma endregion

	private:
		std::vector<w_thread> _threads;
	};
}

