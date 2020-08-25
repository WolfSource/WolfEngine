#include "w_hash.h"
#include <apr-1/apr_hash.h>

w_hash w_hash_init(_Inout_ w_mem_pool pMemPool)
{
    const char* _trace_info = "w_hash_init";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    return apr_hash_make(pMemPool);
}

w_hash w_hash_make_custom(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_hash_custom_fn pHashCustomFunc)
{
    const char* _trace_info = "w_hash_make_custom";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    return apr_hash_make_custom(pMemPool, pHashCustomFunc);
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

w_hash w_hash_clone(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_hash pSourceHash)
{
    const char* _trace_info = "w_hash_copy";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    return (w_hash)apr_hash_copy(pMemPool, pSourceHash);
}

w_hash w_hash_merge(
    _Inout_ w_mem_pool pMemPool,
    _In_ const w_hash pHash1,
    _In_ const w_hash pHash2,
    _In_ w_hash_merger pHashMergerFunction,
    _In_ const void* pCustomUserData)
{
    const char* _trace_info = "w_hash_merge";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    return apr_hash_merge(
        pMemPool,
        pHash1, 
        pHash2, 
        pHashMergerFunction, 
        pCustomUserData);
}

w_hash w_hash_overlay(
    _Inout_ w_mem_pool pMemPool,
    _In_ const w_hash pBase,
    _In_ const w_hash pOverlay)
{
    const char* _trace_info = "w_hash_overlay";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;
    }
    return apr_hash_overlay(pBase, pOverlay, pMemPool);
}

w_hash_index w_hash_first(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_hash pHash)
{
    const char* _trace_info = "w_hash_first";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return APR_BADARG;

    }
    return apr_hash_first(pMemPool, pHash);
}

w_hash_index w_hash_next(_In_ w_hash_index pHashIndex)
{
    if (!pHashIndex)
    {
        return NULL;
    }
    return apr_hash_next(pHashIndex);
}

void w_hash_this(
    _In_ w_hash_index pHashIndex,
    _Inout_ const void** pKey,
    _Inout_ size_t* pKeyLen,
    _Inout_ void** pValue)
{
    if (pHashIndex)
    {
        apr_hash_this(
            pHashIndex,
            pKey,
            pKeyLen,
            pValue);
    }
}

const void* w_hash_this_key(_In_ w_hash_index pHashIndex)
{
    if (!pHashIndex)
    {
        return NULL;
    }
    return apr_hash_this_key(pHashIndex);
}

void* w_hash_this_val(w_hash_index pHashIndex)
{
    if (!pHashIndex)
    {
        return NULL;
    }
    return apr_hash_this_val(pHashIndex);
}

int w_hash_do(
    _In_ w_hash_do_callback_fn* pHashCallbackDo,
    _In_ void* pRec,
    _In_ const w_hash pHash)
{
    if (!pHashCallbackDo || !pHash) return 0;
    
    return apr_hash_do(pHashCallbackDo, pRec, pHash);
}

size_t w_hash_this_key_len(_In_ w_hash_index pHashIndex)
{
    if (!pHashIndex) return 0;
    
    return apr_hash_this_key_len(pHashIndex);
}

w_mem_pool w_hash_get_mem_pool(_In_ const w_hash pHash)
{
    if (!pHash)
    {
        return NULL;
    }
    return apr_hash_pool_get(pHash);
}
