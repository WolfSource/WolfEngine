#include "w_mem_map.h"
#include <apr-1/apr_mmap.h>

W_RESULT    w_mem_map_create(_Inout_ w_mem_map* pNewMemMap,
                              _In_ w_file pFile,
                              _In_ w_offset pOffset,
                              _In_ size_t pSize,
                              _In_ int pFlag,
                              _In_ w_mem_pool pMemPool)
{    
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_get_default_memory_pool();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_mem_map_create");
            return W_FAILURE;
        }
    }
    
    return apr_mmap_create((apr_mmap_t**)pNewMemMap, pFile, pOffset, pSize, pFlag, _pool);
}


W_RESULT    w_mem_map_dup(_Inout_ w_mem_map* pNewMemMap,
                          _In_ w_mem_map pOldMemMap,
                          _In_ w_mem_pool pMemPool)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_get_default_memory_pool();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_mem_map_dup");
            return W_FAILURE;
        }
    }
    
    return apr_mmap_dup((apr_mmap_t**)pNewMemMap, (apr_mmap_t*)pOldMemMap, _pool);
}

W_RESULT     w_mem_map_delete(_In_ w_mem_map pMemoryMap)
{
     return apr_mmap_delete(pMemoryMap);
}

W_RESULT     w_mem_map_offset(void** pAddress, w_mem_map pMemoryMap, w_offset pOffset)
{
    return apr_mmap_offset(pAddress, pMemoryMap, pOffset);
}
