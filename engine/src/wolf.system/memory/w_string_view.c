#include "w_string_view.h"

w_string_view w_string_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_string_view* pStringView,
    _In_ const char* pData)
{
    if (!pMemPool)
    {
        return NULL;
    }

    *pStringView = NULL;
    size_t _len = strlen(pData);
    if (_len)
    {
        *pStringView = w_malloc(pMemPool, sizeof(w_string_view_t));
        if (*pStringView)
        {
            size_t _size = _len + 1;
            (*pStringView)->data = w_malloc(pMemPool, _size); //with NULL
            strcpy_s((*pStringView)->data, _len, pData);
            (*pStringView)->data[_len] = '\0';
            (*pStringView)->str_len = _len;
            (*pStringView)->reserved_size = _size;
        }
    }
    return (*pStringView);
}

w_string_view w_string_dup(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_string_view* pDst,
    _Inout_ w_string_view pSrc)
{
    if (!pMemPool || !pSrc)
    {
        return NULL;
    }

    size_t _len = pSrc->str_len;
    if (!_len)
    {
        return NULL;
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
            return NULL;
        }
    }
    else
    {
        //allocate memory for string view
        *pDst = (w_string_view)w_malloc(pMemPool, sizeof(w_string_view_t));
        if (!(*pDst))
        {
            return NULL;
        }
    }

    //use the same memory
    if (_need_to_allocate_memory)
    {
        (*pDst)->data = w_malloc(pMemPool, _len + 1); //with NULL
        (*pDst)->reserved_size = _req_size;
    }
    strcpy_s((*pDst)->data, _len, pSrc->data);
    (*pDst)->data[_len] = '\0';
    (*pDst)->str_len = _len;
    
    return (*pDst);
}

W_RESULT w_string_is_empty(_In_ w_string_view pStr)
{
    return (pStr->str_len == 0 || pStr->data == NULL || pStr->data[0] == '\0') ? W_SUCCESS : W_FAILURE;
}

W_RESULT w_string_clear(_Inout_ w_string_view* pStr)
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