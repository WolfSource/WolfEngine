#include "w_array.h"
#include <apr-1/apr_tables.h>
#include <apr-1/apr_strings.h>
#include "log/w_log.h"

w_array w_array_init(
    _Inout_ w_mem_pool pMemPool,
    _In_ int pInitSize,
    _In_ int pSizeOfEachElement)
{
    const char* _trace_info = "w_array_init";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return NULL;
    }

    if (pInitSize < 0 || pSizeOfEachElement < 0)
    {
        W_ASSERT_P(false,
            "bad arguments. trace info: %s",
            _trace_info);
        return NULL;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return (w_array)apr_array_make(
            _pool,
            pInitSize,
            pSizeOfEachElement);
    }
    return NULL;
}

size_t w_array_get_size(_In_ w_array pArray)
{
    return pArray ? pArray->nelts : 0;
}

const void* w_array_get_element(_Inout_ w_array pArray, _In_ int pElementIndex)
{
    if (!pArray || pElementIndex >= pArray->nelts)
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

void* w_array_pop(_Inout_ w_array pArray)
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
    //const apr_array_header_t* _header = apr_table_elts((const )pArray);
    return apr_is_empty_array(pArray);
}

void w_array_clear(_Inout_ w_array pArray)
{
    if (!pArray)
    {
        return;
    }
    apr_array_clear(pArray);
}
