#include "w_hash.h"
#include <apr-1/apr_hash.h>

w_hash w_hash_init(w_mem_pool pMemPool)
{
    const char* _trace_info = "w_hash_init";
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
            return W_FAILURE;
        }
    }
    return (w_hash)apr_hash_make((apr_pool_t*)_pool);
}

w_hash w_hash_make_custom(
    _In_ w_mem_pool pMemPool,
    _In_ w_hash_custom_fn pHashCustomFunc)
{
    const char* _trace_info = "w_hash_make_custom";
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
            return W_FAILURE;
        }
    }

    return (w_hash)apr_hash_make_custom((apr_pool_t*)_pool, pHashCustomFunc);
}

void w_hash_set(
    _In_ w_hash pHash,
    _In_ const void* pKey,
    _In_ size_t pKeyLen,
    _In_z_ const void* pValue)
{
    if (!pHash || !pKey || pKeyLen == 0)
    {
        return APR_BADARG;
    }
    apr_hash_set(
        (apr_hash_t*)pHash,
        pKey,
        pKeyLen,
        pValue);
}

uint32_t w_hash_size(_In_ w_hash pHash)
{
    if (!pHash) 
    {
        return 0;
    }
    return apr_hash_count(pHash);
}

void w_hash_clear(_In_ w_hash pHash)
{
    if (!pHash)
    {
        return APR_BADARG;
    }
    apr_hash_clear(pHash);
}

void* w_hash_get(
    _In_ w_hash pHash,
    _In_z_ const void* pKey,
    _In_ size_t pKeyLen)
{
    if (!pHash || !pKey || pKeyLen == 0)
    {
        return APR_BADARG;
    }
    return apr_hash_get(pHash, pKey, pKeyLen);
}