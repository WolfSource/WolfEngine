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
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    return apr_mmap_create(
        pNewMemMap, 
        pFile, 
        pOffset, 
        pSize, 
        pFlag, 
        pMemPool);
}

W_RESULT    w_mem_map_dup(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_mem_map* pNewMemMap,
    _In_ w_mem_map pOldMemMap)
{   
    return apr_mmap_dup(
        pNewMemMap, 
        pOldMemMap, 
        pMemPool);
}

W_RESULT     w_mem_map_delete(_In_ w_mem_map pMemoryMap)
{
    return apr_mmap_delete(pMemoryMap);
}

W_RESULT     w_mem_map_offset(
    _Inout_ void** pAddress,
    _In_ w_mem_map pMemoryMap,
    _In_ w_offset pOffset)
{
    return apr_mmap_offset(pAddress, pMemoryMap, pOffset);
}