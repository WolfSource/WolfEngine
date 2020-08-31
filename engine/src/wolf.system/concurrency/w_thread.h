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

    typedef void* (*w_thread_job)(w_thread, void*);
    typedef void (*w_thread_once_job)(void);

    /**
     * create once flag
     * @param pOnceFlag create once flag
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_thread_init_once_flag(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_thread_once_flag pOnceFlag);

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
            _In_      void* pJobArgs);

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
     * terminate thread
     * @param pThread thread info
    */
    W_SYSTEM_EXPORT
        void w_thread_fini(_Inout_ w_thread pThread);

    /**
     * exit thread
     * @param pExitStatus exit thread with status
    */
    W_SYSTEM_EXPORT
        void w_thread_fini_with_status(
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

    /**
     * Create and initialize a mutex that can be used to synchronize threads.
     * @param pMutex the memory address where the newly created mutex will be stored.
     * @param pFlags Or'ed value of:
     * <PRE>
     *           0x0 = THREAD_MUTEX_DEFAULT   platform-optimal lock behavior.
     *           0x1 = THREAD_MUTEX_NESTED    enable nested (recursive) locks.
     *           0x2 = THREAD_MUTEX_UNNESTED  disable nested locks (non-recursive).
                 0x4 = THREAD_MUTEX_TIMED  enable timed locks.
     * </PRE>
     * @param pMemPool the pool from which to allocate the mutex.
     * @warning Be cautious in using APR_THREAD_MUTEX_DEFAULT.  While this is the
     * most optimal mutex based on a given platform's performance characteristics,
     * it will behave as either a nested or an unnested lock.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_thread_mutex_init(_Inout_ w_mutex* pMutex,
            _In_ uint32_t pFlags,
            _In_ w_mem_pool pMemPool);


    /**
     * Acquire the lock for the given mutex. If the mutex is already locked,
     * the current thread will be put to sleep until the lock becomes available.
     * @param pMutex the mutex on which to acquire the lock.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_thread_mutex_lock(_In_ w_mutex pMutex);

    /**
     * Attempt to acquire the lock for the given mutex. If the mutex has already
     * been acquired, the call returns immediately with APR_EBUSY. Note: it
     * is important that the APR_STATUS_IS_EBUSY(s) macro be used to determine
     * if the return value was APR_EBUSY, for portability reasons.
     * @param pMutex the mutex on which to attempt the lock acquiring.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_thread_mutex_trylock(_In_ w_mutex pMutex);

    /**
     * Release the lock for the given mutex.
     * @param pMutex the mutex from which to release the lock.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_thread_mutex_unlock(_In_ w_mutex pMutex);

    /**
     * Destroy the mutex and free the memory associated with the lock.
     * @param pMutex the mutex to destroy.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_thread_mutex_fini(_In_ w_mutex pMutex);

    /**
     * Get the pool used by this thread_mutex.
     * @param pMutex mutex
     * @return memory pool
    */
    W_SYSTEM_EXPORT
        w_mem_pool  w_thread_mutex_get_mem_pool(_In_ w_mutex pMutex);

#ifdef __cplusplus
}
#endif
