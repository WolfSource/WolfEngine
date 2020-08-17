/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_mem_cache.h
    Description      : a wrapper over memcached
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include <memory/w_hash.h>

//forward declaration
typedef struct apr_memcache_server_t* w_mem_cache_server;
typedef struct apr_memcache_t* w_mem_cache;
typedef struct apr_memcache_value_t* w_mem_cache_value;
typedef struct apr_memcache_stats_t* w_mem_cache_stats;
typedef enum apr_memcache_server_status_t w_mem_cache_server_status;

W_SYSTEM_EXPORT
uint32_t   w_mem_cache_hash(_In_ w_mem_cache pMemCache, _In_z_ const char* pData, _In_ size_t pDataLen);
 
W_SYSTEM_EXPORT
uint32_t   w_mem_cache_hash_crc32(_In_ void* pBaton, _In_z_ const char* pData, _In_ size_t pDataLen);
 
W_SYSTEM_EXPORT
uint32_t   w_mem_cache_hash_default(_In_ void* pBaton, _In_z_ const char* pData, _In_ size_t pDataLen);
 
W_SYSTEM_EXPORT
w_mem_cache_server   w_mem_cache_find_server_hash(_In_ w_mem_cache pMemCache, _In_ uint32_t pHash);
 
W_SYSTEM_EXPORT
w_mem_cache_server   w_mem_cache_find_server_hash_default(_In_ void* pBaton,
                                                         _In_ w_mem_cache pMemCache,
                                                         _In_ uint32_t pHash);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_add_server(_In_ w_mem_cache pMemCache, _In_ w_mem_cache_server pMemCacheServer);
 
W_SYSTEM_EXPORT
w_mem_cache_server   w_mem_cache_find_server(_In_ w_mem_cache pMemCache,
                                             _In_z_ const char* pHost,
                                             _In_ uint16_t pPort);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_enable_server(_In_ w_mem_cache pMemCache, _In_ w_mem_cache_server pMemCacheServer);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_disable_server(_In_ w_mem_cache pMemCache, _In_ w_mem_cache_server pMemCacheServer);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_server_create(_In_ w_mem_pool pMemPool,
                                      _In_z_ const char* pHost,
                                      _In_ uint16_t pPort,
                                      _In_ uint32_t pMin,
                                      _In_ uint32_t pSMax,
                                      _In_ uint32_t pMax,
                                      _In_ uint32_t pTTL,
                                      _Inout_ w_mem_cache_server* pMemCacheServer);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_create(_In_ w_mem_pool pMemPool,
                               _In_ uint16_t pMaxServers,
                               _In_ uint32_t pFlags,
                               _Inout_ w_mem_cache* pMemCache);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_getp(_In_ w_mem_cache pMemCache,
                             _In_ w_mem_pool pMemPool,
                             _In_z_ const char* pKey,
                             _Inout_ char** pBaton,
                             _Inout_ size_t* pLen,
                             _Inout_ uint16_t* pFlags);

W_SYSTEM_EXPORT
void    w_mem_cache_add_multget_key(_In_ w_mem_pool pDataPool,
                                    _In_z_ const char* pKey,
                                    _Inout_ w_hash* pValues);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_multgetp(_In_ w_mem_cache pMemCache,
                                 _In_ w_mem_pool pTempPool,
                                 _In_ w_mem_pool pDataPool,
                                 _Inout_ w_hash pValues);

W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_set(_In_ w_mem_cache pMemCache,
                            _In_z_ const char* pKey,
                            _In_z_ char * pBaton,
                            _In_ size_t pDataSize,
                            _In_ uint32_t pTimeOut,
                            _In_ uint16_t pFlags);

W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_add(_In_ w_mem_cache pMemCache,
                            _In_z_ const char* pKey,
                            _In_z_ char* pBaton,
                            _In_ size_t pDataSize,
                            _In_ uint32_t pTimeOut,
                            _In_ uint16_t pFlags);

W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_replace(_In_ w_mem_cache pMemCache,
                                _In_z_ const char* pKey,
                                _In_z_ char* pBaton,
                                _In_ size_t pDataSize,
                                _In_ uint32_t pTimeOut,
                                _In_ uint16_t pFlags);

W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_delete(_In_ w_mem_cache pMemCache,
                               _In_z_ const char* pKey,
                               _In_ uint32_t pTimeOut);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_incr(_In_ w_mem_cache pMemCache,
                             _In_z_ const char* pKey,
                             _In_ int32_t pN,
                             _In_ uint32_t* PNV);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_decr(_In_ w_mem_cache pMemCache,
                             _In_z_ const char* pKey,
                             _In_ int32_t pN,
                             _In_ uint32_t* pNewValue);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_get_version(_In_ w_mem_cache_server pMemCacheServer,
                                    _In_ w_mem_pool pMemPool,
                                    _Inout_ char** pBaton);
 
W_SYSTEM_EXPORT
W_RESULT    w_mem_cache_get_stats(_In_ w_mem_cache_server pMemCacheServer,
                                  _In_ w_mem_pool pMemPool,
                                  _Inout_ w_mem_cache_stats* pStats);

#ifdef __cplusplus
}
#endif
