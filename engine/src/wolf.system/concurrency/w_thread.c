#include "w_thread.h"
#include <apr-1/apr_thread_proc.h>
#include <apr-1/apr_portable.h>

W_RESULT w_thread_init_once_flag(_Inout_ w_thread_once_flag pOnceFlag)
{
    //get default thread pool
    w_mem_pool _pool = w_get_default_memory_pool();
    if (!_pool) return W_FAILURE;
    
    //create once flag
    apr_status_t _status = apr_thread_once_init(&pOnceFlag, _pool);
    return _status == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

W_RESULT w_thread_once_call(_Inout_ w_thread_once_flag pOnceFlag, _In_ w_thread_once_job pOnceJob)
{    
    //create once flag
    apr_status_t _status = apr_thread_once(pOnceFlag, pOnceJob);
    return _status == APR_SUCCESS ? W_SUCCESS : W_FAILURE;
}

W_RESULT w_thread_create(_Inout_ w_thread     pThread,
                         _In_    w_thread_job pJob,
                         _In_    void*        pJobArgs)
{
    //get default thread pool
    w_mem_pool _pool = w_get_default_memory_pool();
    if (!_pool) return W_FAILURE;
    
    //create default attribute thread
    apr_threadattr_t* _attr;
    apr_threadattr_create(&_attr, _pool);
    
    //create thread
    apr_thread_create(&pThread, _attr, pJob, pJobArgs, _pool);
    
    return W_SUCCESS;
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

void w_thread_terminate(_Inout_ w_thread pThread)
{
    //terminate thread
    apr_status_t _status = APR_SUCCESS;
    apr_thread_exit(pThread, _status);
}

void w_thread_terminate_with_status(_Inout_ w_thread pThread, _In_ int pExitStatus)
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
    return (w_thread_id)apr_os_thread_current();
}

W_RESULT w_thread_current_ids_are_equal(_In_ w_thread_id pThread1, _In_ w_thread_id pThread2)
{
    return (apr_os_thread_equal((apr_os_thread_t)pThread1, (apr_os_thread_t)pThread2) != 0) ? W_SUCCESS : W_FAILURE;
}

void w_thread_current_sleep_for_nanoseconds(_In_ double pTime)
{
	apr_sleep(pTime * 1e-3);
}

void w_thread_current_sleep_for_microseconds(_In_ double pTime)
{
	apr_sleep(pTime);
}

void w_thread_current_sleep_for_milliseconds(_In_ double pTime)
{
	apr_sleep(pTime * 1e+3);
}

void w_thread_current_sleep_for_seconds(_In_ double pTime)
{
	apr_sleep(pTime * 1e+6);
}

void w_thread_get_number_of_cpu_threads(_Inout_ int* pCores,
                                        _Inout_ int* pThreads,
                                        _Inout_ int* pActualThreads)
{
#ifdef _WIN32
	SYSTEM_INFO _sys_info;
	GetSystemInfo(&_sys_info);
	*pCores = (int)_sys_info.dwNumberOfProcessors;
	*pThreads = 2;
	*pActualThreads = (*pCores) * (*pThreads);
#else
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
#endif
}


W_RESULT    w_thread_mutex_create(_Inout_ w_mutex* pMutex,
                                  _In_ uint32_t pFlags,
                                  _In_ w_mem_pool pMemPool)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_get_default_memory_pool();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_thread_mutex_create");
            return W_FAILURE;
        }
    }
    
    return apr_thread_mutex_create(pMutex, pFlags, _pool);
}
 
W_RESULT    w_thread_mutex_lock(_In_ w_mutex pMutex)
{
    if (!pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_lock");
        return W_FAILURE;
    }
    return apr_thread_mutex_lock(pMutex);
}
 
W_RESULT    w_thread_mutex_trylock(_In_ w_mutex pMutex)
{
    if (!pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_trylock");
        return W_FAILURE;
    }
    return apr_thread_mutex_trylock(pMutex);
}
 
W_RESULT    w_thread_mutex_unlock(_In_ w_mutex pMutex)
{
    if (!pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_unlock");
        return W_FAILURE;
    }
     return apr_thread_mutex_unlock(pMutex);
}
 
W_RESULT    w_thread_mutex_destroy(_In_ w_mutex pMutex)
{
    if (!pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_destroy");
        return W_FAILURE;
    }
    return apr_thread_mutex_destroy(pMutex);
}
 
w_mem_pool  w_thread_mutex_get_mem_pool(_In_ w_mutex pMutex)
{
    if (!pMutex)
    {
        W_ASSERT(false, "mutex is NULL!. trace info: w_thread_mutex_pool_get");
        return NULL;
    }
    return (w_mem_pool)apr_thread_mutex_pool_get(pMutex);
}
