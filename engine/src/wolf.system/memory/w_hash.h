/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_hash.h
    Description      : hash with key and value.
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

    //forward declaration
    typedef struct apr_hash_t* w_hash;
    typedef struct apr_hash_index_t* w_hash_index;

    typedef unsigned int(*w_hash_custom_fn)(
        const char* /*pKey*/,
#ifdef W_PLATFORM_WIN
        __int64*
#else
        int*
#endif
        /*pLen*/);

    typedef void* (*w_hash_merger)(
        w_apr_pool /*pMemPool*/,
        const void* /*pKey*/, 
#ifdef W_PLATFORM_WIN
        __int64 
#else
        int
#endif
        /*pLen*/, 
        const void* /*pHash1Value*/, 
        const void* /*pHash2Value*/, 
        const void* /*pData*/);

    typedef int(w_hash_do_callback_fn)(
        void* /*pRec*/, 
        const void* /*pKey*/, 
#ifdef W_PLATFORM_WIN
        __int64 
#else
        int
#endif
        /*pLen*/, 
        const void* /*pValue*/);
    
    /**
     * Create a hash table.
     * @param pMemPool The pool to allocate the hash table out of. Set NULL to use default memory pool.
     * @param pHash The hash table out of.
     * @return The hash table just created
     */
    W_SYSTEM_EXPORT
        W_RESULT w_hash_init(_Inout_ w_mem_pool pMemPool, _Inout_ w_hash* pHash);

    /**
     * Create a hash table with a custom hash function
     * @param pMemPool The pool to allocate the hash table out of
     * @param pHashCustomFunc A custom hash function.
     * @return The hash table just created
     */
    W_SYSTEM_EXPORT
        w_hash w_hash_make_custom(
            _Inout_ w_mem_pool pMemPool,
            _In_ w_hash_custom_fn pHashCustomFunc);

    /**
     * Get the number of key/value pairs in the hash table.
     * @param pHash The hash table
     * @return The number of key/value pairs in the hash table.
     */
    W_SYSTEM_EXPORT
        uint32_t w_hash_size(_In_ w_hash pHash);

    /**
     * Clear any key/value pairs in the hash table.
     * @param pHash , The hash table
     */
    W_SYSTEM_EXPORT
        void 	w_hash_clear(_In_ w_hash pHash);

    /**
     * Associate a value with a key in a hash table.
     * @param pHash , The hash table
     * @param pKey , Pointer to the key
     * @param pKeyLen , Length of the key. Can be APR_HASH_KEY_STRING to use the string length.
     * @param pValue , Value to associate with the key
     * @remark If the value is NULL the hash entry is deleted.
                The key is stored as is, and so must have a lifetime
                at least as long as the hash table's pool.
     */
    W_SYSTEM_EXPORT
        void w_hash_set(
            _In_ w_hash pHash,
            _In_ const void* pKey,
            _In_ size_t pKeyLen,
            _In_z_ const void* pValue);

    /**
     * Look up the value associated with a key in a hash table.
     * @param pHash The hash table
     * @param pKey Pointer to the key
     * @param pKeyLen Length of the key. Can be APR_HASH_KEY_STRING to use the string length.
     * @return Returns NULL if the key is not present.
     */
    W_SYSTEM_EXPORT
        void* w_hash_get(
            _In_ w_hash pHash,
            _In_z_ const void* pKey,
            _In_ size_t pKeyLen);

    /**
     * Make a copy of a hash table
     * @param pMemPool The pool from which to allocate the new hash table
     * @param pSourceHash The hash table to clone
     * @return Returns The hash table just created.
     */
    W_SYSTEM_EXPORT
        w_hash w_hash_clone(
            _Inout_ w_mem_pool pMemPool,
            _In_ w_hash pSourceHash);

    /**
     * Merge two hash tables into one new hash table. If the same key
     * is present in both tables, call the supplied merge function to
     * produce a merged value for the key in the new table.  Both
     * hash tables must use the same hash function.
     * @param pMemPool The pool to use for the new hash table
     * @param pHash1 The first of the tables to merge
     * @param pHash2 The second of the tables to merge
     * @param pHashMerger A callback function to merge values, or NULL to
        make values from pHash1 override values from pHash2
     * @param pCustomUserData a custome user data to pass to the merger function
     * @return A new hash table containing all of the data from the two passed in
     */
    W_SYSTEM_EXPORT
        w_hash w_hash_merge(
            _Inout_ w_mem_pool pMemPool,
            _In_ const w_hash pHash1,
            _In_ const w_hash pHash2,
            _In_ w_hash_merger pHashMergerFunction,
            _In_ const void* pCustomUserData);

    /**
     * Merge two hash tables into one new hash table. The values of the overlay
     * hash override the values of the base if both have the same key.
       Both hash tables must use the same hash function.
     * @param pMemPool The pool to use for the new hash table
     * @param pBase The table that represents the initial values of the new table
     * @param pOverlay The table to add to the initial table
     * @return A new hash table containing all of the data from the two passed in
     */
    W_SYSTEM_EXPORT
        w_hash w_hash_overlay(
            _In_ w_mem_pool pMemPool,
            _In_ const w_hash pBase,
            _In_ const w_hash pOverlay);

    /**
     * Start iterating over the entries in a hash table.
     * @param pHash The hash table
     * @param pMemPool The pool to allocate the apr_hash_index_t iterator.
            If this pool is NULL, then the default pool iterator is used.
     * @return The iteration state
     * @remark  There is no restriction on adding or deleting hash entries during
     * an iteration (although the results may be unpredictable unless all you do
     * is delete the current entry) and multiple iterations can be in
     * progress at the same time.
     *
     * @par Example:
     * @code
     * int sum_values(apr_pool_t *p, apr_hash_t *ht)
     * {
     *     apr_hash_index_t *hi;
     *     void *val;
     *     int sum = 0;
     *     for (hi = w_hash_first(p, ht); hi; hi = w_hash_next(hi)) {
     *         w_hash_this(hi, NULL, NULL, &val);
     *         sum += *(int *)val;
     *     }
     *     return sum;
     * }
     * @endcode
     */
    W_SYSTEM_EXPORT
        w_hash_index w_hash_first(
            _Inout_ w_mem_pool pMemPool,
            _In_ w_hash pHash);

    /**
     * Continue iterating over the entries in a hash table.
     * @param pHashIndex The iteration state
     * @return a pointer to the updated iteration state. NULL if there are no more entries.
     */
    W_SYSTEM_EXPORT
        w_hash_index w_hash_next(_In_ w_hash_index pHashIndex);

    /**
     * Get the current entry's details from the iteration state.
     * @param pHashIndex The iteration state
     * @param pKey Return pointer for the pointer to the key.
     * @param pKeyLen Return pointer for the key length.
     * @param pValue Return pointer for the associated value.
     * @remark The return pointers should point to a variable that will be set to
            the corresponding data, or they may be NULL if the data isn't interesting.
     */
    W_SYSTEM_EXPORT
        void w_hash_this(
            _In_ w_hash_index pHashIndex,
            _Inout_ const void** pKey,
            _Inout_ size_t* pKeyLen,
            _Inout_ void** pValue);


    /**
     * Get the current entry's key from the iteration state.
     * @param pHashIndex The iteration state
     * @return The pointer to the key
     */
    W_SYSTEM_EXPORT
        const void* w_hash_this_key(_In_ w_hash_index pHashIndex);

    /**
     * Get the current entry's key length from the iteration state.
     * @param pHashIndex The iteration state
     * @return The key length
     */
    W_SYSTEM_EXPORT
        size_t w_hash_this_key_len(_In_ w_hash_index pHashIndex);

    /**
     * Get the current entry's value from the iteration state.
     * @param pHashIndex The iteration state
     * @return The pointer to the value
     */
    W_SYSTEM_EXPORT
        void* w_hash_this_val(w_hash_index pHashIndex);

    /**
     * Iterate over a hash table running the provided function once for every
     * element in the hash table. The @param comp function will be invoked for
     * every element in the hash table.
     * @param pHashCompareFunc The function to run
     * @param pRec The data to pass as the first argument to the function
     * @param pHash The hash table to iterate over
     * @return 0 if one of the pHashCompareFunc() iterations returned zero;
            1 if all iterations returned non-zero
     */
    W_SYSTEM_EXPORT
        int w_hash_do(
            _In_ w_hash_do_callback_fn* pHashCompareFunc,
            _In_ void* pRec,
            _In_ const w_hash pHash);

    /**
     * Get a pointer to the pool which the hash table was created in
     * @param pHash the hash
     * @return apr memory pool
     */
    W_SYSTEM_EXPORT
        w_apr_pool w_hash_get_mem_pool(_In_ const w_hash pHash);

#ifdef __cplusplus
}
#endif
