/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_concurrent_queue.h
    Description      : thread safe FIFO queue
    Comment          :
*/


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

typedef struct apr_queue_t* w_concurrent_queue;

/**
 * create a thread safe FIFO queue
 * @param pQueue The new queue
 * @param pQueueCapacity maximum size of the queue
 * @param pMemPool pool to allocate queue from
*/
W_RESULT  w_concurrent_queue_init(_Inout_ w_concurrent_queue* pQueue,
                                  _In_ uint32_t pQueueCapacity,
                                  _In_ w_mem_pool pMemPool);

/**
 * push/add an object to the queue, blocking if the queue is already full
 *
 * @param pQueue the queue
 * @param pData the data
 * @returns APR_EINTR the blocking was interrupted (try again)
 * @returns APR_EOF the queue has been terminated
 * @returns W_SUCCESS on a successful push
*/
W_RESULT  w_concurrent_queue_push(_In_ w_concurrent_queue pQueue, _In_ void* pData);

/**
 * pop/get an object from the queue, blocking if the queue is already empty
 *
 * @param pQueue the queue
 * @param pData the data
 * @returns APR_EINTR the blocking was interrupted (try again)
 * @returns APR_EOF if the queue has been terminated
 * @returns W_SUCCESS on a successful pop
*/
W_RESULT  w_concurrent_queue_pop(_In_ w_concurrent_queue pQueue, _Inout_ void** pData);

/**
 * push/add an object to the queue, returning immediately if the queue is full
 *
 * @param pQueue the queue
 * @param pData the data
 * @returns APR_EINTR the blocking operation was interrupted (try again)
 * @returns APR_EAGAIN the queue is full
 * @returns APR_EOF the queue has been terminated
 * @returns W_SUCCESS on a successful push
*/
W_RESULT  w_concurrent_queue_trypush(_In_ w_concurrent_queue pQueue, _In_ void* pData);

/**
 * pop/get an object to the queue, returning immediately if the queue is empty
 *
 * @param pQueue the queue
 * @param pData the data
 * @returns APR_EINTR the blocking operation was interrupted (try again)
 * @returns APR_EAGAIN the queue is empty
 * @returns APR_EOF the queue has been terminated
 * @returns W_SUCCESS on a successful pop
*/
W_RESULT  w_concurrent_queue_trypop(_In_ w_concurrent_queue pQueue, _Inout_ void** pData);

/**
 * returns the size of the queue.
 *
 * @warning this is not threadsafe, and is intended for reporting/monitoring
 * of the queue.
 * @param pQueue the queue
 * @returns the size of the queue
*/
uint32_t  w_concurrent_queue_size(_In_ w_concurrent_queue pQueue);

/**
 * interrupt all the threads blocking on this queue.
 * @param pQueue the queue
 * @return result
*/
W_RESULT  w_concurrent_queue_interrupt_all(_In_ w_concurrent_queue pQueue);

/**
 * terminate the queue, sending an interrupt to all the blocking threads
 * @param pQueue the queue
 * @return result
*/
W_RESULT  w_concurrent_queue_term(_In_ w_concurrent_queue pQueue);

#ifdef __cplusplus
}
#endif
