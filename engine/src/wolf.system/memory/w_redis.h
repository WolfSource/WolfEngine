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
typedef apr_redis_t* w_redis;
typedef apr_redis_server_t* w_redis_server;
typedef apr_redis_stats_t* w_redis_stats;

/**
 * Creates a CRC32 hash used to split keys between servers
 * @param pRedisClient The redis client object to use
 * @param pData Data to be hashed
 * @param pDataLen Length of the data to use
 * @return CRC32 hash of data
 * @remark The CRC32 hash is not compatible with old redisd clients.
 */
uint32_t w_redis_hash(_In_ w_redis pRedisClient,
                      _In_z_ const char* pData,
                      _In_ const size_t pDataLen);

/**
 * Creates pure CRC32 Hash. Used by some clients
 * @param pBaton The location
 * @param pData Data to be hashed
 * @param pDataLen Length of the data to use
 * @return pure CRC32 hash of data
*/
uint32_t w_redis_hash_crc32(_In_ void* pBaton,
                            _In_z_ const char* pData,
                            _In_ const size_t pDataLen);

/**
 * hash compatible with the standard Perl Client.
 * @param pBaton The location
 * @param pData Data to be hashed
 * @param pDataLen Length of the data to use
 * @return hash
*/
uint32_t w_redis_hash_default(_In_ void* pBaton,
                              _In_ const char* pData,
                              _In_ const size_t pDataLen);

/**
 * Picks a server based on a hash
 * @param pRedisClient The redis client object to use
 * @param pHash Hashed value of a Key
 * @return server that controls specified hash
 */
w_redis_server w_redis_find_server_hash(_In_ w_redis pRedisClient,
                                          _In_ const uint32_t pHash);

/**
 * Server selection compatible with the standard Perl Client.
 * @param pBaton The location
 * @param pRedisClient The redis client object to use
 * @param pHash the specific hash
 * @return server that controls specified hash
*/
w_redis_server w_redis_find_server_hash_default(_In_ void* pBaton,
                                                _In_ w_redis pRedisClient,
                                                _In_ const uint32_t pHash);

/**
 * Adds a server to a client object
 * @param pRedisClient The redis client object to use
 * @param pServer Server to add
 * @return result code
 * @remark Adding servers is not thread safe, and should be done once at startup.
 * @warning Changing servers after startup may cause keys to go to different servers.
 */
W_RESULT w_redis_add_server(_In_ w_redis pRedisClient,
                            _In_ w_redis_server pServer);


/**
 * Finds a Server object based on a hostname/port pair
 * @param pRedisClient The redis client object to use
 * @param pHost Hostname of the server
 * @param pPort Port of the server
 * @return Server with matching Hostname and Port, or NULL if none was found.
 */
w_redis_server w_redis_find_server(_In_ w_redis pRedisClient,
                                   _In_z_ const char* pHost,
                                   _In_ uint16_t pPort);

/**
 * Enables a Server for use again
 * @param pRedisClient The redis client object to use
 * @param pRedisServer Server to Activate
 * @return result code
 */
W_RESULT w_redis_enable_server(_In_ w_redis pRedisClient,
                               _In_ w_redis_server pRedisServer);

/**
 * Disable a Server
 * @param pRedisClient The redis client object to use
 * @param pRedisServer Server to Disable
 * @return result code
 */
W_RESULT w_redis_disable_server(_In_ w_redis pRedisClient,
                                _In_ w_redis_server pRedisServer);

/**
 * Creates a new Server Object
 * @param pMemPool Pool to use
 * @param pHost hostname of the server
 * @param pPort port of the server
 * @param pMin  minimum number of client sockets to open
 * @param pSMax soft maximum number of client connections to open
 * @param pMax  hard maximum number of client connections
 * @param pTTL  time to live in microseconds of a client connection
 * @param pReadWriteTimeOut read/write timeout value in seconds of a client connection
 * @param pNewServerLocation location of the new server object
 * @return result code
 * @remark pMin, pSMax, and pMax are only used when APR_HAS_THREADS
 */
W_RESULT w_redis_server_init(_In_ w_mem_pool pMemPool,
                             _In_z_ const char* pHost,
                             _In_ uint16_t pPort,
                             _In_ uint32_t pMin,
                             _In_ uint32_t pSMax,
                             _In_ uint32_t pMax,
                             _In_ uint32_t pTTL,
                             _In_ uint32_t pReadWriteTimeOut,
                             _Inout_ w_redis_server* pNewServerLocation);
/**
 * Creates a new redisd client object
 * @param pMemPool Pool to use
 * @param pMaxServers maximum number of servers
 * @param pFlags Not currently used
 * @param pRedisClient  location of the new redis client object
 * @return result code
 */
W_RESULT w_redis_init(_In_ w_mem_pool pMemPool,
                      _In_ uint16_t pMaxServers,
                      _In_ uint32_t pFlags,
                      _Inout_ w_redis* pRedisClient);

