/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_condition_variable.h
    Description      : condition variable
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include "chrono/w_timespan.h" 

    struct w_condition_variable_t;
    typedef struct w_condition_variable_t* w_condition_variable;

    /**
     * create a signal
     * @param pMemPool the pool from which to allocate the condition variable
     * @param pConditionVariable condition variable
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_condition_variable_init(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_condition_variable* pConditionVariable);

    /**
     * Put the active calling thread to sleep until signaled to wake up. Each condition variable must be associated with a mutex, and that mutex must be locked before calling this function, or the behavior will be undefined. As the calling thread is put to sleep, the given mutex will be simultaneously released; and as this thread wakes up the lock is again simultaneously acquired.
     * @param pConditionVariable condition variable
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_condition_variable_wait(_In_ w_condition_variable pConditionVariable);

    /**
     * Put the active calling thread to sleep until signaled to wake up or the timeout is reached. 
     * Each condition variable must be associated with a mutex, 
     * and that mutex must be locked before calling this function, or the behavior will be undefined. 
     * As the calling thread is put to sleep, the given mutex will be simultaneously released;   
     * @param pConditionVariable condition variable
     * @param pTime the timespan
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_condition_variable_wait_for(
            _Inout_ w_condition_variable pConditionVariable,
            _In_ w_timespan pTime);

    /**
     * Put the active calling thread to sleep until signaled to wake up or the timeout is reached. 
     * Each condition variable must be associated with a mutex, 
     * and that mutex must be locked before calling this function, or the behavior will be undefined. 
     * As the calling thread is put to sleep, the given mutex will be simultaneously released; 
     * and as this thread wakes up the lock is again simultaneously acquired.
     * @param pConditionVariable condition variable
     * @param pMilliSeconds the total milliseconds for wait
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_condition_variable_wait_for_ms(
            _Inout_  w_condition_variable pConditionVariable,
            _In_     double pMilliSeconds);

    /**
     * Signals a single thread, if one exists, that is blocking on the given condition variable. 
     * That thread is then scheduled to wake up and acquire the associated mutex. 
     * Although it is not required, if predictable scheduling is desired, 
     * that mutex must be locked while calling this function.
     * @param pConditionVariable condition variable
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_condition_variable_signal(_In_ w_condition_variable pConditionVariable);

    /**
     * Signals all threads blocking on the given condition variable. 
     * Each thread that was signaled is then scheduled to wake up and acquire the associated mutex. 
     * This will happen in a serialized manner.
     * @param pConditionVariable condition variable
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_condition_variable_broadcast(_In_ w_condition_variable pConditionVariable);

    /**
     * Destroy the condition variable and free the associated memory.
     * @param pConditionVariable condition variable
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_condition_variable_fini(_In_ w_condition_variable pConditionVariable);

#ifdef __cplusplus
}
#endif
