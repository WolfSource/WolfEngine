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
W_RESULT     w_mem_map_create(_Inout_ w_mem_map* pNewMemMap,
                              _In_ w_file pFile,
                              _In_ w_offset pOffset,
                              _In_ size_t pSize,
                              _In_ int pFlag,
                              _In_ w_mem_pool pMemPool);

/**
 * Duplicate the specified Mmemory MAP.
 * @param pNewMemMap The structure to duplicate into.
 * @param pOldMemMap The memory map to duplicate.
 * @param pMemPool The pool to use for new memory map.
 * @return result code
*/
W_RESULT     w_mem_map_dup(_Inout_ w_mem_map* pNewMemMap, _In_ w_mem_map pOldMemMap, _In_ w_mem_pool pMemPool);

/**
 * Remove a memory map.
 * @param pMemoryMap The mmap'ed file.
 * @return result code
*/
W_RESULT     w_mem_map_delete(_In_ w_mem_map pMemoryMap);

/**
 * Move the pointer into the mmap'ed file to the specified offset.
 * @param pAddress The pointer to the offset specified.
 * @param pMemoryMap The mmap'ed file.
 * @param pOffset The offset to move to.
 * @return result code
*/
W_RESULT     w_mem_map_offset(void** pAddress, w_mem_map pMemoryMap, w_offset pOffset);

#ifdef __cplusplus
}
#endif
