#include "wolf.h"

//http://dev.ariel-networks.com/apr/apr-tutorial/html/apr-tutorial.html#toc1
#include <apr.h>
#include <apr-1/apr_pools.h>
#include <apr-1/apr_strings.h>

#include <curl/curl.h>

// this is used to cache lengths in apr_pwstrcat
#define MAX_SAVED_LENGTHS  6

static w_mem_pool s_default_memory_pool = NULL;

W_RESULT wolf_initialize()
{
    apr_status_t _ret = apr_initialize();
    if(_ret != APR_SUCCESS)
    {
        return _ret;
    }
    
    //create default memory pool
    apr_pool_create(&s_default_memory_pool, NULL);
    if (!s_default_memory_pool) return W_FAILURE;
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    return W_SUCCESS;
}

w_mem_pool w_mem_pool_get_default(void)
{
    return s_default_memory_pool;
}

w_mem_pool w_mem_pool_create(void)
{
    apr_pool_t* _pool;
    apr_pool_create(&_pool, NULL);
    return (w_mem_pool)_pool;
}

void w_mem_pool_terminate(_In_ w_mem_pool pMemPool)
{
    if (pMemPool)
    {
        apr_pool_destroy(pMemPool);
    }
}

void* w_malloc(_In_ const size_t pMemSize, _In_z_ const char* pTraceInfo)
{
    w_mem_pool _pool = w_mem_pool_get_default();
    if(!_pool)
    {
        char _buf[W_MAX_BUFFER_SIZE];
        apr_snprintf(_buf, W_MAX_BUFFER_SIZE,
                     "could not get default memory. trace info: w_malloc from %s\n", pTraceInfo);
        W_ASSERT_P(false, "%s", _buf);
        return NULL;
    }
     
    return apr_palloc(_pool, pMemSize);
}

static apr_status_t _plain_cleanup(void* p)
{
    fprintf(stderr,"plain clean up: pid=%d\n",(int)getpid());
    return APR_SUCCESS;
}

static apr_status_t _child_cleanup(void* p)
{
    fprintf(stderr,"child clean up: pid=%d\n",(int)getpid());
    return APR_SUCCESS;
}

void w_free(_In_ const void* pMemory)
{
    if (!pMemory)
    {
        return;
    }
    w_mem_pool _pool = w_mem_pool_get_default();
    if(!_pool)
    {
        W_ASSERT(false, "could not get default memory. trace info: w_free");
        return;
    }
    apr_pool_cleanup_register(_pool, pMemory, _plain_cleanup, _child_cleanup);
}

char* w_strcat(_In_ w_mem_pool pMemPool, ...)
{
    w_mem_pool _pool;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        _pool = w_mem_pool_get_default();
    }

    char* cp, * argp, * res;
    apr_size_t saved_lengths[MAX_SAVED_LENGTHS];
    int nargs = 0;

    /* Pass one --- find length of required string */

    apr_size_t len = 0;
    va_list adummy;

    va_start(adummy, pMemPool);

    while ((cp = va_arg(adummy, char*)) != NULL) 
    {
        apr_size_t cplen = strlen(cp);
        if (nargs < MAX_SAVED_LENGTHS) 
        {
            saved_lengths[nargs++] = cplen;
        }
        len += cplen;
    }

    va_end(adummy);

    /* Allocate the required string */

    res = (char*)apr_palloc(_pool, len + 1);
    cp = res;

    /* Pass two --- copy the argument strings into the result space */

    va_start(adummy, pMemPool);

    nargs = 0;
    while ((argp = va_arg(adummy, char*)) != NULL) 
    {
        if (nargs < MAX_SAVED_LENGTHS) 
        {
            len = saved_lengths[nargs++];
        }
        else {
            len = strlen(argp);
        }

        memcpy(cp, argp, len);
        cp += len;
    }

    va_end(adummy);

    /* Return the result string */

    *cp = '\0';

    return res;
}

wchar_t* w_wstrcat(w_mem_pool* pMemPool, ...)
{
    const char* _trace_info = "w_wstrcat";
    char* _dst = "";

    wchar_t* _w;
    char* _src;
    size_t _w_len, _len, _ret_len = 0;
    bool _got_error = false;

    va_list _args;
    va_start(_args, pMemPool);
    while ((_w = va_arg(_args, wchar_t*)) != NULL)
    {
        _w_len = wcslen(_w);
        _ret_len += _w_len;
        if (_w_len)
        {
            _len = _w_len * 2;
            _src = w_malloc(_len, _trace_info);
            if (!_src)
            {
                _got_error = true;
                break;
            }
            wcstombs_s(NULL, _src, _len, _w, _w_len);
            _dst = w_strcat(pMemPool, _dst, _src);
            w_free(_src);
        }
    }
    va_end(_args);

    if (_got_error)
    {
        return NULL;
    }

    //convert char* to wchar_t*
    wchar_t* _ret = w_malloc(sizeof(wchar_t) * _ret_len + 1, _trace_info);
    if (!_ret)
    {
        W_ASSERT_P(
            false,
            "could not get allocate memory for wchar*. trace info: %s", _trace_info);
        return NULL;
    }
    mbstowcs(_ret, _dst, _ret_len);
    _ret[_ret_len] = L'\0';

    return _ret;
}

void wolf_terminate()
{
    curl_global_cleanup();
    apr_terminate();
}
