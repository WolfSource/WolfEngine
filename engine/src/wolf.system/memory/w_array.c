#include "w_array.h"
#include <apr_tables.h>
#include <apr_strings.h>

w_array w_array_init(
    _In_ int pInitSize,
    _In_ int pSizeOfEachElement,
    _In_ w_mem_pool pMemPool)
{
    const char* _trace_info = "w_array_init";

    if (pInitSize < 0 || pSizeOfEachElement < 0)
    {
        W_ASSERT_P(false,
            "bad arguments. trace info: %s",
            _trace_info);
        return NULL;
    }

    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_mem_pool_get_default();
        if (!_pool)
        {
            W_ASSERT_P(false,
                "could not get default memory pool. trace info: %s",
                _trace_info);
            return NULL;
        }
    }

    return (w_array)apr_array_make(
        pMemPool,
        pInitSize,
        pSizeOfEachElement);
}

const void* w_array_get_element(_Inout_ w_array pArray, _In_ int pElementIndex)
{
    if (!pArray || !pElementIndex >= pArray->elt_size)
    {
        return NULL;
    }
    const void* _ret = ((const void**)pArray->elts)[pElementIndex];
    return _ret;
}

void* w_array_append(_Inout_ w_array pArray, _In_ void* pItem)
{
    if (!pArray)
    {
        return NULL;
    }
    return (*(void**)apr_array_push(pArray) = pItem);
}

void* w_array_remove(_Inout_ w_array pArray)
{
    if (!pArray)
    {
        return NULL;
    }
    return apr_array_pop(pArray);
}

int w_array_is_empty(_In_ w_array pArray)
{
    if (!pArray)
    {
        return 0;
    }
    const apr_array_header_t* _header = apr_table_elts(pArray);
    return apr_is_empty_array(_header);
}

void w_array_clear(_Inout_ w_array pArray)
{
    if (!pArray)
    {
        return;
    }
    apr_array_clear(pArray);
}
