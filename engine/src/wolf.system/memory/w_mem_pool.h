/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_mem_pool.h
    Description      : contains three types of memory pool including fast allocate, linear reclaim and linked list
    Comment          : 
*/

#pragma once

#include <os/w_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

    struct w_mem_pool_t;
    typedef struct w_mem_pool_t* w_mem_pool;
    typedef struct apr_pool_t* w_apr_pool;

    /**
     * create and initialize a memory pool
     * @param pMemPool The pool to allocate out of
     * @note This memory pool is optimized for allocation speed; The downside of
            this design is that there's no way to reclaim memory within a pool.
            So make sure use seperated memory pools, then allocate a pool
            per transaction and finally release it once the transaction ends.
            This memory pool is essentially designed for smaller chunks.
            If you need a large size of memory chunk, e.g. over several mega bytes,
            don't use this memory pool and use W_MEM_POOL_LINEAR_RECLAIM type.
     * @return result code
     */
    W_SYSTEM_EXPORT
        W_RESULT w_mem_pool_init(_Inout_ w_mem_pool* pMemPool);

    /**
     * create and initialize a memory pool
     * @param pMemPool The pool to allocate out of
     * @param pParentMemPool The parent pool
     * @note This memory pool is optimized for allocation speed; The downside of
            this design is that there's no way to reclaim memory within a pool.
            So make sure use seperated memory pools, then allocate a pool
            per transaction and finally release it once the transaction ends.
            This memory pool is essentially designed for smaller chunks.
            If you need a large size of memory chunk, e.g. over several mega bytes,
            don't use this memory pool and use W_MEM_POOL_LINEAR_RECLAIM type.
     * @return result code
     */
    W_SYSTEM_EXPORT
        W_RESULT w_mem_pool_init_from_parent(
            _Inout_ w_mem_pool* pMemPool,
            _Inout_opt_ w_mem_pool* pParentMemPool);

    /**
     * allocate a memory
     * @param pMemPool The pool to allocate out of
     * @param pMemSize The memory size.
     */
    W_SYSTEM_EXPORT
        void* w_malloc(
            _Inout_ w_mem_pool pMemPool,
            _In_ size_t pMemSize);

    /**
     * allocate a memory and initialize it with zero
     * @param pMemPool The pool to allocate out of
     * @param pMemSize The memory size.
     */
    W_SYSTEM_EXPORT
        void* w_calloc(
            _Inout_ w_mem_pool pMemPool,
            _In_ size_t pMemSize);

    /**
     * terminate a memory pool
     * @param pMemPool The pool which is going to destroy.
    */
    W_SYSTEM_EXPORT
        void w_mem_pool_fini(_Inout_ w_mem_pool* pMemPool);

    /**
    * clear a apr memory pool for reusing again
    * @param pMemPool The pool which is going to destroy.
    */
    W_SYSTEM_EXPORT
        void w_mem_pool_apr_clear(_Inout_ w_mem_pool pMemPool);

    /**
     * get apr memory pool (fast extend one)
     * @param pMemPool the memory pool
     * @return a pointer to apr pool
    */
    W_SYSTEM_EXPORT
        w_apr_pool w_mem_pool_get_apr_pool(_Inout_ w_mem_pool pMemPool);

    /**
     * get counts of active memory pools
     * @param pMemPoolType the memory pool
     * @return number of references
    */
    W_SYSTEM_EXPORT
        size_t w_mem_pool_get_ref_counts();

#ifdef __cplusplus
}
#endif
