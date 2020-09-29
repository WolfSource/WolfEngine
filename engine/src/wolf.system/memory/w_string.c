#include "w_string.h"
#include <log/w_log.h>
#include <ctype.h>

#ifdef W_PLATFORM_OSX
#include <string.h>
#endif

W_RESULT w_string_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_string* pStringView,
    _In_ const char* pData)
{
    const char* _trace_info = "w_string_init";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "bad args! trace info: %s", _trace_info);
        return W_BAD_ARG;
    }

    *pStringView = NULL;
    size_t _len = strlen(pData);
    if (_len)
    {
        *pStringView = w_malloc(pMemPool, sizeof(w_string_t));
        if (*pStringView)
        {
            size_t _size = _len + 1;
            (*pStringView)->data = w_malloc(pMemPool, _size); //with NULL
#ifdef W_PLATFORM_WIN
            strcpy_s((*pStringView)->data, _size, pData); 
#else
            strcpy((*pStringView)->data, pData);
#endif
            (*pStringView)->data[_len] = '\0';
            (*pStringView)->str_len = _len;
            (*pStringView)->reserved_size = _size;
            return W_SUCCESS;
        }
    }
    return W_FAILURE;
}

W_RESULT w_string_dup(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_string* pDst,
    _Inout_ w_string pSrc)
{
    const char* _trace_info = "w_string_dup";
    if (!pMemPool || !pSrc)
    {
        W_ASSERT_P(false, "bad args! trace info: %s", _trace_info);
        return W_BAD_ARG;
    }

    size_t _len = pSrc->str_len;
    if (!_len)
    {
        return W_FAILURE;
    }

    size_t _req_size = _len + 1;
    bool _need_to_allocate_memory = true;
    //allocate memory
    if (*pDst)
    {
        if ((*pDst)->reserved_size >= _req_size)
        {
            _need_to_allocate_memory = false;
        }
        else
        {
            //destination already contains memory, make sure free it first
            return W_FAILURE;
        }
    }
    else
    {
        //allocate memory for string view
        *pDst = (w_string)w_malloc(pMemPool, sizeof(w_string_t));
        if (!(*pDst))
        {
            return W_FAILURE;
        }
    }

    //use the same memory
    if (_need_to_allocate_memory)
    {
        (*pDst)->data = w_malloc(pMemPool, _len + 1); //with NULL
        (*pDst)->reserved_size = _req_size;
    }
#ifdef W_PLATFORM_WIN
    strcpy_s((*pDst)->data, _len, pSrc->data); 
#else
    strcpy((*pDst)->data, pSrc->data);
#endif
    (*pDst)->data[_len] = '\0';
    (*pDst)->str_len = _len;

    return W_SUCCESS;
}

W_RESULT w_string_is_empty(_In_ w_string pStr)
{
    return (pStr->str_len == 0 || pStr->data == NULL || pStr->data[0] == '\0') ? W_SUCCESS : W_FAILURE;
}

void w_string_to_lower(_Inout_z_ w_string pString)
{
    if (pString)
    {
        int _ret = 0;
        char* _c = pString->data;
        size_t _l = pString->str_len;
        while (_l)
        {
            *_c = tolower(*_c);
            _c++;
            _l--;
        }
    }
}

W_RESULT w_string_clear(_Inout_ w_string* pStr)
{
    if (!pStr)
    {
        return W_FAILURE;
    }

    if (*pStr)
    {
        (*pStr)->str_len = 0;
        (*pStr)->data[0] = '\0';
    }
    return W_SUCCESS;
}
