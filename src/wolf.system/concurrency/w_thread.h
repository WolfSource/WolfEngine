/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_thread.h
    Description      : a basic wrapper over POSIX or UNIX thread
    Comment          :
*/


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

    //forward declaration
    typedef struct apr_thread_t* w_thread;
    typedef struct apr_thread_once_t* w_thread_once_flag;
    typedef struct apr_thread_mutex_t* w_mutex;
    typedef void* w_thread_id;

    typedef void* (
#if defined(W_PLATFORM_ANDROID) || defined(W_PLATFORM_LINUX)
#else
    __stdcall 
#endif
        *w_thread_job)(w_thread, void*);

    typedef void (*w_thread_once_job)(void);

    

    /**
     * create once flag
     * @param pMemPool the pool from which to allocate the once flag
     * @param pOnceFlag create once flag
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_thread_init_once_flag(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_thread_once_flag* pOnceFlag);

    /**
     * Run the specified function one time, regardless of how many threads call it.
     * @param pOnceFlag once flag
     * @param pOnceJob job
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_thread_once_call(
            _Inout_ w_thread_once_flag pOnceFlag, 
            _In_ w_thread_once_job pOnceJob);

    /**
     * create a thread
     * @param pMemPool the pool from which to allocate the thread
     * @param pThread thread info
     * @param pJob thread job
     * @param pJobArgs thread arguments as void*
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_thread_init(
            _Inout_   w_mem_pool pMemPool,
            _Inout_   w_thread* pThread,
            _In_      w_thread_job  pJob,
            _In_opt_  void* pJobArgs);

    /**
     * join a thread
     * @param pThread thread info
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_thread_join(_Inout_ w_thread pThread);

    /**
     * detach a thread
     * @param pThread thread info
    */
    W_SYSTEM_EXPORT
        void w_thread_detach(_Inout_ w_thread pThread);

    /**
     * exit thread
     * @note call this function within itself
     * @param pThread thread
    */
    W_SYSTEM_EXPORT
        void w_thread_exit(_Inout_ w_thread pThread);

    /**
     * exit thread
     * @note call this function within itself
     * @param pExitStatus exit thread with status
    */
    W_SYSTEM_EXPORT
        void w_thread_exit_with_status(
            _Inout_ w_thread pThread, 
            _In_ int pExitStatus);

    /**
     * get current thread
     * @return get current thread info
     */
    W_SYSTEM_EXPORT
        w_thread w_thread_get_current(void);

    /**
     * get current thread id
     * @return get current thread id
     */
    W_SYSTEM_EXPORT
        w_thread_id w_thread_get_current_id(void);

    /**
     * get two threads are equal or not
     * @param pThread1 first thread id
     * @param pThread2 second thread id
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_thread_current_ids_are_equal(
            _In_ w_thread_id pThread1, 
            _In_ w_thread_id pThread2);

    /**
     * sleep current thread
     * @param pTime in nanoseconds
    */
    W_SYSTEM_EXPORT
        void w_thread_current_sleep_for_nanoseconds(_In_ double pTime);

    /**
     * sleep current thread
     * @param pTime in microseconds
     */
    W_SYSTEM_EXPORT
        void w_thread_current_sleep_for_microseconds(_In_ double pTime);

    /**
     * sleep current thread
     * @param pTime in milliseconds
     */
    W_SYSTEM_EXPORT
        void w_thread_current_sleep_for_milliseconds(_In_ double pTime);

    /**
     * sleep current thread
     * @param pTime in seconds
    */
    W_SYSTEM_EXPORT
        void w_thread_current_sleep_for_seconds(_In_ double pTime);

#if !defined(W_PLATFORM_ANDROID) && !defined(W_PLATFORM_IOS)
    /**
     * get number of cpus and threads
     * @param pCores is number of CPU Cores
     * @param pThreads is number of threads
     * @param pActualThreads is number of actual threads
    */
    W_SYSTEM_EXPORT
        void w_thread_get_number_of_cpu_threads(
            _Inout_ int* pCores,
            _Inout_ int* pThreads,
            _Inout_ int* pActualThreads);
#endif

#ifdef __cplusplus
}
#endif
