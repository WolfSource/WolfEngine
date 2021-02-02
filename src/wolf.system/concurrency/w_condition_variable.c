#include "w_condition_variable.h"
#include <apr_thread_cond.h>
#include <apr_thread_mutex.h>
#include <log/w_log.h>

typedef struct w_condition_variable_t
{
    apr_thread_cond_t*  condition_variable;
    apr_thread_mutex_t* mutex;
} w_condition_variable_t;

W_RESULT w_condition_variable_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_condition_variable* pConditionVariable)
{
    const char* _trace_info = "w_condition_variable_init";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "memory pool is invalid! trace info %s", _trace_info);
        return APR_BADARG;
    }
 
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        w_condition_variable_t* _cv = (w_condition_variable_t*)w_malloc(pMemPool, sizeof(w_condition_variable_t));
        if (!_cv)
        {
            W_ASSERT_P(false,
                "could not allocate memory for condition variable! trace info %s",
                _trace_info);
            return W_FAILURE;
        }

        if (apr_thread_mutex_create(
            &_cv->mutex,
            APR_THREAD_MUTEX_DEFAULT, _pool) != APR_SUCCESS)
        {
            W_ASSERT_P(false,
                "could not create mutex! trace info %s",
                _trace_info);
            return W_FAILURE;
        }

        if (apr_thread_cond_create(&_cv->condition_variable, _pool) != APR_SUCCESS)
        {
            W_ASSERT_P(false,
                "could not create condition variable! trace info %s",
                _trace_info);
            return W_FAILURE;
        }

        *pConditionVariable = _cv;

        return W_SUCCESS;
    }

    return W_FAILURE;
}

W_RESULT w_condition_variable_wait(_In_ w_condition_variable pConditionVariable)
{
    const char* _trace_info = "w_condition_variable_wait";
    if (!pConditionVariable || !pConditionVariable->condition_variable || !pConditionVariable->mutex)
    {
        W_ASSERT_P(false, "invalid parameters. trace info %s", _trace_info);
        return W_BAD_ARG;
    }
    apr_status_t _ret = apr_thread_cond_wait(
        pConditionVariable->condition_variable,
        pConditionVariable->mutex);
    return _ret == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

W_RESULT w_condition_variable_wait_for(
    _Inout_  w_condition_variable pConditionVariable,
    _In_     w_timespan pTimeout)
{
    const char* _trace_info = "w_condition_variable_wait_for";
    if (!pConditionVariable || !pConditionVariable->condition_variable || !pConditionVariable->mutex)
    {
        W_ASSERT_P(false, "invalid parameters. trace info %s", _trace_info);
        return W_BAD_ARG;
    }

    apr_interval_time_t _micro_secs = (apr_interval_time_t)(w_timespan_get_total_milliseconds(pTimeout) * 1000);
    return apr_thread_cond_timedwait(
        pConditionVariable->condition_variable,
        pConditionVariable->mutex,
        _micro_secs);
}

W_RESULT w_condition_variable_wait_for_ms(
    _Inout_  w_condition_variable pConditionVariable,
    _In_     double pMilliSeconds)
{
    const char* _trace_info = "w_condition_variable_wait_for";
    if (!pConditionVariable || !pConditionVariable->condition_variable || !pConditionVariable->mutex)
    {
        W_ASSERT_P(false, "invalid parameters. trace info %s", _trace_info);
        return W_BAD_ARG;
    }

    apr_interval_time_t _micro_secs = (apr_interval_time_t)(pMilliSeconds * 1000);
    return apr_thread_cond_timedwait(
        pConditionVariable->condition_variable,
        pConditionVariable->mutex,
        _micro_secs);
}

W_RESULT w_condition_variable_signal(_In_ w_condition_variable pConditionVariable)
{
    const char* _trace_info = "w_condition_variable_signal";
    if (!pConditionVariable || !pConditionVariable->condition_variable || !pConditionVariable->mutex)
    {
        W_ASSERT_P(false, "invalid parameters. trace info %s", _trace_info);
        return W_BAD_ARG;
    }

    return apr_thread_cond_signal(pConditionVariable->condition_variable);
}
 
W_RESULT w_condition_variable_broadcast(_In_ w_condition_variable pConditionVariable)
{
    const char* _trace_info = "w_condition_variable_broadcast";
    if (!pConditionVariable || !pConditionVariable->condition_variable || !pConditionVariable->mutex)
    {
        W_ASSERT_P(false, "invalid parameters. trace info %s", _trace_info);
        return W_BAD_ARG;
    }

    return apr_thread_cond_broadcast(pConditionVariable->condition_variable);
}
 
W_RESULT w_condition_variable_fini(_In_ w_condition_variable pConditionVariable)
{
    const char* _trace_info = "w_condition_variable_fini";
    if (!pConditionVariable || !pConditionVariable->condition_variable || !pConditionVariable->mutex)
    {
        W_ASSERT_P(false, "invalid parameters. trace info %s", _trace_info);
        return W_BAD_ARG;
    }

    return (
        apr_thread_mutex_destroy(pConditionVariable->mutex) ||
        apr_thread_cond_destroy(pConditionVariable->condition_variable)
        );
}
 
