#include "w_mutex.h"
#include <apr-1/apr_thread_mutex.h>
#include "log/w_log.h"

W_RESULT    w_mutex_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_mutex* pMutex,
    _In_ uint32_t pFlags)
{
    const char* _trace_info = "w_thread_mutex_init";
    if (pMemPool)
    {
        apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
        if (_pool)
        {
            return apr_thread_mutex_create(pMutex, pFlags, _pool);
        }
    }
    W_ASSERT_P(false, "bad args. trace info %s", _trace_info);
    return APR_BADARG;
}
 
W_RESULT    w_mutex_lock(_In_ w_mutex pMutex)
{
    if (!pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_lock");
        return W_FAILURE;
    }
    return apr_thread_mutex_lock(pMutex);
}
 
W_RESULT    w_mutex_trylock(_In_ w_mutex pMutex)
{
    if (!pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_trylock");
        return W_FAILURE;
    }
    return apr_thread_mutex_trylock(pMutex);
}
 
W_RESULT    w_mutex_unlock(_In_ w_mutex pMutex)
{
    if (!pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_unlock");
        return W_FAILURE;
    }
     return apr_thread_mutex_unlock(pMutex);
}
 
W_RESULT    w_mutex_fini(_In_ w_mutex* pMutex)
{
    if (!pMutex || !*pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_destroy");
        return W_FAILURE;
    }

    apr_status_t _ret = apr_thread_mutex_destroy(*pMutex);
    if (_ret == APR_SUCCESS)
    {
        *pMutex = NULL;
    }
    return _ret;
}
 
w_mem_pool  w_mutex_get_mem_pool(_In_ w_mutex pMutex)
{
    if (!pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_pool_get");
        return NULL;
    }
    return (w_mem_pool)apr_thread_mutex_pool_get(pMutex);
}
