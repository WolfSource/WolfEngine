#include "w_shared_mem.h"
#include <apr_shm.h>
#include "log/w_log.h"

W_RESULT    w_shared_mem_create(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_shared_mem* pSharedMem,
    _In_ size_t pReqSize,
    _In_z_ const char* pFileName)
{
    const char* _trace_info = "w_shared_mem_create";
    if (!pMemPool || !pReqSize)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_shm_create(pSharedMem, pReqSize, pFileName, _pool);
    }
    return W_FAILURE;
}
 
W_RESULT    w_shared_mem_create_ex(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_shared_mem* pSharedMem,
    _In_ size_t pReqSize,
    _In_z_ const char* pFileName,
    _In_ int pFlags)
{
    const char* _trace_info = "w_shared_mem_create_ex";

    if (!pMemPool || !pReqSize)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_shm_create_ex(
            pSharedMem,
            pReqSize,
            pFileName,
            _pool,
            pFlags);
    }
    return W_FAILURE;
}
 
W_RESULT    w_shared_mem_remove(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pFileName)
{
    const char* _trace_info = "w_shared_mem_remove";

    if (!pMemPool || !pFileName)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_shm_remove(pFileName, _pool);
    }
    return W_FAILURE;
}
 
W_RESULT    w_shared_mem_delete(_In_ w_shared_mem pSharedMem)
{
    const char* _trace_info = "w_shared_mem_delete";
    if (!pSharedMem)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    return apr_shm_delete(pSharedMem);
}
 
W_RESULT    w_shared_mem_destroy(_In_ w_shared_mem pSharedMem)
{
    const char* _trace_info = "w_shared_mem_destroy";
    if (!pSharedMem)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    return apr_shm_destroy(pSharedMem);
}
 
W_RESULT    w_shared_mem_attach(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_shared_mem* pSharedMem,
    _In_z_ const char* pFileName)
{
    const char* _trace_info = "w_shared_mem_attach";
    if (!pMemPool || !pSharedMem || !*pSharedMem)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_shm_attach(
            pSharedMem,
            pFileName,
            _pool);
    }
    return W_FAILURE;
}
 
W_RESULT    w_shared_mem_attach_ex(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_shared_mem* pSharedMem,
    _In_z_ const char* pFileName,
    _In_ int pFlags)
{
    const char* _trace_info = "w_shared_mem_attach_ex";
    if (!pMemPool || !pSharedMem || !*pSharedMem || !pFileName)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_shm_attach_ex(
            pSharedMem,
            pFileName,
            _pool,
            pFlags);
    }
    return W_FAILURE;
}

W_RESULT    w_shared_mem_detach(_In_ w_shared_mem pSharedMem)
{
    const char* _trace_info = "w_shared_mem_detach";
    if (!pSharedMem)
    {
        W_ASSERT_P(false, "shared memory is NULL! trace info: %s", _trace_info);
        return W_FAILURE;
    }
    return apr_shm_detach(pSharedMem);
}
 
void* w_shared_mem_get_base_addr(_In_ const w_shared_mem pSharedMem)
{
    const char* _trace_info = "w_shared_mem_get_base_addr";
    if (!pSharedMem)
    {
        W_ASSERT_P(false, "shared memory is NULL! trace info: %s", _trace_info);
        return NULL;
    }
    return apr_shm_baseaddr_get(pSharedMem);
}
 
size_t  w_shared_mem_get_size(_In_ const w_shared_mem pSharedMem)
{
    const char* _trace_info = "w_shared_mem_get_size";
    if (!pSharedMem)
    {
        W_ASSERT_P(false, "bad args! trace info: %s", _trace_info);
        return 0;
    }
    return apr_shm_size_get(pSharedMem);
}
 
w_apr_pool  w_shared_mem_get_mem_pool(_In_ const w_shared_mem pSharedMem)
{
    const char* _trace_info = "w_shared_mem_get_mem_pool";
    if (!pSharedMem)
    {
        W_ASSERT_P(false, "bad args! trace info: %s", _trace_info);
        return NULL;
    }
    return apr_shm_pool_get(pSharedMem);
}
