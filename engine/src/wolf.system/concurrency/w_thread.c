#include "w_thread.h"
#include <apr-1/apr_thread_proc.h>
#include <apr-1/apr_portable.h>
#include "log/w_log.h"

W_RESULT w_thread_init_once_flag(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_thread_once_flag* pOnceFlag)
{
    const char* _trace_info = "w_thread_init_once_flag";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "bad args. trace info %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_thread_once_init(&(*pOnceFlag), _pool);
    }
    return W_FAILURE;
}

W_RESULT w_thread_once_call(_Inout_ w_thread_once_flag pOnceFlag, _In_ w_thread_once_job pOnceJob)
{
    if (!pOnceFlag || !pOnceJob)
    {
        return APR_BADARG;
    }
    //create once flag
    return apr_thread_once(pOnceFlag, pOnceJob);
}

W_RESULT w_thread_init(
    _Inout_  w_mem_pool   pMemPool,
    _Inout_  w_thread*    pThread,
    _In_     w_thread_job pJob,
    _In_opt_ void*        pJobArgs)
{
    const char* _trace_info = "w_thread_init";
    if (pMemPool)
    {
        apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
        if (_pool)
        {
            //create default attribute thread
            apr_threadattr_t* _attr;
            apr_threadattr_create(&_attr, _pool);

            //create thread
            return apr_thread_create(pThread, _attr, pJob, pJobArgs, _pool);
        }
    }
    W_ASSERT_P(false, "bad args. trace info %s", _trace_info);
    return APR_BADARG;
}

W_RESULT w_thread_join(_Inout_ w_thread pThread)
{
    //join thread
    apr_status_t _status;
    apr_thread_join(&_status, pThread);
    return _status == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

void w_thread_detach(_Inout_ w_thread pThread)
{
    //join thread
    apr_thread_detach(pThread);
}

void w_thread_exit(_Inout_ w_thread pThread)
{
    //terminate thread
    apr_status_t _status = APR_SUCCESS;
    apr_thread_exit(pThread, _status);
}

void w_thread_exit_with_status(_Inout_ w_thread pThread, _In_ int pExitStatus)
{
    //terminate thread
    apr_status_t _status = (apr_status_t)pExitStatus;
    apr_thread_exit(pThread, _status);
}

w_thread w_thread_get_current(void)
{
    w_thread _current_thread = NULL;
    apr_os_thread_t _current_os_thread = apr_os_thread_current();
    apr_os_thread_t* _ptr_to_os_thread = &_current_os_thread;
    apr_status_t _ret = apr_os_thread_get(&_ptr_to_os_thread,_current_thread);
    return _ret == APR_SUCCESS ? _current_thread : NULL;
}

w_thread_id w_thread_get_current_id(void)
{
    apr_os_thread_t _current_os_thread = apr_os_thread_current();

    apr_os_thread_t* _dst = NULL;
    //allocate memory for it
    w_mem_pool _mem_pool = NULL;
    w_mem_pool_init(&_mem_pool);
    if (_mem_pool)
    {
        size_t _size = sizeof(apr_os_thread_t);
        _dst = (apr_os_thread_t*)w_malloc(_mem_pool, _size);
        if (_dst)
        {
            memcpy(_dst, &_current_os_thread, _size);
        }
    }
    w_mem_pool_fini(&_mem_pool);
    return (void*)_dst;
}

W_RESULT w_thread_current_ids_are_equal(_In_ w_thread_id pThread1, _In_ w_thread_id pThread2)
{
    apr_os_thread_t* _t1 = (apr_os_thread_t*)pThread1;
    apr_os_thread_t* _t2 = (apr_os_thread_t*)pThread2;

    return (apr_os_thread_equal(*_t1, *_t2) != 0) ? W_SUCCESS : W_FAILURE;
}

void w_thread_current_sleep_for_nanoseconds(_In_ double pTime)
{
	apr_sleep((apr_interval_time_t)(pTime * 1e-3));
}

void w_thread_current_sleep_for_microseconds(_In_ double pTime)
{
	apr_sleep((apr_interval_time_t)(pTime));
}

void w_thread_current_sleep_for_milliseconds(_In_ double pTime)
{
	apr_sleep((apr_interval_time_t)(pTime * 1e+3));
}

void w_thread_current_sleep_for_seconds(_In_ double pTime)
{
	apr_sleep((apr_interval_time_t)(pTime * 1e+6));
}

#if !defined(W_PLATFORM_ANDROID) && !defined(W_PLATFORM_IOS)
void w_thread_get_number_of_cpu_threads(_Inout_ int* pCores,
                                        _Inout_ int* pThreads,
                                        _Inout_ int* pActualThreads)
{
#ifdef W_PLATFORM_WIN
	SYSTEM_INFO _sys_info;
	GetSystemInfo(&_sys_info);
	*pCores = (int)_sys_info.dwNumberOfProcessors;
	*pThreads = 2;
	*pActualThreads = (*pCores) * (*pThreads);
#elif defined(W_PLATFORM_OSX) || defined(W_PLATFORM_LINUX)
    /*
    unsigned int _a = 11, _b = 0, _c = 1, _d = 0;

    ASM volatile("cpuid"
            : "=a" (_a),
              "=b" (_b),
              "=c" (_c),
              "=d" (_d)
            : "0" (_a), "2" (_c)
            : );
    *pCores = _a;
    *pThreads = _b;
    *pActualThreads = _d;
     */
#endif
}
#endif
