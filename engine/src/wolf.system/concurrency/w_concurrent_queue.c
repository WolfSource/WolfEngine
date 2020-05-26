#include "w_concurrent_queue.h"
#include <apr-util/apr_queue.h>

W_RESULT  w_concurrent_queue_create(_Inout_ w_concurrent_queue* pQueue,
                                    _In_ const uint32_t pQueueCapacity,
                                    _In_ w_mem_pool pMemPool)
{
    apr_status_t _ret = apr_queue_create((apr_queue_t**)pQueue, pQueueCapacity, pMemPool);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT(false, "error happened, trace info: w_concurrent_queue_create\n");
    }
    return _ret;
}

W_RESULT  w_concurrent_queue_push(_In_ w_concurrent_queue pQueue, _In_ void* pData)
{
    apr_status_t _ret = apr_queue_push((apr_queue_t*)pQueue, pData);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened %d, trace info: w_concurrent_queue_push\n", _ret);
    }
    return _ret;
}

W_RESULT  w_concurrent_queue_pop(_In_ w_concurrent_queue pQueue, _Inout_ void** pData)
{
    apr_status_t _ret = apr_queue_pop((apr_queue_t*)pQueue, pData);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened %d, trace info: w_concurrent_queue_pop\n", _ret);
    }
    return _ret;
}

W_RESULT  w_concurrent_queue_trypush(_In_ w_concurrent_queue pQueue, _In_ void* pData)
{
    apr_status_t _ret = apr_queue_trypush((apr_queue_t*)pQueue, pData);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened %d, trace info: w_concurrent_queue_trypush\n", _ret);
    }
    return _ret;
}

W_RESULT  w_concurrent_queue_trypop(_In_ w_concurrent_queue pQueue, _Inout_ void** pData)
{
    apr_status_t _ret = apr_queue_trypop((apr_queue_t*)pQueue, pData);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened %d, trace info: w_concurrent_queue_trypop\n", _ret);
    }
    return _ret;
}

uint32_t  w_concurrent_queue_size(_In_ w_concurrent_queue pQueue)
{
    return apr_queue_size((apr_queue_t*)pQueue);
}

W_RESULT  w_concurrent_queue_interrupt_all(_In_ w_concurrent_queue pQueue)
{
    apr_status_t _ret = apr_queue_interrupt_all((apr_queue_t*)pQueue);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened %d, trace info: w_concurrent_queue_interrupt_all\n", _ret);
    }
    return _ret;
}

W_RESULT  w_concurrent_queue_term(_In_ w_concurrent_queue pQueue)
{
    apr_status_t _ret = apr_queue_term((apr_queue_t*)pQueue);
    if (_ret != APR_SUCCESS)
    {
        W_ASSERT_P(false, "error happened %d, trace info: w_concurrent_queue_term\n", _ret);
    }
    return _ret;
}
