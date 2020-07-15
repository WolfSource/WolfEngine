#include "w_shared_mem.h"
#include <apr-1/apr_shm.h>

W_RESULT    w_shared_mem_create(_Inout_ w_shared_mem* pSharedMem,
                                _In_ size_t pReqSize,
                                _In_z_ const char* pFileName,
                                _In_ w_mem_pool pMemPool)
{
    if (!pMemPool)
    {
        //get default thread pool
        pMemPool = w_get_default_memory_pool();
        if (!pMemPool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_shared_mem_create");
            return W_FAILURE;
        }
    }
    return apr_shm_create((apr_shm_t**)pSharedMem, pReqSize, pFileName, (apr_pool_t*)pMemPool);
}
 
W_RESULT    w_shared_mem_create_ex(_Inout_ w_shared_mem* pSharedMem,
                                   _In_ size_t pReqSize,
                                   _In_z_ const char* pFileName,
                                   _In_ w_mem_pool pMemPool,
                                   _In_ int pFlags)
{
    if (!pMemPool)
    {
        //get default thread pool
        pMemPool = w_get_default_memory_pool();
        if (!pMemPool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_shared_mem_create_ex");
            return W_FAILURE;
        }
    }
    return apr_shm_create_ex((apr_shm_t**)pSharedMem, pReqSize, pFileName, (apr_pool_t*)pMemPool, pFlags);
}
 
W_RESULT    w_shared_mem_remove(_In_z_ const char* pFileName, _In_ w_mem_pool pMemPool)
{
    if (!pMemPool)
    {
        W_ASSERT(false, "memory pool is NULL! trace info: w_shared_mem_remove");
        return W_FAILURE;
    }
    return apr_shm_remove(pFileName, (apr_pool_t*)pMemPool);
}
 
W_RESULT    w_shared_mem_delete(_In_ w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_delete");
        return W_FAILURE;
    }
    return apr_shm_delete((apr_shm_t*)pSharedMem);
}
 
W_RESULT    w_shared_mem_destroy(_In_ w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_destroy");
        return W_FAILURE;
    }
    return apr_shm_destroy((apr_shm_t*)pSharedMem);
}
 
W_RESULT    w_shared_mem_attach(_Inout_ w_shared_mem* pSharedMem,
                                _In_z_ const char* pFileName,
                                _In_ w_mem_pool pMemPool)
{
    if (!pSharedMem || !*pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_attach");
        return W_FAILURE;
    }
    if (!pMemPool)
    {
        //get default thread pool
        pMemPool = w_get_default_memory_pool();
        if (!pMemPool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_shared_mem_attach");
            return W_FAILURE;
        }
    }
    
    return apr_shm_attach((apr_shm_t**)pSharedMem, pFileName, (apr_pool_t*)pMemPool);
}
 
W_RESULT    w_shared_mem_attach_ex(_Inout_ w_shared_mem* pSharedMem,
                                   _In_z_ const char* pFileName,
                                   _In_ w_mem_pool pMemPool,
                                   _In_ int pFlags)
{
    if (!pSharedMem || !*pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL!. trace info: w_shared_mem_attach_ex");
        return W_FAILURE;
    }
    if (!pMemPool)
    {
        //get default thread pool
        pMemPool = w_get_default_memory_pool();
        if (!pMemPool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_shared_mem_attach_ex");
            return W_FAILURE;
        }
    }
    
    return apr_shm_attach_ex((apr_shm_t**)pSharedMem, pFileName, (apr_pool_t*)pMemPool, pFlags);
}

W_RESULT    w_shared_mem_detach(_In_ w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_detach");
        return W_FAILURE;
    }
    return apr_shm_detach((apr_shm_t*)pSharedMem);
}
 
void*   w_shared_mem_get_base_addr(_In_ const w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_get_base_addr");
        return NULL;
    }
    return apr_shm_baseaddr_get((const apr_shm_t*)pSharedMem);
}
 
size_t  w_shared_mem_get_size(_In_ const w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_get_size");
        return 0;
    }
    return apr_shm_size_get((const apr_shm_t*)pSharedMem);
}
 
w_mem_pool  w_shared_mem_get_mem_pool(_In_ const w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_get_mem_pool");
        return 0;
    }
    return apr_shm_pool_get((const apr_shm_t*)pSharedMem);
}
