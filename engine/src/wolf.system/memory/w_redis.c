#include <apr-util/apr_redis.h>
#include "w_redis.h"

uint32_t w_redis_hash(_In_ w_redis pRedisClient,
                      _In_z_ const char* pData,
                      _In_ size_t pDataLen)
{
    if (!pRedisClient || pData || pDataLen == 0)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_hash");
        return 0;
    }
    return apr_redis_hash(pRedisClient, pData, pDataLen);
}

uint32_t w_redis_hash_crc32(_In_ void* pBaton,
                            _In_z_ const char* pData,
                            _In_ size_t pDataLen)
{
    if (!pBaton || pData || pDataLen == 0)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_hash_crc32");
        return 0;
    }
    return apr_redis_hash_crc32(pBaton, pData, pDataLen);
}

uint32_t w_redis_hash_default(_In_ void* pBaton,
                              _In_ const char* pData,
                              _In_ size_t pDataLen)
{
    if (!pBaton || pData || pDataLen == 0)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_hash_default");
        return 0;
    }
    return apr_redis_hash_default(pBaton, pData, pDataLen);
}

w_redis_server w_redis_find_server_hash(_In_ w_redis pRedisClient,
                                        _In_ uint32_t pHash)
{
    if (!pRedisClient || pHash == 0)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_find_server_hash");
        return NULL;
    }
    return apr_redis_find_server_hash(pRedisClient, pHash);
}

w_redis_server w_redis_find_server_hash_default(_In_ void* pBaton,
                                                _In_ w_redis pRedisClient,
                                                _In_ uint32_t pHash)
{
    if (!pRedisClient || pHash == 0)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_find_server_hash");
        return NULL;
    }
    
    return apr_redis_find_server_hash_default(pBaton, pRedisClient, pHash);
}

W_RESULT w_redis_add_server(_In_ w_redis pRedisClient,
                            _In_ w_redis_server pServer)
{
    if (!pRedisClient || !pServer)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_add_server");
        return W_FAILURE;
    }
    return apr_redis_add_server(pRedisClient, pServer);
}

w_redis_server w_redis_find_server(_In_ w_redis pRedisClient,
                                   _In_z_ const char* pHost,
                                   _In_ uint16_t pPort)
{
    if (!pRedisClient || !pHost)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_find_server");
        return NULL;
    }
    return apr_redis_find_server(pRedisClient, pHost, pPort);
}

W_RESULT w_redis_enable_server(_In_ w_redis pRedisClient,
                               _In_ w_redis_server pRedisServer)
{
    if (!pRedisClient || !pRedisServer)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_enable_server");
        return W_FAILURE;
    }
    return apr_redis_enable_server(pRedisClient, pRedisServer);
}

W_RESULT w_redis_disable_server(_In_ w_redis pRedisClient,
                                _In_ w_redis_server pRedisServer)
{
    if (!pRedisClient || !pRedisServer)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_disable_server");
        return W_FAILURE;
    }
    return apr_redis_disable_server(pRedisClient, pRedisServer);
}

W_RESULT w_redis_server_init(_In_ w_mem_pool pMemPool,
                             _In_z_ const char* pHost,
                             _In_ uint16_t pPort,
                             _In_ uint32_t pMin,
                             _In_ uint32_t pSMax,
                             _In_ uint32_t pMax,
                             _In_ uint32_t pTTL,
                             _In_ uint32_t pReadWriteTimeOut,
                             _Inout_ w_redis_server* pNewServerLocation)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_get_default_memory_pool();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_redis_server_create");
            return W_FAILURE;
        }
    }
    return apr_redis_server_create(_pool,
                                   pHost,
                                   pPort,
                                   pMin,
                                   pSMax,
                                   pMax,
                                   pTTL,
                                   pReadWriteTimeOut,
                                   pNewServerLocation);
}

W_RESULT w_redis_init(_In_ w_mem_pool pMemPool,
                      _In_ uint16_t pMaxServers,
                      _In_ uint32_t pFlags,
                      _Inout_ w_redis* pRedisClient)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_get_default_memory_pool();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_redis_create");
            return W_FAILURE;
        }
    }
    return apr_redis_create(_pool,
                            pMaxServers,
                            pFlags,
                            pRedisClient);
}

