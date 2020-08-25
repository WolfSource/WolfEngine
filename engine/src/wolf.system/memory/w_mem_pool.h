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

    typedef enum
    {
        /*
            This memory pool is optimized for allocation speed; The downside of
            this design is that there's no way to reclaim memory within a pool.
            So make sure use seperated memory pools, then allocate a pool
            per transaction and finally release it once the transaction ends.
        */
        W_MEM_POOL_FAST_EXTEND = 0x00,
        /*
            This memory pool is optimized for reclaiming;
            It's a fast lock free thread caching 16-byte aligned memory allocator.
            It has support for huge/large pages on Windows, Linux and MacOS.
            If the system does not support huge pages it will be automatically disabled.
        */
        W_MEM_POOL_ALIGNED_RECLAIM = 0x01,
        //W_MEM_POOL_LINKED_LIST = 2,
        W_MEM_POOL_UNKNOWN = 0xFF,
    } w_mem_pool_type;

    struct w_mem_pool_t;
    typedef struct w_mem_pool_t* w_mem_pool;
    typedef struct apr_pool_t* w_apr_pool;

    /**
     * create and initialize a memory pool
     * @param pMemPool The pool to allocate out of
     * @param pType The type of memory pool
     * <PRE>
            W_MEM_POOL_FAST_EXTEND = 0

            This memory pool is optimized for allocation speed; The downside of
            this design is that there's no way to reclaim memory within a pool.
            So make sure use seperated memory pools, then allocate a pool
            per transaction and finally release it once the transaction ends.
            This memory pool is essentially designed for smaller chunks.
            If you need a large size of memory chunk, e.g. over several mega bytes,
            don't use this memory pool and use W_MEM_POOL_LINEAR_RECLAIM type.

            W_MEM_POOL_ALIGNED_RECLAIM = 1

            This memory pool is optimized for reclaiming;  
            It's a fast lock free thread caching 16-byte aligned memory allocator. 
            It has support for huge/large pages on Windows, Linux and MacOS. 
            If the system does not support huge pages it will be automatically disabled. 
     * </PRE>
     * @return result code
     */
    W_SYSTEM_EXPORT
        W_RESULT w_mem_pool_init(
            _Inout_ w_mem_pool* pMemPool,
            _In_ w_mem_pool_type pType);

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
     * free a memory address
     * @param pMemPool The pool which is going to destroy.
     * @param pMemoryAddress The memory address.
     */
    W_SYSTEM_EXPORT
        W_RESULT w_free(
            _Inout_ w_mem_pool pMemPool,
            _In_ void* pMemoryAddress);

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
     * get usable size of memory block, from given pointer to the end of block
     * @param pMemPool the memory pool
     * @param pPtr pointer to memory block
     * @return the free size of memory block
    */
    W_SYSTEM_EXPORT
        size_t	w_mem_pool_aligned_reclaim_get_usable_size(
            _In_ w_mem_pool pMemPool,
            _In_ void* pPtr);

    /**
     * get type of memory pool
     * @param pMemPool the memory pool
     * @return the type of memory pool
    */
    W_SYSTEM_EXPORT
        w_mem_pool_type	w_mem_pool_get_type(_In_ w_mem_pool pMemPool);

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
        size_t w_mem_pool_get_ref_counts(_In_ w_mem_pool_type pMemPoolType);

#ifdef __cplusplus
}
#endif
