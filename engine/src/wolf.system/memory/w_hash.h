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

typedef unsigned int(*w_hash_custom_fn)(const char* /*pKey*/, size_t* /*pLen*/);
typedef int(w_hash_do_callback_fn)(void* /*pRec*/, const void* /*pKey*/, size_t /*pLen*/, const void* /*pValue*/);
typedef void* (*w_hash_merger)(w_mem_pool /*pMemPool*/, const void* /*pKey*/, size_t /*pLen*/, const void* /*pHash1Value*/, const void* /*pHash2Value*/, const void* /*pData*/);

/**
 * Create a hash table.
 * @param pool The pool to allocate the hash table out of. Set NULL to use default memory pool.
 * @return The hash table just created
 */
w_hash w_hash_init(_In_ w_mem_pool pMemPool);

/**
 * Create a hash table with a custom hash function
 * @param pMemPool The pool to allocate the hash table out of
 * @param pHashCustomFunc A custom hash function.
 * @return The hash table just created
 */
w_hash w_hash_make_custom(
    _In_ w_mem_pool pMemPool,
    _In_ w_hash_custom_fn pHashCustomFunc);

 /**
  * Get the number of key/value pairs in the hash table.
  * @param pHash The hash table
  * @return The number of key/value pairs in the hash table.
  */
uint32_t w_hash_size(_In_ w_hash pHash);

/**
 * Clear any key/value pairs in the hash table.
 * @param pHash , The hash table
 */
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
void* w_hash_get(
    _In_ w_hash pHash, 
    _In_z_ const void* pKey, 
    _In_ size_t pKeyLen);

/**
 * Make a copy of a hash table
 * @param pSourceHash The hash table to clone
 * @param pMemPool The pool from which to allocate the new hash table
 * @return Returns The hash table just created.
 */
w_hash w_hash_clone(
    _In_ w_hash pSourceHash,
    _In_ w_mem_pool pMemPool);

/**
 * Merge two hash tables into one new hash table. If the same key
 * is present in both tables, call the supplied merge function to
 * produce a merged value for the key in the new table.  Both
 * hash tables must use the same hash function.
 * @param pHash1 The first of the tables to merge
 * @param pHash2 The second of the tables to merge
 * @param pHashMerger A callback function to merge values, or NULL to 
    make values from pHash1 override values from pHash2
 * @param pCustomUserData a custome user data to pass to the merger function
 * @param pMemPool The pool to use for the new hash table
 * @return A new hash table containing all of the data from the two passed in
 */
w_hash w_hash_merge(
    _In_ const w_hash pHash1,
    _In_ const w_hash pHash2,
    _In_ w_hash_merger pHashMergerFunction,
    _In_ const void* pCustomUserData,
    _In_ w_mem_pool pMemPool);

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
w_hash_index w_hash_first(
    _In_ w_hash pHash,
    _In_ w_mem_pool pMemPool);

/**

 */
w_hash_index w_hash_next(w_hash_index pHashIndex);

/*

apr_hashfunc_default(const char* key, apr_ssize_t* klen)





void 	apr_hash_this(apr_hash_index_t* hi, const void** key, apr_ssize_t* klen, void** val)

const void* apr_hash_this_key(apr_hash_index_t* hi)

apr_ssize_t 	apr_hash_this_key_len(apr_hash_index_t* hi)

void* apr_hash_this_val(apr_hash_index_t* hi)



apr_hash_t* apr_hash_overlay(apr_pool_t* p, const apr_hash_t* overlay, const apr_hash_t* base)


int 	apr_hash_do(apr_hash_do_callback_fn_t* comp, void* rec, const apr_hash_t* ht)

apr_pool_t* apr_hash_pool_get(const apr_hash_t* thehash)*/

#ifdef __cplusplus
}
#endif
