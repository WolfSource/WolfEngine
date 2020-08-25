#include "w_shared_mem.h"
#include <apr-1/apr_shm.h>

W_RESULT    w_shared_mem_create(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_shared_mem* pSharedMem,
    _In_ size_t pReqSize,
    _In_z_ const char* pFileName)
{
    const char* _trace_info = "w_shared_mem_create";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    return apr_shm_create((apr_shm_t**)pSharedMem, pReqSize, pFileName, pMemPool);
}
 
W_RESULT    w_shared_mem_create_ex(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_shared_mem* pSharedMem,
    _In_ size_t pReqSize,
    _In_z_ const char* pFileName,
    _In_ int pFlags)
{
    const char* _trace_info = "w_shared_mem_create_ex";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    return apr_shm_create_ex(
        pSharedMem,
        pReqSize,
        pFileName,
        pMemPool,
        pFlags);
}
 
W_RESULT    w_shared_mem_remove(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pFileName)
{
    const char* _trace_info = "w_shared_mem_remove";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    return apr_shm_remove(pFileName, pMemPool);
}
 
W_RESULT    w_shared_mem_delete(_In_ w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_delete");
        return APR_BADARG;
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
 
W_RESULT    w_shared_mem_attach(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_shared_mem* pSharedMem,
    _In_z_ const char* pFileName)
{
    const char* _trace_info = "w_shared_mem_attach";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    if (!pSharedMem || !*pSharedMem)
    {
        W_ASSERT_P(false, "shared memory is NULL! trace info: %s", _trace_info);
        return APR_BADARG;
    }
    if (!pMemPool)
    {
        //get default thread pool
        pMemPool = w_mem_pool_get_default();
        if (!pMemPool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_shared_mem_attach");
            return W_FAILURE;
        }
    }

    return apr_shm_attach(
        pSharedMem,
        pFileName, pMemPool);
}
 
W_RESULT    w_shared_mem_attach_ex(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_shared_mem* pSharedMem,
    _In_z_ const char* pFileName,
    _In_ int pFlags)
{
    const char* _trace_info = "w_shared_mem_attach_ex";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    if (!pSharedMem || !*pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL!. trace info: w_shared_mem_attach_ex");
        return APR_BADARG;
    }

    return apr_shm_attach_ex(
        pSharedMem,
        pFileName,
        pMemPool,
        pFlags);
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
 
void* w_shared_mem_get_base_addr(_In_ const w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_get_base_addr");
        return NULL;
    }
    return apr_shm_baseaddr_get(pSharedMem);
}
 
size_t  w_shared_mem_get_size(_In_ const w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_get_size");
        return 0;
    }
    return apr_shm_size_get(pSharedMem);
}
 
w_mem_pool  w_shared_mem_get_mem_pool(_In_ const w_shared_mem pSharedMem)
{
    if (!pSharedMem)
    {
        W_ASSERT(false, "shared memory is NULL! trace info: w_shared_mem_get_mem_pool");
        return 0;
    }
    return apr_shm_pool_get(pSharedMem);
}
