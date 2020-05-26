#include "w_condition_variable.h"
#include <apr-1/apr_thread_cond.h>

W_RESULT w_condition_variable_init(_Inout_ w_condition_variable** pCond)
{
    w_mem_pool _pool = w_get_default_memory_pool();
    if(!_pool)
    {
        W_ASSERT(false, "could not get default memory pool. trace info: w_condition_variable_init");
        return W_FAILURE;
    }
    
    apr_status_t _ret = apr_thread_cond_create(pCond, _pool);
    return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

W_RESULT w_condition_variable_wait(_In_ w_condition_variable* pCond, _In_ w_mutex* pMutex)
{
    apr_status_t _ret = apr_thread_cond_wait(pCond, pMutex);
    return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

W_RESULT w_condition_variable_wait_for(_Inout_  w_condition_variable* pCond,
                                       _Inout_  w_mutex* pMutex,
                                       _In_     w_timespan* pTimeout)
{
    long _micro_secs = (int64_t)(w_timespan_get_total_milliseconds(pTimeout) * 1000);
    apr_status_t _ret = apr_thread_cond_timedwait(pCond, pMutex, _micro_secs);
    return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

W_RESULT w_condition_variable_signal(_In_ w_condition_variable* pCond)
{
    apr_status_t _ret = apr_thread_cond_signal(pCond);
    return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}
 
W_RESULT w_condition_variable_broadcast(_In_ w_condition_variable* pCond)
{
    apr_status_t _ret = apr_thread_cond_broadcast(pCond);
    return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}
 
W_RESULT w_condition_variable_destroy(_In_ w_condition_variable* pCond)
{
    apr_status_t _ret = apr_thread_cond_destroy(pCond);
    return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}
 
