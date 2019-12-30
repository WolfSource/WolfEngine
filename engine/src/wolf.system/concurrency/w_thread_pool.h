/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_thread_pool.h
    Description      : a simple thread based on https://docs.oracle.com/cd/E19253-01/816-5137/ggfbr/index.html
    Comment          :
*/

#pragma once

#ifdef __cpluspus
extern "C" {
#endif

#include "wolf.h"

struct w_thread_pool;

/**
 * create a thread
 * @param pMinThreads minimum thread of this queue
 * @param pMaxThreads maximum thread of this queue
 * @param pLinger linger
 * @param pAttr thread attributes
*/
struct w_thread_pool* w_thread_pool_create(_In_ const uint32_t pMinThreads,
                                           _In_ const uint32_t pMaxThreads,
                                           _In_ const uint32_t pLinger,
                                           _In_ const pthread_attr_t* pAttr);

/**
 * create a thread
 * @param pPool pointer to thread pool
 * second parameter is job function
 * @param pArg argument of job
 * @return result which is an integer, 0 means ok
*/
int  w_thread_pool_add_job(struct w_thread_pool* pPool, void *(*func)(void*), void* pArg);

/**
 * wait for all threads
 * @param pPool pointer to thread pool
*/
void  w_thread_pool_wait_all(struct w_thread_pool* pPool);

/**
 * cancel all threads and destroy them
 * @param pPool pointer to thread pool
*/
void  w_thread_pool_destroy(struct w_thread_pool* pPool);


#ifdef __cpluspus
}
#endif
