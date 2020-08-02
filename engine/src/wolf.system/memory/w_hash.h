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

typedef unsigned int(*w_hash_fn)(const char* /*pKey*/, size_t* /*pLen*/);
typedef int(w_hash_do_callback_fn)(void* /*pRec*/, const void* /*pKey*/, size_t /*pLen*/, const void* /*pValue*/);

/*
apr_hashfunc_default(const char* key, apr_ssize_t* klen)

apr_hash_t* apr_hash_make(apr_pool_t* pool)

apr_hash_t* apr_hash_make_custom(apr_pool_t* pool, apr_hashfunc_t hash_func)

apr_hash_t* apr_hash_copy(apr_pool_t* pool, const apr_hash_t* h)

void 	apr_hash_set(apr_hash_t* ht, const void* key, apr_ssize_t klen, const void* val)

void* apr_hash_get(apr_hash_t* ht, const void* key, apr_ssize_t klen)

apr_hash_index_t* apr_hash_first(apr_pool_t* p, apr_hash_t* ht)

apr_hash_index_t* apr_hash_next(apr_hash_index_t* hi)

void 	apr_hash_this(apr_hash_index_t* hi, const void** key, apr_ssize_t* klen, void** val)

const void* apr_hash_this_key(apr_hash_index_t* hi)

apr_ssize_t 	apr_hash_this_key_len(apr_hash_index_t* hi)

void* apr_hash_this_val(apr_hash_index_t* hi)

unsigned int 	apr_hash_count(apr_hash_t* ht)

void 	apr_hash_clear(apr_hash_t* ht)

apr_hash_t* apr_hash_overlay(apr_pool_t* p, const apr_hash_t* overlay, const apr_hash_t* base)

apr_hash_t* apr_hash_merge(apr_pool_t* p, const apr_hash_t* h1, const apr_hash_t* h2, void* (*merger)(apr_pool_t* p, const void* key, apr_ssize_t klen, const void* h1_val, const void* h2_val, const void* data), const void* data)

int 	apr_hash_do(apr_hash_do_callback_fn_t* comp, void* rec, const apr_hash_t* ht)

apr_pool_t* apr_hash_pool_get(const apr_hash_t* thehash)*/

#ifdef __cplusplus
}
#endif
