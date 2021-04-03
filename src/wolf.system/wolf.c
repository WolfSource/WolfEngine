#include "wolf.h"
#include <w_log/w_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <limits.h>
//#include <concurrency/libev/ev.h>

//http://dev.ariel-networks.com/apr/apr-tutorial/html/apr-tutorial.html#toc1
#include <apr.h>
#include <apr_strings.h>

#ifdef WOLF_ENABLE_CURL
#include <curl/curl.h>
#endif

// this is used to cache lengths in apr_pwstrcat
#define MAX_SAVED_LENGTHS  6

#ifdef W_PLATFORM_WIN
BOOL APIENTRY DllMain(_In_ HMODULE phModule, _In_ DWORD phl_reason_for_call, _In_ LPVOID plpReserved)
{
    switch (phl_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif

//static struct ev_loop* s_main_loop = NULL;

W_RESULT wolf_init()
{
    int a = 0;
    if (apr_initialize()
        // initialize curl
#ifdef WOLF_ENABLE_CURL
        || curl_global_init(CURL_GLOBAL_ALL)
#endif
        )
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
    res = (char*)w_malloc(pMemPool, len + 1);
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
    if (w_mem_pool_init(&_local_pool) != W_SUCCESS)
    {
        W_ASSERT_P(false, 
            "could not allocate local memory pool. trace info: %s", 
            _trace_info);
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
#ifdef W_PLATFORM_WIN
            wcstombs_s(NULL, _src, _len, _w, _w_len);
#elif defined W_PLATFORM_OSX
            _len = wcstombs ( _src, _w, _w_len );
#endif
            _dst = w_strcat(pMemPool, _dst, _src, NULL);
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
            "could not get allocate memory for wchar*. trace info: %s", 
            _trace_info);
        return NULL;
    }
    mbstowcs(_ret, _dst, _ret_len);
    _ret[_ret_len] = L'\0';

out:
    //terminate local memory pool
    w_mem_pool_fini(&_local_pool);
    return _ret;
}

//void wolf_run(int pFlags)
//{
//    ev_run(EV_DEFAULT, pFlags);
//}

char* w_strerror(_In_ W_RESULT pErrorCode, _Inout_z_  char* pBuffer, _In_ size_t pBufferSize)
{
    return apr_strerror((apr_status_t)pErrorCode, pBuffer, pBufferSize);
}

void wolf_fini()
{
    //ev_break(s_main_loop, EVBREAK_ALL);
#ifdef WOLF_ENABLE_CURL
    curl_global_cleanup();
#endif
    apr_terminate();
}
