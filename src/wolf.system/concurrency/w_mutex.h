/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_mutex.h
    Description      : a basic wrapper over POSIX or UNIX mutex
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

    //forward declaration
    typedef struct apr_thread_mutex_t* w_mutex;   

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
        W_RESULT    w_mutex_init(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_mutex* pMutex,
            _In_ uint32_t pFlags);

    /**
     * Acquire the lock for the given mutex. If the mutex is already locked,
     * the current thread will be put to sleep until the lock becomes available.
     * @param pMutex the mutex on which to acquire the lock.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_mutex_lock(_In_ w_mutex pMutex);

    /**
     * Attempt to acquire the lock for the given mutex. If the mutex has already
     * been acquired, the call returns immediately with APR_EBUSY. Note: it
     * is important that the APR_STATUS_IS_EBUSY(s) macro be used to determine
     * if the return value was APR_EBUSY, for portability reasons.
     * @param pMutex the mutex on which to attempt the lock acquiring.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_mutex_trylock(_In_ w_mutex pMutex);

    /**
     * Release the lock for the given mutex.
     * @param pMutex the mutex from which to release the lock.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_mutex_unlock(_In_ w_mutex pMutex);

    /**
     * Destroy the mutex and free the memory associated with the lock.
     * @param pMutex the mutex to destroy.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_mutex_fini(_In_ w_mutex* pMutex);

    /**
     * Get the pool used by this thread_mutex.
     * @param pMutex mutex
     * @return memory pool
    */
    W_SYSTEM_EXPORT
        w_mem_pool  w_mutex_get_mem_pool(_In_ w_mutex pMutex);

#ifdef __cplusplus
}
#endif
