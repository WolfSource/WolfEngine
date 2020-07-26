#include "w_async.h"
#include "apr-1/apr_general.h"

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

W_RESULT w_async_init(_In_ w_async** pAsync, _In_ w_async_callback pAsyncCallBack)
{
    w_async* _async = w_malloc(sizeof(w_async), "w_async_init");
    if (_async)
    {
        _async->a = (w_async_base*)w_malloc(sizeof(w_async_base), "w_async_init");
        if (!_async->a)
        {
            w_free(pAsync);
            return W_FAILURE;
        }

        // This loop sits in the thread
        _async->l = ev_loop_new(0);
        ev_async_init(_async->a, pAsyncCallBack);
        w_thread_create(&_async->t, &_thread_job, (void*)_async->l);

        *pAsync = _async;

        return W_SUCCESS;
    }
    return W_FAILURE;
}

W_RESULT w_async_start(_In_ w_async* pAsync)
{
    if (!pAsync || !pAsync->l || !pAsync->a)
    {
        W_ASSERT(false, "pAsync is NULL!. trace info: w_async_start");
        return W_FAILURE;
    }
    
    if (pAsync->l && pAsync->a)
    {
        ev_async_start(pAsync->l, pAsync->a);
    }
    
    return W_SUCCESS;
}

W_RESULT w_async_send(_In_ w_async* pAsync)
{
    if (!pAsync || !pAsync->l || !pAsync->a)
    {
        W_ASSERT(false, "pAsync is NULL!. trace info: w_async_send");
        return W_FAILURE;
    }
   
    if (ev_async_pending(pAsync->a))
    {
        W_ASSERT(false, "pAsync is pending!. trace info: w_async_send");
        return W_FAILURE;
    }
    
    ev_async_send(pAsync->l, pAsync->a);
    
    return W_SUCCESS;
}
