#include "w_hash.h"
#include <apr_hash.h>
#include "log/w_log.h"

W_RESULT w_hash_init(_Inout_ w_mem_pool pMemPool, _Inout_ w_hash* pHash)
{
    const char* _trace_info = "w_hash_init";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "memory pool is invalid! trace info: %s", _trace_info);
        return W_BAD_ARG;
    }
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        *pHash = apr_hash_make(_pool);
        if (*pHash)
        {
            return W_SUCCESS;
        }
    }
    return W_FAILURE;
}

w_hash w_hash_make_custom(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_hash_custom_fn pHashCustomFunc)
{
    const char* _trace_info = "w_hash_make_custom";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "memory pool is invalid! trace info: %s", _trace_info);
        return NULL;
    }
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_hash_make_custom(_pool, pHashCustomFunc);
    }
    return NULL;
}

void w_hash_set(
    _In_ w_hash pHash,
    _In_ const void* pKey,
    _In_ size_t pKeyLen,
    _In_z_ const void* pValue)
{
    const char* _trace_info = "w_hash_set";
    if (!pHash || !pKey || pKeyLen == 0)
    {
        W_ASSERT_P(false, "invalid parameters! trace info: %s", _trace_info);
        return;
    }

    apr_hash_set(
        pHash,
        pKey,
        pKeyLen,
        pValue);
}

uint32_t w_hash_size(_In_ w_hash pHash)
{
    const char* _trace_info = "w_hash_size";
    if (!pHash)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
        return 0;
    }
    return apr_hash_count(pHash);
}

void w_hash_clear(_In_ w_hash pHash)
{
    const char* _trace_info = "w_hash_clear";
    if (!pHash)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
        return;
    }
    apr_hash_clear(pHash);
}

void* w_hash_get(
    _In_ w_hash pHash,
    _In_z_ const void* pKey,
    _In_ size_t pKeyLen)
{
    const char* _trace_info = "w_hash_get";
    if (!pHash || !pKey || pKeyLen == 0)
    {
        W_ASSERT_P(false, "invalid parameters! trace info: %s", _trace_info);
        return NULL;
    }
    return apr_hash_get(pHash, pKey, pKeyLen);
}

w_hash w_hash_clone(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_hash pSourceHash)
{
    const char* _trace_info = "w_hash_clone";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "memory pool is invalid! trace info: %s", _trace_info);
        return NULL;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_hash_copy(_pool, pSourceHash);
    }

    return NULL;
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
        W_ASSERT_P(false, "memory pool is invalid! trace info: %s", _trace_info);
        return NULL;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_hash_merge(
            _pool,
            pHash1,
            pHash2,
            pHashMergerFunction,
            pCustomUserData);
    }
    return NULL;
}

w_hash w_hash_overlay(
    _Inout_ w_mem_pool pMemPool,
    _In_ const w_hash pBase,
    _In_ const w_hash pOverlay)
{
    const char* _trace_info = "w_hash_overlay";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "memory pool is invalid! trace info: %s", _trace_info);
        return NULL;
    }
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_hash_overlay(_pool, pBase, pOverlay);
    }
    return NULL;
}

w_hash_index w_hash_first(
    _Inout_ w_mem_pool pMemPool,
    _In_ w_hash pHash)
{
    const char* _trace_info = "w_hash_first";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "memory pool is invalid! trace info: %s", _trace_info);
        return NULL;
    }
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        return apr_hash_first(_pool, pHash);
    }
    return NULL;
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
#ifdef W_PLATFORM_WIN
            pKeyLen,
#else
            (int*)pKeyLen,
#endif
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

w_apr_pool w_hash_get_mem_pool(_In_ const w_hash pHash)
{
    if (pHash)
    {
        return apr_hash_pool_get(pHash);
    }
    return NULL;
}
