#include "w_async.h"
#include "apr-1/apr_general.h"
#include "log/w_log.h"

void* _thread_job(w_thread pThread, void* pArgs)
{
    if (!pArgs)
    {
        W_ASSERT(false, "pArgs is NULL!. trace info: async::thread_job");
        return NULL;
    }
    
    struct ev_loop* _loop = (struct ev_loop*)pArgs;
    if (!_loop)
    {
        W_ASSERT(false, "pArgs is not ev_loop* !. trace info: async::thread_job");
        return NULL;
    }
    
    ev_loop(_loop, 0);
    return NULL;
}

W_RESULT w_async_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_async* pAsync, 
    _In_ w_async_callback pAsyncCallBack)
{
    const char* _trace_info = "w_condition_variable_init";
    if (!pMemPool || !pAsync)
    {
        W_ASSERT_P(false, "bad args! trace info %s", _trace_info);
        return APR_BADARG;
    }

    *pAsync = NULL;
    w_async _async = (w_async_t*)w_malloc(pMemPool, sizeof(w_async_t));
    if (!_async)
    {
        W_ASSERT_P(false, "bad args! trace info %s", _trace_info);
        return APR_BADARG;
    }
    *pAsync = _async;

    _async->a = (w_async_base*)w_malloc(pMemPool, sizeof(w_async_base));
    if (!_async->a)
    {
        return W_FAILURE;
    }

    // This loop sits in the thread
    _async->l = ev_default_loop(0);
    ev_async_init(_async->a, pAsyncCallBack);
    if (w_thread_init(pMemPool, &_async->t, &_thread_job, (void*)_async->l) == W_SUCCESS)
    {
        ev_async_start(_async->l, _async->a);
        return W_SUCCESS;
    }
    return W_FAILURE;
}

W_RESULT w_async_send(_In_ w_async pAsync, _In_opt_ void* pArg)
{
    if (!pAsync || !pAsync->l || !pAsync->a)
    {
        W_ASSERT(false, "pAsync is NULL!. trace info: w_async_send");
        return W_BAD_ARG;
    }
   
    if (ev_async_pending(pAsync->a))
    {
        //W_ASSERT(false, "pAsync is pending!. trace info: w_async_send");
        return W_FAILURE;
    }
    
    if (pArg)
    {
        pAsync->a->data = pArg;
    }
    ev_async_send(pAsync->l, pAsync->a);

    return W_SUCCESS;
}

W_RESULT w_async_start(_In_ w_async pAsync)
{
    if (!pAsync || !pAsync->l || !pAsync->a)
    {
        W_ASSERT(false, "pAsync is NULL!. trace info: w_async_send");
        return W_BAD_ARG;
    }

    ev_async_start(pAsync->l, pAsync->a);
    return W_SUCCESS;
}

W_RESULT w_async_stop(_In_ w_async pAsync)
{
    if (!pAsync || !pAsync->l || !pAsync->a)
    {
        W_ASSERT(false, "pAsync is NULL!. trace info: w_async_send");
        return W_BAD_ARG;
    }

    ev_async_stop(pAsync->l, pAsync->a);
    return W_SUCCESS;
}
