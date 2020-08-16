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

W_RESULT w_async_init(_Inout_ w_async* pAsync, _In_ w_async_callback pAsyncCallBack)
{
    if (!pAsync)
    {
        return W_FAILURE;
    }
    pAsync->a = (w_async_base*)w_malloc(sizeof(w_async_base), "w_async_init");
    if (!pAsync->a)
    {
        return W_FAILURE;
    }

    // This loop sits in the thread
    pAsync->l = ev_loop_new(0);
    ev_async_init(pAsync->a, pAsyncCallBack);
    w_thread_create(&pAsync->t, &_thread_job, (void*)pAsync->l);
    ev_async_start(pAsync->l, pAsync->a);

    return W_SUCCESS;
}

W_RESULT w_async_send(_In_ w_async* pAsync, _In_ void* pArg)
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
    
    if (pArg)
    {
        pAsync->a->data = pArg;
    }
    ev_async_send(pAsync->l, pAsync->a);
    
    return W_SUCCESS;
}