/**
 * Gets a value from the server
 * @param pRedisClient client to use
 * @param pMemPool Pool to use
 * @param pKey null terminated string containing the key
 * @param pBaton location of the allocated value
 * @param pLen   length of data at baton
 * @param pFlags any flags set by the client for this key
 * @return result code
 */
W_RESULT w_redis_get(_In_ w_redis pRedisClient,
                     _In_ w_mem_pool pMemPool,
                     _In_z_ const char* pKey,
                     _Inout_ char ** pBaton,
                     _Inout_ size_t* pLen,
                     _Inout_ uint16_t* pFlags);

/**
 * Sets a value by key on the server
 * @param pRedisClient client to use
 * @param pKey   null terminated string containing the key
 * @param pBaton data to store on the server
 * @param pDataSize   length of data at baton
 * @param pFlags any flags set by the client for this key
 */
W_RESULT w_redis_set(_In_ w_redis pRedisClient,
                     _In_z_ const char* pKey,
                     _In_z_ char* pBaton,
                     _In_ const size_t pDataSize,
                     _In_ uint16_t pFlags);

/**
 * Sets a value by key on the server
 * @param pRedisClient client to use
 * @param pKey   null terminated string containing the key
 * @param pBaton data to store on the server
 * @param pDataSize   length of data at baton
 * @param pTimeOut time in seconds for the data to live on the server
 * @param pFlags any flags set by the client for this key
 * @return result code
 */
W_RESULT w_redis_setex(_In_ w_redis pRedisClient,
                       _In_z_ const char* pKey,
                       _In_z_ char* pBaton,
                       _In_ const size_t pDataSize,
                       _In_ uint32_t pTimeOut,
                       _In_ uint16_t pFlags);

/**
 * Deletes a key from a server
 * @param pRedisClient client to use
 * @param pKey   null terminated string containing the key
 * @param pTimeOut time for the delete to stop other clients from adding
 * @return result code
 */
W_RESULT w_redis_delete(_In_ w_redis pRedisClient,
                        _In_z_ const char* pKey,
                        _In_ uint32_t pTimeOut);

/**
 * Query a server's version
 * @param pRedisServer server to query
 * @param pMemPool Pool to allocate answer from
 * @param pBaton location to store server version string
 * @return result code
 */
W_RESULT w_redis_version(_In_ w_redis_server pRedisServer,
                         _In_ w_mem_pool pMemPool,
                         _Inout_ char** pBaton);

/**
 * Query a server's INFO
 * @param pRedisServer server to query
 * @param pMemPool Pool to allocate answer from
 * @param pBaton location to store server INFO response string
 * @return result code
 */
W_RESULT w_redis_info(_In_ w_redis_server pRedisServer,
                      _In_ w_mem_pool pMemPool,
                      _Inout_ char** pBaton);

/**
 * Increments a value
 * @param pRedisClient client to use
 * @param pKey null terminated string containing the key
 * @param pIncrementNumber number to increment by
 * @param pNewValue new value after incrementing
 * @return result code
 */
W_RESULT w_redis_incr(_In_ w_redis pRedisClient,
                      _In_z_ const char* pKey,
                      _In_ int32_t pIncrementNumber,
                      _Inout_ uint32_t* pNewValue);
/**
 * Decrements a value
 * @param pRedisClient client to use
 * @param pKey null terminated string containing the key
 * @param pIncrementNumber  number to decrement by
 * @param pNewValue new value after decrementing
 * @return result code
 */
W_RESULT w_redis_decr(_In_ w_redis pRedisClient,
                      _In_z_ const char* pKey,
                      _In_ int32_t pIncrementNumber,
                      _Inout_ uint32_t* pNewValue);

/**
 * Pings the server
 * @param pRedisServer Server to ping
 * @return result code
 */
W_RESULT w_redis_ping(_In_ w_redis_server pRedisServer);

/**
 * Gets multiple values from the server, allocating the values out of p
 * @param pRedisClient client to use
 * @param pTempPool Pool used for temporary allocations. May be cleared inside this call.
 * @param pDataPool Pool used to allocate data for the returned values.
 * @param pValues hash of apr_redis_value_t keyed by strings, contains the result of the multiget call.
 * @return result code
 */
W_RESULT w_redis_multiget(_In_ w_redis pRedisClient,
                          _In_ w_mem_pool pTempPool,
                          _In_ w_mem_pool pDataPool,
                          _Inout_ w_hash pValues);

/**
 * Query a server for statistics
 * @param pRedisServer server to query
 * @param pMemPool memory pool to allocate answer from
 * @param pStats location of the new statistics structure
 */
W_RESULT w_redis_get_stats(_In_ w_redis_server pRedisServer,
                           _In_ w_mem_pool pMemPool,
                           _Inout_ w_redis_stats* pStats);


#ifdef __cplusplus
}
#endif
