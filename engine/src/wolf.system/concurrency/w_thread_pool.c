#include "w_thread_pool.h"
#include <apr-1/apr_thread_proc.h>
#include <apr-util/apr_thread_pool.h>

w_thread_pool w_thread_pool_init(_In_ size_t pMinThreads,
                                 _In_ size_t pMaxThreads,
                                 _In_ w_mem_pool pMemoryPool)
{
    apr_thread_pool_t* _thread_pool;
    apr_status_t _ret = apr_thread_pool_create (&_thread_pool,
                                                pMinThreads,
                                                pMaxThreads,
                                                (apr_pool_t*)pMemoryPool);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT(false, "error happened, trace info: w_thread_pool_create\n");
        return NULL;
    }
    
    return (w_thread_pool)_thread_pool;
}
  
W_RESULT  w_thread_pool_free(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_destroy((apr_thread_pool_t*)pThreadPool) == APR_SUCCESS;
}

W_RESULT  w_thread_pool_push(_In_ w_thread_pool pThreadPool,
                             _In_ w_thread_job pTaskFunction,
                             _In_ void* pParam,
                             _In_ uint8_t pPriority,
                             _In_ void* pOwner)
{
    apr_status_t _ret = apr_thread_pool_push((apr_thread_pool_t*)pThreadPool,
                                             (apr_thread_start_t)pTaskFunction,
                                             pParam,
                                             pPriority,
                                             pOwner);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT(false, "error happened, trace info: w_thread_pool_push\n");
        return W_FAILURE;
    }
    
    return W_SUCCESS;
}

W_RESULT w_thread_pool_schedule(_In_ w_thread_pool pThreadPool,
                                _In_ w_thread_job pTaskFunction,
                                _In_ void* pParam,
                                _In_ long  pIntervalTime,
                                _In_ void* pOwner)
{
    apr_status_t _ret = apr_thread_pool_schedule((apr_thread_pool_t*)pThreadPool,
                                                 (apr_thread_start_t)pTaskFunction,
                                                 pParam,
                                                 pIntervalTime,
                                                 pOwner);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT(false, "error happened, trace info: w_thread_pool_schedule\n");
        return W_FAILURE;
    }
    
    return W_SUCCESS;
}

W_RESULT w_thread_pool_top(_In_ w_thread_pool pThreadPool,
                           _In_ w_thread_job pTaskFunction,
                           _In_ void* pParam,
                           _In_ uint8_t pPriority,
                           _In_ void* pOwner)
{
    apr_status_t _ret = apr_thread_pool_top((apr_thread_pool_t*)pThreadPool,
                                            (apr_thread_start_t)pTaskFunction,
                                             pParam,
                                             pPriority,
                                             pOwner);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT(false, "error happened, trace info: w_thread_pool_top\n");
        return W_FAILURE;
    }
    
    return W_SUCCESS;
}

W_RESULT w_thread_pool_tasks_cancel(_In_ w_thread_pool pThreadPool, void* pOwner)
{
    apr_status_t _ret = apr_thread_pool_tasks_cancel((apr_thread_pool_t*)pThreadPool, pOwner);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT(false, "error happened, trace info: w_thread_pool_tasks_cancel\n");
        return W_FAILURE;
    }
    
    return W_SUCCESS;
}

size_t w_thread_pool_tasks_count(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_tasks_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_scheduled_tasks_count(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_scheduled_tasks_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_threads_count (_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_threads_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_busy_count (_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_busy_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_idle_count (_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_idle_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_idle_max_set (_In_ w_thread_pool pThreadPool, _In_ size_t pCount)
{
    return apr_thread_pool_idle_max_set((apr_thread_pool_t*)pThreadPool, pCount);
}

size_t w_thread_pool_tasks_run_count(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_tasks_run_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_tasks_high_count(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_tasks_high_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_threads_high_count(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_threads_high_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_threads_idle_timeout_count(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_threads_idle_timeout_count((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_idle_max_get(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_idle_max_get((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_thread_max_set(_In_ w_thread_pool pThreadPool, _In_ size_t pCount)
{
    return apr_thread_pool_thread_max_set((apr_thread_pool_t*)pThreadPool, pCount);
}

long w_thread_pool_idle_wait_set(_In_ w_thread_pool pThreadPool, _In_ long pTimeOut)
{
    return apr_thread_pool_idle_wait_set((apr_thread_pool_t*)pThreadPool, pTimeOut);
}

long w_thread_pool_idle_wait_get(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_idle_wait_get((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_thread_max_get(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_thread_max_get((apr_thread_pool_t*)pThreadPool);
}

size_t w_thread_pool_threshold_set(_In_ w_thread_pool pThreadPool, _In_ size_t pValue)
{
    return apr_thread_pool_threshold_set((apr_thread_pool_t*)pThreadPool, pValue);
}

size_t w_thread_pool_threshold_get(_In_ w_thread_pool pThreadPool)
{
    return apr_thread_pool_threshold_get((apr_thread_pool_t*)pThreadPool);
}

W_RESULT w_thread_pool_task_owner_get(_In_ w_thread pThread, _In_ void** pOwner)
{
    apr_status_t _ret = apr_thread_pool_task_owner_get((apr_thread_t*)pThread, pOwner);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT(false, "error happened, trace info: w_thread_pool_task_owner_get\n");
        return W_FAILURE;
    }
    
    return W_SUCCESS;
}
