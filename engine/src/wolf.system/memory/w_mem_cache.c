#include <apr-util/apr_memcache.h>
#include "w_mem_cache.h"

uint32_t   w_mem_cache_hash(_In_ w_mem_cache pMemCache, _In_z_ const char* pData, _In_ const size_t pDataLen)
{
    return apr_memcache_hash(pMemCache, pData, pDataLen);
}
 
uint32_t   w_mem_cache_hash_crc32(_In_ void* pBaton, _In_z_ const char* pData, _In_ const size_t pDataLen)
{
    return apr_memcache_hash_crc32(pBaton, pData, pDataLen);
}
 
uint32_t   w_mem_cache_hash_default(_In_ void* pBaton, _In_z_ const char* pData, _In_ const size_t pDataLen)
{
    return apr_memcache_hash_default(pBaton, pData, pDataLen);
}
 
w_mem_cache_server   w_mem_cache_find_server_hash(_In_ w_mem_cache pMemCache, _In_ const uint32_t pHash)
{
    return apr_memcache_find_server_hash(pMemCache, pHash);
}
 
w_mem_cache_server   w_mem_cache_find_server_hash_default(_In_ void* pBaton,
                                                         _In_ w_mem_cache pMemCache,
                                                         _In_ const uint32_t pHash)
{
    return apr_memcache_find_server_hash_default(pBaton, pMemCache, pHash);
}
 
W_RESULT    w_mem_cache_add_server(_In_ w_mem_cache pMemCache, _In_ w_mem_cache_server pMemCacheServer)
{
    return apr_memcache_add_server(pMemCache, pMemCacheServer);
}
 
w_mem_cache_server   w_mem_cache_find_server(_In_ w_mem_cache pMemCache,
                                             _In_z_ const char* pHost,
                                             _In_ uint16_t pPort)
{
    return apr_memcache_find_server(pMemCache, pHost, pPort);
}
 
W_RESULT    w_mem_cache_enable_server(_In_ w_mem_cache pMemCache, _In_ w_mem_cache_server pMemCacheServer)
{
    return apr_memcache_enable_server(pMemCache, pMemCacheServer);
}
 
W_RESULT    w_mem_cache_disable_server(_In_ w_mem_cache pMemCache, _In_ w_mem_cache_server pMemCacheServer)
{
    return apr_memcache_disable_server(pMemCache, pMemCacheServer);
}
 
W_RESULT    w_mem_cache_server_create(_In_ w_mem_pool pMemPool,
                                      _In_z_ const char* pHost,
                                      _In_ uint16_t pPort,
                                      _In_ uint32_t pMin,
                                      _In_ uint32_t pSMax,
                                      _In_ uint32_t pMax,
                                      _In_ uint32_t pTTL,
                                      _Inout_ w_mem_cache_server* pMemCacheServer)
{
    return apr_memcache_server_create(pMemPool, pHost, pPort, pMin, pSMax, pMax, pTTL, pMemCacheServer);
}
 
W_RESULT    w_mem_cache_create(_In_ w_mem_pool pMemPool,
                               _In_ uint16_t pMaxServers,
                               _In_ uint32_t pFlags,
                               _Inout_ w_mem_cache* pMemCache)
{
    return apr_memcache_create(pMemPool, pMaxServers, pFlags, pMemCache);
}
 
W_RESULT    w_mem_cache_getp(_In_ w_mem_cache pMemCache,
                             _In_ w_mem_pool pMemPool,
                             _In_z_ const char* pKey,
                             _Inout_ char** pBaton,
                             _Inout_ size_t* pLen,
                             _Inout_ uint16_t* pFlags)
{
    return apr_memcache_getp(pMemCache, pMemPool, pKey, pBaton, pLen, pFlags);
}

void    w_mem_cache_add_multget_key(_In_ w_mem_pool pDataPool,
                                    _In_z_ const char* pKey,
                                    _Inout_ w_hash* pValues)
{
    apr_memcache_add_multget_key(pDataPool, pKey, pValues);
}
 
W_RESULT    w_mem_cache_multgetp(_In_ w_mem_cache pMemCache,
                                 _In_ w_mem_pool pTempPool,
                                 _In_ w_mem_pool pDataPool,
                                 _Inout_ w_hash pValues)
{
    return apr_memcache_multgetp(pMemCache, pTempPool, pDataPool, pValues);
}

W_RESULT    w_mem_cache_set(_In_ w_mem_cache pMemCache,
                            _In_z_ const char* pKey,
                            _In_z_ char * pBaton,
                            _In_ const size_t pDataSize,
                            _In_ uint32_t pTimeOut,
                            _In_ uint16_t pFlags)
{
    return apr_memcache_set(pMemCache, pKey, pBaton, pDataSize, pTimeOut, pFlags);
}

W_RESULT    w_mem_cache_add(_In_ w_mem_cache pMemCache,
                            _In_z_ const char* pKey,
                            _In_z_ char* pBaton,
                            _In_ const size_t pDataSize,
                            _In_ uint32_t pTimeOut,
                            _In_ uint16_t pFlags)
{
    return apr_memcache_add(pMemCache, pKey, pBaton, pDataSize, pTimeOut, pFlags);
}

W_RESULT    w_mem_cache_replace(_In_ w_mem_cache pMemCache,
                                _In_z_ const char* pKey,
                                _In_z_ char* pBaton,
                                _In_ const size_t pDataSize,
                                _In_ uint32_t pTimeOut,
                                _In_ uint16_t pFlags)
{
    return apr_memcache_replace(pMemCache, pKey, pBaton, pDataSize, pTimeOut, pFlags);
}

W_RESULT    w_mem_cache_delete(_In_ w_mem_cache pMemCache,
                               _In_z_ const char* pKey,
                               _In_ uint32_t pTimeOut)
{
    return apr_memcache_delete(pMemCache, pKey, pTimeOut);
}
 
W_RESULT    w_mem_cache_incr(_In_ w_mem_cache pMemCache,
                             _In_z_ const char* pKey,
                             _In_ int32_t pN,
                             _In_ uint32_t* PNV)
{
    return apr_memcache_incr(pMemCache, pKey, pN, PNV);
}
 
W_RESULT    w_mem_cache_decr(_In_ w_mem_cache pMemCache,
                             _In_z_ const char* pKey,
                             _In_ int32_t pN,
                             _In_ uint32_t* pNewValue)
{
    return apr_memcache_decr(pMemCache, pKey, pN, pNewValue);
}
 
W_RESULT    w_mem_cache_get_version(_In_ w_mem_cache_server pMemCacheServer,
                                    _In_ w_mem_pool pMemPool,
                                    _Inout_ char** pBaton)
{
    return apr_memcache_version(pMemCacheServer, pMemPool, pBaton);
}
 
W_RESULT    w_mem_cache_get_stats(_In_ w_mem_cache_server pMemCacheServer,
                                  _In_ w_mem_pool pMemPool,
                                  _Inout_ w_mem_cache_stats* pStats)
{
    return apr_memcache_stats(pMemCacheServer, pMemPool, (apr_memcache_stats_t**)pStats);
}
