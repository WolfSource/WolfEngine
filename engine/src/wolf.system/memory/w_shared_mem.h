/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_shared_mem.h
    Description      : shared memory
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

    //forward declaration
    typedef struct apr_shm_t* w_shared_mem;

    /**
     * Create and make accessible a shared memory segment with default properties.
     * @param pMemPool The pool to allocate out of
     * @param pSharedMem The shared memory structure to create.
     * @param pReqSize The desired size of the segment.
     * @param pFileName The file to use for shared memory on platforms that require it.
     * @param pMemPool the pool from which to allocate the shared memory structure.
     * @remark A note about Anonymous vs. Named shared memory segments:
     *         Not all plaforms support anonymous shared memory segments, but in
     *         some cases it is prefered over other types of shared memory
     *         implementations. Passing a NULL 'file' parameter to this function
     *         will cause the subsystem to use anonymous shared memory segments.
     *         If such a system is not available, APR_ENOTIMPL is returned.
     * @remark A note about allocation sizes:
     *         On some platforms it is necessary to store some metainformation
     *         about the segment within the actual segment. In order to supply
     *         the caller with the requested size it may be necessary for the
     *         implementation to request a slightly greater segment length
     *         from the subsystem. In all cases, the apr_shm_baseaddr_get()
     *         function will return the first usable byte of memory.
     * @return result code
     *
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_shared_mem_create(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_shared_mem* pSharedMem,
            _In_ size_t pReqSize,
            _In_z_ const char* pFileName);

    /**
     * Create and make accessible a shared memory segment with platform-
     * specific processing.
     * @param pMemPool The pool to allocate out of
     * @param pSharedMem The shared memory structure to create.
     * @param pReqSize The desired size of the segment.
     * @param pFileName The file to use for shared memory on platforms that
     *        require it.
     * @param pMemPool the pool from which to allocate the shared memory
     *        structure.
     * @param pFlags mask of APR_SHM_* (defined above)
     * @remark A note about Anonymous vs. Named shared memory segments:
     *         Not all plaforms support anonymous shared memory segments, but in
     *         some cases it is prefered over other types of shared memory
     *         implementations. Passing a NULL 'file' parameter to this function
     *         will cause the subsystem to use anonymous shared memory segments.
     *         If such a system is not available, APR_ENOTIMPL is returned.
     * @remark A note about allocation sizes:
     *         On some platforms it is necessary to store some metainformation
     *         about the segment within the actual segment. In order to supply
     *         the caller with the requested size it may be necessary for the
     *         implementation to request a slightly greater segment length
     *         from the subsystem. In all cases, the apr_shm_baseaddr_get()
     *         function will return the first usable byte of memory.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_shared_mem_create_ex(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_shared_mem* pSharedMem,
            _In_ size_t pReqSize,
            _In_z_ const char* pFileName,
            _In_ int pFlags);

    /**
     * Remove named resource associated with a shared memory segment,
     * preventing attachments to the resource, but not destroying it.
     * @param pMemPool The pool to allocate out of
     * @param pFileName The filename associated with shared-memory segment which needs to be removed
     * @param pMemPool The pool used for file operations
     * @remark This function is only supported on platforms which support
     * name-based shared memory segments, and will return APR_ENOTIMPL on
     * platforms without such support.  Removing the file while the shm
     * is in use is not entirely portable, caller may use this to enhance
     * obscurity of the resource, but be prepared for the call to fail,
     * and for concurrent attempts to create a resource of the same name
     * to also fail.  The pool cleanup of apr_shm_create (apr_shm_destroy)
     * also removes the named resource.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_shared_mem_remove(
            _Inout_ w_mem_pool pMemPool,
            _In_z_ const char* pFileName);

    /**
     * Delete named resource associated with a shared memory segment,
     * preventing attachments to the resource.
     * @param pSharedMem The shared memory segment structure to delete.
     * @remark This function is only supported on platforms which support
     * name-based shared memory segments, and will return APR_ENOTIMPL on
     * platforms without such support.  Removing the file while the shm
     * is in use is not entirely portable, caller may use this to enhance
     * obscurity of the resource, but be prepared for the call to fail,
     * and for concurrent attempts to create a resource of the same name
     * to also fail.  The pool cleanup of apr_shm_create (apr_shm_destroy)
     * also removes the named resource.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_shared_mem_delete(_In_ w_shared_mem pSharedMem);

    /**
     * Destroy a shared memory segment and associated memory.
     * @param pSharedMem The shared memory segment structure to destroy.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_shared_mem_destroy(_In_ w_shared_mem pSharedMem);

    /**
     * Attach to a shared memory segment that was created by another process.
     * @param pMemPool The pool to allocate out of
     * @param pSharedMem The shared memory structure to create.
     * @param pFileName The file used to create the original segment. (This MUST match the original filename.)
     * @param pMemPool the pool from which to allocate the shared memory structure for this process.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_shared_mem_attach(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_shared_mem* pSharedMem,
            _In_z_ const char* pFileName);

    /**
     * Attach to a shared memory segment that was created by another process, with platform-specific processing.
     * @param pMemPool The pool to allocate out of
     * @param pSharedMem The shared memory structure to create.
     * @param pFileName The file used to create the original segment.(This MUST match the original filename.)
     * @param pMemPool the pool from which to allocate the shared memory structure for this process.
     * @param pFlags mask of APR_SHM_*
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_shared_mem_attach_ex(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_shared_mem* pSharedMem,
            _In_z_ const char* pFileName,
            _In_ int pFlags);

    /**
     * Detach from a shared memory segment without destroying it.
     * @param pSharedMem The shared memory structure representing the segment to detach from.
     * @return result code
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_shared_mem_detach(_In_ w_shared_mem pSharedMem);

    /**
     * Retrieve the base address of the shared memory segment.
     * NOTE: This address is only usable within the callers address
     * space, since this API does not guarantee that other attaching
     * processes will maintain the same address mapping.
     * @param pSharedMem The shared memory segment from which to retrieve the base address.
     * @return address, aligned by APR_ALIGN_DEFAULT.
    */
    W_SYSTEM_EXPORT
        void* w_shared_mem_get_base_addr(_In_ const w_shared_mem pSharedMem);

    /**
     * Retrieve the length of a shared memory segment in bytes.
     * @param pSharedMem The shared memory segment from which to retrieve the segment length.
     * @return size in bytes
    */
    W_SYSTEM_EXPORT
        size_t      w_shared_mem_get_size(_In_ const w_shared_mem pSharedMem);

    /**
     * Get the pool used by this shared memory segment.
     * @param pSharedMem The shared memory segment
     * @return memory pool
    */
    W_SYSTEM_EXPORT
        w_mem_pool  w_shared_mem_get_mem_pool(_In_ const w_shared_mem pSharedMem);

#ifdef __cplusplus
}
#endif
