#include "w_mem_map.h"
#include <apr-1/apr_mmap.h>

W_RESULT    w_mem_map_create(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_mem_map* pNewMemMap,
    _In_ w_file pFile,
    _In_ w_offset pOffset,
    _In_ size_t pSize,
    _In_ int pFlag)
{
    const char* _trace_info = "w_mem_map_create";
    if (!pMemPool ||
        w_mem_pool_get_type(pMemPool) != W_MEM_POOL_FAST_EXTEND ||
        !pNewMemMap ||
        !*pNewMemMap)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_mmap_create(
            pNewMemMap,
            pFile,
            pOffset,
            pSize,
            pFlag,
            _pool);
    }
    return W_FAILURE;
}

W_RESULT    w_mem_map_dup(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_mem_map* pNewMemMap,
    _In_ w_mem_map pOldMemMap)
{
    const char* _trace_info = "w_mem_map_dup";
    if (!pMemPool ||
        w_mem_pool_get_type(pMemPool) != W_MEM_POOL_FAST_EXTEND ||
        !pNewMemMap ||
        !*pNewMemMap ||
        !pOldMemMap)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_mmap_dup(
            pNewMemMap,
            pOldMemMap,
            _pool);
    }
    return W_FAILURE;
}

W_RESULT     w_mem_map_delete(_In_ w_mem_map pMemoryMap)
{
    const char* _trace_info = "w_mem_map_delete";
    if (!pMemoryMap)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return W_FAILURE;
    }
    return apr_mmap_delete(pMemoryMap);
}

W_RESULT     w_mem_map_offset(
    _Inout_ void** pAddress,
    _In_ w_mem_map pMemoryMap,
    _In_ w_offset pOffset)
{
    const char* _trace_info = "w_mem_map_delete";
    if (!pAddress || !*pAddress || pMemoryMap)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return W_FAILURE;
    }
    return apr_mmap_offset(pAddress, pMemoryMap, pOffset);
}