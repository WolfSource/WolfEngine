#include "w_timer.h"
#include <concurrency/w_thread.h>
#include <apr-1/apr_thread_proc.h>

typedef struct
{
    double              sleep_before_thread_in_ms;
    w_thread*           thread;
    w_job               callback;
    void*               arg;
} timer_job;

//static w_job* s_jobs;
static void async_job(_In_ void* pJob)
{
    timer_job* _job = (timer_job*)pJob;
    if (!_job) return;
    
    w_thread_current_sleep_for_microseconds(_job->sleep_before_thread_in_ms);
    (void)_job->callback(_job->arg);
    
    free(_job);
}

void timer_sync_with_callback(_In_ const w_timespan* const pInterval,
                              _In_ w_job pCallBack,
                              _In_ void* pArg)
{
    double _ms = w_timespan_get_milliseconds(pInterval);
    w_thread_current_sleep_for_milliseconds(_ms);
    (void)pCallBack(pArg);
}

void timer_async_with_callback(_In_ const w_timespan* const pInterval,
                               _In_ w_job pCallBack,
                               _In_ void* pArg)
{
    apr_pool_t* _pool = w_get_default_memory_pool();
    if(!_pool)
    {
        W_ASSERT(false, "could not get default memory. trace info: timer_async_with_callback");
        return;
    }
        
    timer_job* _job = (timer_job*)apr_palloc(_pool, sizeof(timer_job));
    _job->thread = NULL;
    _job->sleep_before_thread_in_ms = w_timespan_get_milliseconds(pInterval);
    _job->callback = pCallBack;
    _job->arg = pArg;
    
    w_thread_create(_job->thread, &async_job, (void*)_job);
    w_thread_join(_job->thread);
}

