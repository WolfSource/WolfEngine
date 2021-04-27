#include "w_thread_pool.h"
#include <w_log/w_log.h>

#include "apr_thread_pool.h"

W_RESULT w_thread_pool_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_thread_pool* pThreadPool,
    _In_ size_t pInitThreads,
    _In_ size_t pMaxThreads)
{
    const char* _trace_info = "w_thread_pool_init";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "memory pool is invalid! trace info %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (!_pool)
    {
        W_ASSERT_P(false, "memory pool is invalid! trace info %s", _trace_info);
        return APR_BADARG;
    }

    *pThreadPool = NULL;
    apr_thread_pool_t* _tp = (apr_thread_pool_t*)pThreadPool;
    apr_status_t _ret = apr_thread_pool_create(
        &_tp,
        (apr_size_t)pInitThreads,
        (apr_size_t)pMaxThreads,
        _pool);
    *pThreadPool = _tp;
    
    return _ret;
}

W_RESULT w_thread_pool_push(
    _In_ w_thread_pool pThreadPool,
    _In_ w_thread_job pThreadJob,
    _In_opt_ void* pParam,
    _In_ uint8_t pPriority,
    _In_opt_ void* pOwner)
{
    const char* _trace_info = "w_thread_pool_push";
    if (!pThreadPool || !pThreadJob)
    {
        W_ASSERT_P(false, "invalid parameters! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (apr_status_t)apr_thread_pool_push(
        (apr_thread_pool_t*)pThreadPool,
        (apr_thread_start_t)pThreadJob,
        pParam,
        pPriority,
        pOwner);
}

W_RESULT w_thread_pool_schedule(
    _In_ w_thread_pool pThreadPool,
    _In_ w_thread_job pThreadJob,
    _In_opt_ void* pParam,
    _In_ int64_t pTime,
    _In_opt_ void* pOwner)
{
    const char* _trace_info = "w_thread_pool_schedule";
    if (!pThreadPool || !pThreadJob)
    {
        W_ASSERT_P(false, "invalid parameters! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (apr_status_t)apr_thread_pool_schedule(
        (apr_thread_pool_t*)pThreadPool,
        (apr_thread_start_t)pThreadJob,
        pParam,
        pTime,
        pOwner);
}

W_RESULT w_thread_pool_top(
    _In_ w_thread_pool pThreadPool,
    _In_ w_thread_job pThreadJob,
    _In_opt_ void* pParam,
    _In_ uint8_t pPriority,
    _In_ void* pOwner)
{
    const char* _trace_info = "w_thread_pool_top";
    if (!pThreadPool || !pThreadJob)
    {
        W_ASSERT_P(false, "invalid parameters! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (apr_status_t)apr_thread_pool_top(
        (apr_thread_pool_t*)pThreadPool,
        (apr_thread_start_t)pThreadJob,
        pParam,
        pPriority,
        pOwner);
}

W_RESULT w_thread_pool_tasks_cancel(_In_ w_thread_pool pThreadPool, _In_ void* pOwner)
{
    const char* _trace_info = "w_thread_pool_tasks_cancel";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (apr_status_t)apr_thread_pool_tasks_cancel(
        (apr_thread_pool_t*)pThreadPool,
        pOwner);
}

size_t w_thread_pool_tasks_count(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_tasks_count";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_tasks_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_scheduled_tasks_count(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_scheduled_tasks_count";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_scheduled_tasks_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_threads_count(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_threads_count";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_threads_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_busy_count(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_busy_count";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_busy_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_idle_count(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_idle_count";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_idle_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_idle_max_set(
    _In_ w_thread_pool pThreadPool,
    _In_ size_t pCount)
{
    const char* _trace_info = "w_thread_pool_idle_max_set";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_idle_max_set((apr_thread_pool_t*)pThreadPool, pCount);
}

size_t w_thread_pool_tasks_run_count(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_tasks_run_count";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_tasks_run_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_tasks_high_count(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_tasks_high_count";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_tasks_high_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_threads_high_count(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_threads_high_count";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_threads_high_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_threads_idle_timeout_count(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_threads_idle_timeout_count";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_threads_idle_timeout_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_idle_max_get(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_idle_max_get";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_idle_max_get((apr_thread_pool_t*)pThreadPool);
}

int64_t w_thread_pool_idle_wait_get(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_idle_wait_get";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (int64_t)apr_thread_pool_idle_wait_get((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_thread_max_get(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_thread_max_get";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_thread_max_get((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_thread_max_set(
    _In_ w_thread_pool pThreadPool,
    _In_ size_t pCount)
{
    const char* _trace_info = "w_thread_pool_thread_max_set";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_thread_max_set((apr_thread_pool_t*)pThreadPool, pCount);
}

int64_t w_thread_pool_idle_wait_set(
    _In_ w_thread_pool pThreadPool,
    _In_ int64_t pTimeout)
{
    const char* _trace_info = "w_thread_pool_idle_wait_set";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (int64_t)apr_thread_pool_idle_wait_set((apr_thread_pool_t*)pThreadPool, pTimeout);
}

size_t w_thread_pool_threshold_get(_In_ w_thread_pool pThreadPool)
{
    const char* _trace_info = "w_thread_pool_threshold_get";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_threshold_get((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_threshold_set(
    _In_ w_thread_pool pThreadPool,
    _In_ size_t pValue)
{
    const char* _trace_info = "w_thread_pool_threshold_set";
    if (!pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (size_t)apr_thread_pool_threshold_set((apr_thread_pool_t*)pThreadPool, pValue);
}

W_RESULT w_thread_pool_task_owner_get(
    _In_ w_thread pThread,
    _In_ void** pOwner)
{
    const char* _trace_info = "w_thread_pool_task_owner_get";
    if (!pThread)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }
    return (W_RESULT)apr_thread_pool_task_owner_get((apr_thread_t*)pThread, pOwner);
}

void w_thread_pool_wait_for(
    _Inout_ w_thread_pool pThreadPool,
    _In_ size_t pNumberOfThreads)
{
    size_t _running_tasks = 0;
    do
    {
        _running_tasks = w_thread_pool_tasks_run_count(pThreadPool);
    } while (_running_tasks != pNumberOfThreads);
}

void w_thread_pool_wait_all(_Inout_ w_thread_pool pThreadPool)
{
    size_t _tasks = w_thread_pool_threads_count(pThreadPool);
    w_thread_pool_wait_for(pThreadPool, _tasks);
}

W_RESULT w_thread_pool_fini(_Inout_ w_thread_pool* pThreadPool)
{
    const char* _trace_info = "w_thread_pool_fini";
    if (!pThreadPool || !*pThreadPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info %s", _trace_info);
        return APR_BADARG;
    }

    apr_thread_pool_t* _thread_pool = *pThreadPool;
    apr_status_t _ret = apr_thread_pool_destroy(_thread_pool);
    if (_ret == APR_SUCCESS)
    {
        *pThreadPool = NULL;
    }
    return (W_RESULT)_ret;
}