W_RESULT w_redis_get(_In_ w_redis pRedisClient,
                     _In_ w_mem_pool pMemPool,
                     _In_z_ const char* pKey,
                     _Inout_ char** pBaton,
                     _Inout_ size_t* pLen,
                     _Inout_ uint16_t* pFlags)
{
    if (!pKey)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_get");
        return W_FAILURE;
    }
    
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_get_default_memory_pool();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_redis_get");
            return W_FAILURE;
        }
    }
    return apr_redis_getp(pRedisClient,
                          _pool,
                          pKey,
                          pBaton,
                          pLen,
                          pFlags);
}

W_RESULT w_redis_set(_In_ w_redis pRedisClient,
                     _In_z_ const char* pKey,
                     _In_z_ char* pBaton,
                     _In_ size_t pDataSize,
                     _In_ uint16_t pFlags)
{
    if (!pRedisClient || !pKey || !pBaton)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_set");
        return W_FAILURE;
    }
    return apr_redis_set(pRedisClient,
                         pKey,
                         pBaton,
                         pDataSize,
                         pFlags);
}

W_RESULT w_redis_setex(_In_ w_redis pRedisClient,
                       _In_z_ const char* pKey,
                       _In_z_ char* pBaton,
                       _In_ size_t pDataSize,
                       _In_ uint32_t pTimeOut,
                       _In_ uint16_t pFlags)
{
    if (!pRedisClient || !pKey || !pBaton)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_setex");
        return W_FAILURE;
    }
    return apr_redis_setex(pRedisClient,
                           pKey,
                           pBaton,
                           pDataSize,
                           pTimeOut,
                           pFlags);
}

W_RESULT w_redis_delete(_In_ w_redis pRedisClient,
                        _In_z_ const char* pKey,
                        _In_ uint32_t pTimeOut)
{
    if (!pRedisClient || !pKey)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_delete");
        return W_FAILURE;
    }
    return apr_redis_delete(pRedisClient,
                            pKey,
                            pTimeOut);
}

W_RESULT w_redis_version(_In_ w_redis_server pRedisServer,
                         _In_ w_mem_pool pMemPool,
                         _Inout_ char** pBaton)
{
    if (!pRedisServer)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_delete");
        return W_FAILURE;
    }
    
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_get_default_memory_pool();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_redis_version");
            return W_FAILURE;
        }
    }
    
    return apr_redis_version(pRedisServer, _pool, pBaton);
}

W_RESULT w_redis_info(_In_ w_redis_server pRedisServer,
                      _In_ w_mem_pool pMemPool,
                      _Inout_ char** pBaton)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_get_default_memory_pool();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_redis_info");
            return W_FAILURE;
        }
    }
    
    return apr_redis_info(pRedisServer, _pool, pBaton);
}

W_RESULT w_redis_incr(_In_ w_redis pRedisClient,
                      _In_z_ const char* pKey,
                      _In_ int32_t pIncrementNumber,
                      _Inout_ uint32_t* pNewValue)
{
    if (!pRedisClient || !pKey)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_incr");
        return W_FAILURE;
    }
    
    return apr_redis_incr(pRedisClient,
                          pKey,
                          pIncrementNumber,
                          pNewValue);
}

W_RESULT w_redis_decr(_In_ w_redis pRedisClient,
                      _In_z_ const char* pKey,
                      _In_ int32_t pIncrementNumber,
                      _Inout_ uint32_t* pNewValue)
{
    if (!pRedisClient || !pKey)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_decr");
        return W_FAILURE;
    }
    
    return apr_redis_decr(pRedisClient,
                          pKey,
                          pIncrementNumber,
                          pNewValue);
}

W_RESULT w_redis_ping(_In_ w_redis_server pRedisServer)
{
    if (!pRedisServer)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_ping");
        return W_FAILURE;
    }
    return apr_redis_ping(pRedisServer);
}

W_RESULT w_redis_multiget(_In_ w_redis pRedisClient,
                          _In_ w_mem_pool pTempPool,
                          _In_ w_mem_pool pDataPool,
                          _Inout_ w_hash pValues)
{
    if (!pRedisClient)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_multiget");
        return W_FAILURE;
    }
    return apr_redis_multgetp(pRedisClient, pTempPool, pDataPool, pValues);
}

W_RESULT w_redis_get_stats(_In_ w_redis_server pRedisServer,
                           _In_ w_mem_pool pMemPool,
                           _Inout_ w_redis_stats* pStats)
{
    if (!pRedisServer)
    {
        W_ASSERT(false, "missing parameters!. trace info: w_redis_get_stats");
        return W_FAILURE;
    }
    
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_get_default_memory_pool();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_redis_get_stats");
            return W_FAILURE;
        }
    }
    
    return apr_redis_stats(pRedisServer, _pool, pStats);
}


