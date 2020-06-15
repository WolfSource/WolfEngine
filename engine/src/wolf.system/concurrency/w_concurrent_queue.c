#include "w_concurrent_queue.h"
#include <apr-util/apr_queue.h>

W_RESULT  w_concurrent_queue_init(_Inout_ w_concurrent_queue* pQueue,
                                  _In_ const uint32_t pQueueCapacity,
                                  _In_ w_mem_pool pMemPool)
{
    apr_status_t _rt = apr_queue_create((apr_queue_t**)pQueue, pQueueCapacity, pMemPool);
    if (_rt != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened. error code: %d. trace info: w_concurrent_queue_create", _rt);
    }
    return _rt;
}

W_RESULT  w_concurrent_queue_push(_In_ w_concurrent_queue pQueue, _In_ void* pData)
{
    apr_status_t _rt = apr_queue_push((apr_queue_t*)pQueue, pData);
    if (_rt != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened. error code: %d, trace info: w_concurrent_queue_push", _rt);
    }
    return _rt;
}

W_RESULT  w_concurrent_queue_pop(_In_ w_concurrent_queue pQueue, _Inout_ void** pData)
{
    apr_status_t _rt = apr_queue_pop((apr_queue_t*)pQueue, pData);
    if (_rt != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened. error code: %d, trace info: w_concurrent_queue_pop", _rt);
    }
    return _rt;
}

W_RESULT  w_concurrent_queue_trypush(_In_ w_concurrent_queue pQueue, _In_ void* pData)
{
    apr_status_t _rt = apr_queue_trypush((apr_queue_t*)pQueue, pData);
    if (_rt != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened. error code: %d, trace info: w_concurrent_queue_trypush", _rt);
    }
    return _rt;
}

W_RESULT  w_concurrent_queue_trypop(_In_ w_concurrent_queue pQueue, _Inout_ void** pData)
{
    apr_status_t _rt = apr_queue_trypop((apr_queue_t*)pQueue, pData);
    if (_rt != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened. error code: %d, trace info: w_concurrent_queue_trypop", _rt);
    }
    return _rt;
}

uint32_t  w_concurrent_queue_size(_In_ w_concurrent_queue pQueue)
{
    return apr_queue_size((apr_queue_t*)pQueue);
}

W_RESULT  w_concurrent_queue_interrupt_all(_In_ w_concurrent_queue pQueue)
{
    apr_status_t _rt = apr_queue_interrupt_all((apr_queue_t*)pQueue);
    if (_rt != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened. error code: %d, trace info: w_concurrent_queue_interrupt_all", _rt);
    }
    return _rt;
}

W_RESULT  w_concurrent_queue_term(_In_ w_concurrent_queue pQueue)
{
    apr_status_t _rt = apr_queue_term((apr_queue_t*)pQueue);
    if (_rt != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened. error code: %d, trace info: w_concurrent_queue_term", _rt);
    }
    return _rt;
}
