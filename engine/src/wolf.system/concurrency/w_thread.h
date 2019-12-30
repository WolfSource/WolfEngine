/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_thread.h
    Description      : a basic wrapper over pthread and win32 thread
    Comment          :
*/


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

//forward declaration
typedef struct apr_thread_t w_thread;
typedef struct apr_thread_once_t w_thread_once_flag;
typedef void* (*w_thread_job)(w_thread*,void*);
typedef void (*w_thread_once_job)(void);

/**
 * create once flag
 * @param pOnceFlag create once flag
 * @return result
*/
W_RESULT w_thread_create_once_flag(_Inout_ w_thread_once_flag* pOnceFlag);

/**
 * Run the specified function one time, regardless of how many threads call it.
 * @param pOnceFlag once flag
 * @param pOnceJob job
 * @return result
*/
W_RESULT w_thread_once_call(_Inout_ w_thread_once_flag* pOnceFlag, _In_ w_thread_once_job pOnceJob);

/**
 * create a thread
 * @param pThread thread info
 * @param pJob thread job
 * @param pJobArgs thread arguments as void*
 * @return result
*/
W_RESULT w_thread_create(_Inout_   w_thread*      pThread,
                         _In_      w_thread_job   pJob,
                         _In_      void*          pJobArgs);

/**
 * join a thread
 * @param pThread thread info
 * @return result
*/
W_RESULT w_thread_join(_Inout_ w_thread* pThread);

/**
 * detach a thread
 * @param pThread thread info
*/
void w_thread_detach(_Inout_ w_thread* pThread);

/**
 * terminate thread
 * @param pThread thread info
*/
void w_thread_terminate(_Inout_ w_thread* pThread);

/**
 * exit thread
 * @param pExitStatus exit thread with status
*/
void w_thread_terminate_with_status(_Inout_ w_thread* pThread, _In_ const int pExitStatus);

/**
 * exit thread
 * @return get current thread info
 */
w_thread* w_thread_get_current_id(void);

/**
 * sleep current thread
 * @param pTime in nanoseconds
*/
void w_thread_current_sleep_for_nanoseconds(_In_ const double pTime);

/**
 * sleep current thread
 * @param pTime in microseconds
 */
void w_thread_current_sleep_for_microseconds(_In_ const double pTime);

/**
 * sleep current thread
 * @param pTime in milliseconds
 */
void w_thread_current_sleep_for_milliseconds(_In_ const double pTime);

/**
 * sleep current thread
 * @param pTime in seconds
*/
void w_thread_current_sleep_for_seconds(_In_ const double pTime);

/**
 * get number of cpus and threads
 * @param pCores is number of CPU Cores
 * @param pThreads is number of threads
 * @param pActualThreads is number of actual threads
*/
void w_thread_get_number_of_cpu_threads(_Inout_ int* pCores,
                                        _Inout_ int* pThreads,
                                        _Inout_ int* pActualThreads);

/*
namespace wolf::system
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
		w_thread_pimp* _pimp;
	};
}*/


#ifdef __cplusplus
}
#endif
