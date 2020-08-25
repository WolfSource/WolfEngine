/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_mem_map.h
    Description      : memory map
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

//forward declaration
typedef struct apr_mmap_t* w_mem_map;

/**
 * Create a new memory mapped file out of an existing file.
 * @param pMemPool The pool to allocate out of
 * @param pNewMemMap The newly created memory mapped file.
 * @param pFile The file to turn into an memory map.
 * @param pOffset The offset into the file to start the data pointer at.
 * @param pSize The size of the file
 * @param pFlag bit-wise or of:
 * <PRE>
 *          APR_MMAP_READ       MMap opened for reading
 *          APR_MMAP_WRITE      MMap opened for writing
 * </PRE>
 * @param pMemPool The pool to use when creating the memory map.
 * @return result code
 */
W_SYSTEM_EXPORT
W_RESULT     w_mem_map_create(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_mem_map* pNewMemMap,
    _In_ w_file pFile,
    _In_ w_offset pOffset,
    _In_ size_t pSize,
    _In_ int pFlag);

/**
 * Duplicate the specified Mmemory MAP.
 * @param pMemPool The pool to allocate out of
 * @param pNewMemMap The structure to duplicate into.
 * @param pOldMemMap The memory map to duplicate.
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT     w_mem_map_dup(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_mem_map* pNewMemMap, 
    _In_ w_mem_map pOldMemMap);

/**
 * Remove a memory map.
 * @param pMemoryMap The mmap'ed file.
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT     w_mem_map_delete(_In_ w_mem_map pMemoryMap);

/**
 * Move the pointer into the mmap'ed file to the specified offset.
 * @param pAddress The pointer to the offset specified.
 * @param pMemoryMap The mmap'ed file.
 * @param pOffset The offset to move to.
 * @return result code
*/
W_SYSTEM_EXPORT
W_RESULT     w_mem_map_offset(
    _Inout_ void** pAddress, 
    _In_ w_mem_map pMemoryMap, 
    _In_ w_offset pOffset);

#ifdef __cplusplus
}
#endif
