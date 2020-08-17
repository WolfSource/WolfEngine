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
typedef struct apr_hash_index_t* w_hash_index_t;

typedef unsigned int(*w_hash_custom_fn)(const char* /*pKey*/, size_t* /*pLen*/);
typedef int(w_hash_do_callback_fn)(void* /*pRec*/, const void* /*pKey*/, size_t /*pLen*/, const void* /*pValue*/);

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

/*
TODO : iterate


apr_hashfunc_default(const char* key, apr_ssize_t* klen)



apr_hash_t* apr_hash_copy(apr_pool_t* pool, const apr_hash_t* h)



apr_hash_index_t* apr_hash_first(apr_pool_t* p, apr_hash_t* ht)

apr_hash_index_t* apr_hash_next(apr_hash_index_t* hi)

void 	apr_hash_this(apr_hash_index_t* hi, const void** key, apr_ssize_t* klen, void** val)

const void* apr_hash_this_key(apr_hash_index_t* hi)

apr_ssize_t 	apr_hash_this_key_len(apr_hash_index_t* hi)

void* apr_hash_this_val(apr_hash_index_t* hi)



apr_hash_t* apr_hash_overlay(apr_pool_t* p, const apr_hash_t* overlay, const apr_hash_t* base)

apr_hash_t* apr_hash_merge(apr_pool_t* p, const apr_hash_t* h1, const apr_hash_t* h2, void* (*merger)(apr_pool_t* p, const void* key, apr_ssize_t klen, const void* h1_val, const void* h2_val, const void* data), const void* data)

int 	apr_hash_do(apr_hash_do_callback_fn_t* comp, void* rec, const apr_hash_t* ht)

apr_pool_t* apr_hash_pool_get(const apr_hash_t* thehash)*/

#ifdef __cplusplus
}
#endif
