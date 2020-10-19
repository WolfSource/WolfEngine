#include <concurrency/libev/ev.h>
#include<wolf.h>
#include"db/w_redis.h"
#include <chrono/w_chrono.h>
#include <chrono/w_gametime.h>
#include<compression/w_compress.h>
#include"concurrency/w_thread.h"
#include<concurrency/w_async.h>
#include<concurrency/w_concurrent_queue.h>
#include<memory/w_array.h>
#include<memory/w_hash.h>
#include<memory/w_table.h>
#include<concurrency/w_condition_variable.h>
#include<concurrency/w_mutex.h>
#include<concurrency/w_thread_pool.h>
#include<concurrency/w_atomic.h>
#include<concurrency/w_atomic.h>
#include<log/w_log.h>
#include"io/w_io.h"
#include"script/w_lua.h"
#include<iostream>



void mycallback(EV_P_ w_async_base* arg1, int arg2) { printf("%s", "ok"); }
void* _hash_func(w_apr_pool pMemPool, const void* pKey, long long pLen, const void* pHash1Value, const void* pHash2Value, const void* pData)
{
    printf("%s", "ok");
    return NULL;

}

int hash_do_callback_fn(void* pRec, const void* pKey, long long pLen, const void* pValue)
{
    printf("%s", "ok2");
    return 1;
}

void* w_thread_job_my(w_thread arg1, void* arg2) { 
    printf("%s", "okthresd");
    return NULL; }
void mycallback_thread() { printf("%s", "okthresd"); }

void g_tick_callback(w_gametime w_game) {
    printf("%s", "ok");
 }

int pHashCustomFunc_tmp(const char* , long long* )
{
    printf("%s", "ok_hash_func");
    return 0;
}

int pCallBack(void* rec, const char* pKey, const char* pValue) {
    const char* label =(const char*) rec;
    printf("callback[%s]: %s %s\n", label, pKey, pValue);
    return TRUE;
}
int displayLuaFunction(lua_State* l)
{
    // number of input arguments
    int argc = lua_gettop(l);

    // print input arguments
    std::cout << "[C++] Function called from Lua with " << argc
        << " input arguments" << std::endl;
    for (int i = 0; i < argc; i++)
    {
        std::cout << " input argument #" << argc - i << ": "
            << lua_tostring(l, lua_gettop(l)) << std::endl;
        lua_pop(l, 1);
    }

    // push to the stack the multiple return values
    std::cout << "[C++] Returning some values" << std::endl;
    double value_1 = 3.141592;
    int pValueType = 3;
    W_RESULT _lua_set_parameter_function = w_lua_set_parameter_function((void*)&value_1, pValueType);

    const char * value_2 = "See you space cowboy";
    int pValueType_2 = 4;
    W_RESULT _lua_set_parameter_function2 = w_lua_set_parameter_function((void*)value_2, pValueType_2);

    // number of return values
    return 2;
}

