#include "convey.h"
#include <wolf.h>
#include <chrono/w_timespan.h>
#include <chrono/w_chrono.h>
#include <chrono/w_gametime.h>
#include <time.h>
#include <compression/w_compress.h>
#include <concurrency/w_concurrent_queue.h>
#include <concurrency/w_condition_variable.h>
#include <concurrency/w_thread.h>
#include <concurrency/w_thread_pool.h>
#include <concurrency/w_timer.h>
#include <concurrency/w_async.h>
#include <concurrency/libev/ev.h>
#include <io/w_io.h>
#include <time.h>
#include <memory/w_table.h>
#include <memory/w_array.h>
#include <memory/w_hash.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include"memory/w_redis.h"
#include"memory/w_mem_pool.h"

#define W_STATUS_IS_NOTFOUND 70015

Main({
    //initialize wolf
    wolf_init();
Convey("w_redis", {

printf("testing w_redis");
w_mem_pool _mem_pool = NULL;
int x = w_mem_pool_init(&_mem_pool);
w_redis_server pRedisServer = w_malloc(_mem_pool, sizeof(w_redis_server));


const char* pHost = "localhost";
uint16_t pPort = 6379;
uint32_t pMin = 0;
uint32_t pMax = 1;
uint32_t pTTL = 60;
uint32_t pReadWriteTimeOut = 60;
w_redis_server  pNewServerLocation;
uint16_t pMaxServers = 10;
uint32_t pFlags = 0;
char* result;
size_t pLen = NULL;
char* result_2;
size_t pLen_2 = NULL;
w_redis_stats stats;
int32_t pIncrementNumber = 1;
uint32_t pNewValue = NULL;
void* v = NULL;
const char* key = NULL;
w_redis pRedisClient;

W_RESULT _redis_init = w_redis_init(_mem_pool, pMaxServers, pFlags, &pRedisClient);
So(_redis_init == 0);

const char prefix[] = "testredis";
const char prefix2[] = "testredis2";



W_RESULT _redis_server_init = w_redis_server_init(_mem_pool, pHost, pPort, pMin, pMax, pMax, pTTL, pReadWriteTimeOut, &pNewServerLocation);
So(_redis_server_init == 0);

 W_RESULT _redis_add_server = w_redis_add_server(pRedisClient, pNewServerLocation);
So(_redis_add_server == 0);

W_RESULT  _redis_get_stats = w_redis_get_stats(_mem_pool, pNewServerLocation, &stats);

if (_redis_get_stats == W_STATUS_IS_NOTFOUND) {
    Printf(" unable to find the socket in the poll structure");
}
So(_redis_get_stats == 0);

W_RESULT _redis_ping = w_redis_ping(pNewServerLocation);
So(_redis_ping == 0);


W_RESULT _redis_setex = w_redis_setex(pRedisClient, prefix2, "21", strlen("21"), 10, 27);
So(_redis_setex == 0);

W_RESULT _redis_get2 = w_redis_get(_mem_pool, pRedisClient, prefix2, &result_2, &pLen_2, NULL);
So(_redis_get2 == 0);

w_redis_server _redis_find_server = w_redis_find_server(pRedisClient, pHost, pPort);
So(_redis_find_server == 0);


W_RESULT redis_set = w_redis_set(pRedisClient, prefix, "271", sizeof("271") - 1, 27);
So(redis_set == 0);


W_RESULT _redis_get = w_redis_get(_mem_pool, pRedisClient, prefix, &result, &pLen, NULL);
So(_redis_get == 0);


W_RESULT _redis_delete = w_redis_delete(pRedisClient, prefix, 0);
So(_redis_delete == 0);

w_hash pValues = malloc(sizeof(w_hash));
w_mem_pool pTempPool = NULL;
int t1 = w_mem_pool_init(&pTempPool);
w_mem_pool pTempPool2 = NULL;
int t = w_mem_pool_init(&pTempPool2);

W_RESULT redis_disable_server = w_redis_disable_server(pRedisClient, _redis_find_server);
So(redis_disable_server == 0);

W_RESULT _redis_enable_server = w_redis_enable_server(pRedisClient, _redis_find_server);
So(_redis_enable_server == 0); 


    });

    wolf_fini();

    printf("\n\nAll tests where completed successfully.\n\n");

    })
