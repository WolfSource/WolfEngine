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
#include <chrono/w_timespan.h>

struct apr_thread_cond_t;
typedef struct apr_thread_cond_t w_condition_variable;

struct apr_thread_mutex_t;
typedef struct apr_thread_mutex_t w_mutex;

/**
 * create a signal
 * @param pCond condition variable
 * @return result
*/
W_RESULT w_condition_variable_init(_Inout_ w_condition_variable** pCond);

/**
 * Put the active calling thread to sleep until signaled to wake up. Each condition variable must be associated with a mutex, and that mutex must be locked before calling this function, or the behavior will be undefined. As the calling thread is put to sleep, the given mutex will be simultaneously released; and as this thread wakes up the lock is again simultaneously acquired.
 * @param pCond condition variable
 * @param pMutex mutex
 * @return result
*/
W_RESULT w_condition_variable_wait(_In_ w_condition_variable* pCond, _In_ w_mutex* pMutex);
 
/**
 * Put the active calling thread to sleep until signaled to wake up or the timeout is reached. Each condition variable must be associated with a mutex, and that mutex must be locked before calling this function, or the behavior will be undefined. As the calling thread is put to sleep, the given mutex will be simultaneously released; and as this thread wakes up the lock is again simultaneously acquired.
 * @param pCond condition variable
 * @param pMutex mutex
 * @return result
*/
W_RESULT w_condition_variable_wait_for(_Inout_ w_condition_variable* pCond,
                                       _Inout_ w_mutex* pMutex,
                                       _In_ w_timespan* pTimeout);
 
/**
 * Signals a single thread, if one exists, that is blocking on the given condition variable. That thread is then scheduled to wake up and acquire the associated mutex. Although it is not required, if predictable scheduling is desired, that mutex must be locked while calling this function.
 * @param pCond condition variable
 * @return result
*/
W_RESULT w_condition_variable_signal(_In_ w_condition_variable* pCond);
 
/**
 * Signals all threads blocking on the given condition variable. Each thread that was signaled is then scheduled to wake up and acquire the associated mutex. This will happen in a serialized manner.
 * @param pCond condition variable
 * @return result
*/
W_RESULT w_condition_variable_broadcast(_In_ w_condition_variable* pCond);
 
/**
 * Destroy the condition variable and free the associated memory.
 * @param pCond condition variable
 * @return result
*/
W_RESULT w_condition_variable_destroy(_In_ w_condition_variable* pCond);
 

#ifdef __cplusplus
}
#endif
