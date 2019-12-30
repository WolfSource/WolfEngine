#include "w_string.h"
#include <apr-1/apr_strings.h>
#include <stdarg.h>

char* w_string_create(_In_ const size_t pSize)
{
    apr_pool_t* _pool = w_get_default_memory_pool();
    if(!_pool)
    {
        W_ASSERT(false, "could not get default memory. trace info: w_string_create");
        return NULL;
    }
    
    char* _dst_str = apr_palloc(_pool, pSize);
    return _dst_str;
}

char* w_string_init_with_string(_In_ const char* pSrcString)
{
    size_t _size = strlen(pSrcString) + 1;
    char* _dst = w_string_create(_size);
    apr_cpystrn(_dst, pSrcString, _size);
    _dst[_size] = '\0';
    return _dst;
}

char* w_string_concat(int pNumberOfArgs, ...)
{
    char* _str = "";
    if (pNumberOfArgs == 0) return _str;
    
    apr_pool_t* _pool = w_get_default_memory_pool();
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


