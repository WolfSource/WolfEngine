#include "wolf.h"
#include "memory/rpmalloc/rpmalloc.h"

//http://dev.ariel-networks.com/apr/apr-tutorial/html/apr-tutorial.html#toc1
#include <apr-2/apr.h>
#include <apr-2/apr_strings.h>

#include <curl/curl.h>

// this is used to cache lengths in apr_pwstrcat
#define MAX_SAVED_LENGTHS  6

W_RESULT wolf_init()
{
    rpmalloc_config_t _config = { 0 };

    //enable huge page size just for WINDOWS, MAC and LINUX
#if defined(W_PLATFORM_WIN) || defined(W_PLATFORM_OSX) || defined(W_PLATFORM_LINUX)
    _config.enable_huge_pages = 1;
#endif

    if (apr_initialize() ||
        curl_global_init(CURL_GLOBAL_ALL) ||
        rpmalloc_initialize_config(&_config))
    {
        return W_FAILURE;
    }
    return W_SUCCESS;
}

char* w_strcat(_In_ w_mem_pool pMemPool, ...)
{
    const char* _trace_info = "w_strcat";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }

    char* cp, * argp, * res;
    apr_size_t saved_lengths[MAX_SAVED_LENGTHS];
    int nargs = 0;

    //pass one, find length of required string

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

    //allocate the required string
    w_apr_pool _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (!_pool)
    {
        W_ASSERT_P(false, "missing apr memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    res = (char*)apr_palloc(_pool, len + 1);
    cp = res;

    //pass two, copy the argument strings into the result space
    va_start(adummy, pMemPool);

    nargs = 0;
    while ((argp = va_arg(adummy, char*)) != NULL) 
    {
        if (nargs < MAX_SAVED_LENGTHS) 
        {
            len = saved_lengths[nargs++];
        }
        else 
        {
            len = strlen(argp);
        }

        memcpy(cp, argp, len);
        cp += len;
    }

    va_end(adummy);

    //return the result string
    *cp = '\0';

    return res;
}

wchar_t* w_wstrcat(_In_ w_mem_pool pMemPool, ...)
{
    const char* _trace_info = "w_wstrcat";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }

    char* _dst = "";

    wchar_t* _w;
    char* _src;
    size_t _w_len, _len, _ret_len = 0;
    bool _got_error = false;

    w_mem_pool _local_pool = NULL;
    if (w_mem_pool_init(&_local_pool, W_MEM_POOL_FAST_EXTEND) != W_SUCCESS)
    {
        W_ASSERT_P(false, "could not allocate local memory pool. trace info: %s", _trace_info);
        return NULL;
    }

    //iterate over args
    va_list _args;
    va_start(_args, pMemPool);
    while ((_w = va_arg(_args, wchar_t*)) != NULL)
    {
        _w_len = wcslen(_w);
        _ret_len += _w_len;
        if (_w_len)
        {
            _len = _w_len * 2;
            _src = w_malloc(_local_pool, _len);
            if (!_src)
            {
                _got_error = true;
                break;
            }
            wcstombs_s(NULL, _src, _len, _w, _w_len);
            _dst = w_strcat(pMemPool, _dst, _src);
        }
    }
    va_end(_args);

    wchar_t* _ret = NULL;
    if (_got_error)
    {
        goto out;
    }

    //convert char* to wchar_t*
    _ret = w_malloc(pMemPool, sizeof(wchar_t) * _ret_len + 1);
    if (!_ret)
    {
        W_ASSERT_P(
            false,
            "could not get allocate memory for wchar*. trace info: %s", _trace_info);
        return NULL;
    }
    mbstowcs(_ret, _dst, _ret_len);
    _ret[_ret_len] = L'\0';

out:
    //terminate local memory pool
    w_mem_pool_fini(&_local_pool);
    return _ret;
}

void wolf_fini()
{
    curl_global_cleanup();
    rpmalloc_finalize();
    apr_terminate();
}
