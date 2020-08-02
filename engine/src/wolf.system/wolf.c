#include "wolf.h"

//http://dev.ariel-networks.com/apr/apr-tutorial/html/apr-tutorial.html#toc1
#include <apr.h>
#include <apr-1/apr_strings.h>

#include <curl/curl.h>

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

w_mem_pool w_get_default_memory_pool(void)
{
    return s_default_memory_pool;
}

w_mem_pool w_create_memory_pool(void)
{
    apr_pool_t* _pool;
    apr_pool_create(&_pool, NULL);
    return (w_mem_pool)_pool;
}

void* w_malloc(_In_ const size_t pMemSize, _In_z_ const char* pTraceInfo)
{
    w_mem_pool _pool = w_get_default_memory_pool();
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

void w_free(_In_ const void* pMem)
{
    if (!pMem)
    {
        return;
    }
    w_mem_pool _pool = w_get_default_memory_pool();
    if(!_pool)
    {
        char _buf[W_MAX_BUFFER_SIZE];
        apr_snprintf(_buf, W_MAX_BUFFER_SIZE,
                     "could not get default memory. trace info: w_free\n");
        W_ASSERT_P(false, "%s", _buf);
        return;
    }
    apr_pool_cleanup_register(_pool, pMem, _plain_cleanup, _child_cleanup);
}

char* w_string(_In_ const char* pSource)
{
    w_mem_pool _pool = w_get_default_memory_pool();
    if(!_pool)
    {
        W_ASSERT(false, "could not get default memory. trace info: w_string_create");
        return NULL;
    }
        
    size_t _size = strlen(pSource) + 1;
    char* _dst = apr_palloc(_pool, _size);
    apr_cpystrn(_dst, pSource, _size);
    _dst[_size] = '\0';
    return _dst;
}

char* w_string_concat(int pNumberOfArgs, ...)
{
    char* _str = "";
    if (pNumberOfArgs == 0) return _str;
    
    w_mem_pool _pool = w_get_default_memory_pool();
    if(!_pool)
    {
        W_ASSERT(false, "could not get default memory. trace info: w_string_create");
        return NULL;
    }
    
    va_list _arg_list;
    va_start(_arg_list, pNumberOfArgs);
    
    if (pNumberOfArgs == 1)
    {
        _str = (char*)va_arg(_arg_list, char*);
    }
    else
    {
        int i = 0;
        do
        {
            char* _s1 = va_arg(_arg_list, char*);
            _str = apr_pstrcat(_pool, _str, _s1, NULL);
            i++;
        } while (i < pNumberOfArgs);
        va_end(_arg_list);
    }
    
    return _str;
}

void wolf_terminate()
{
    curl_global_cleanup();
    apr_terminate();
}
