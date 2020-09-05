#include "w_condition_variable.h"
#include <apr-1/apr_thread_cond.h>

W_RESULT w_condition_variable_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_condition_variable* pCond)
{
    const char* _trace_info = "w_condition_variable_init";
    if (pMemPool)
    {
        apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
        if (_pool)
        {
            return apr_thread_cond_create(pCond, _pool);
        }
    }
    W_ASSERT_P(false, "bad args. trace info %s", _trace_info);
    return APR_BADARG;
}

W_RESULT w_condition_variable_wait(_In_ w_condition_variable pCond, _In_ w_mutex pMutex)
{
    apr_status_t _ret = apr_thread_cond_wait(pCond, pMutex);
    return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

W_RESULT w_condition_variable_wait_for(
    _Inout_  w_condition_variable pCond,
    _Inout_  w_mutex pMutex,
    _In_     w_timespan pTimeout)
{
    apr_interval_time_t _micro_secs = (apr_interval_time_t)(w_timespan_get_total_milliseconds(pTimeout) * 1000);
    return apr_thread_cond_timedwait(pCond, pMutex, _micro_secs);
}

W_RESULT w_condition_variable_signal(_In_ w_condition_variable pCond)
{
    return apr_thread_cond_signal(pCond);
}
 
W_RESULT w_condition_variable_broadcast(_In_ w_condition_variable pCond)
{
    return apr_thread_cond_broadcast(pCond);
}
 
W_RESULT w_condition_variable_destroy(_In_ w_condition_variable pCond)
{
    return apr_thread_cond_destroy(pCond);
}
 
