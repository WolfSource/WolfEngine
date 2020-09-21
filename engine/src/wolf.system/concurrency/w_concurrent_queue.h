 /*
     Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
     Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
     Website          : https://WolfEngine.App
     Name             : w_concurrent_queue.h
     Description      : Thread Safe FIFO bounded queue
     Comment          : since most implementations of the queue are backed by a condition
                        variable implementation, it isn't available on systems without threads.
                        Although condition variables are sometimes available without threads.
 */

#pragma once

#include "wolf.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct w_concurrent_queue_t w_concurrent_queue_t;
    typedef w_concurrent_queue_t* w_concurrent_queue;

    /**
     * create and initialize a FIFO queue
     * @param pMemPool a memory pool to allocate queue from
     * @param pQueue the new queue
     * @param pQueueCapacity the maximum size of the queue
     * @returns APR_BADARG on bad functions arguments
     * @returns APR_SUCCESS on a successful initialization
     */
    W_SYSTEM_EXPORT
        W_RESULT w_concurrent_queue_init(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_concurrent_queue* pQueue,
            _In_ uint32_t pQueueCapacity);

    /**
     * push/add an object to the queue, blocking if the queue is already full
     *
     * @param pQueue the queue
     * @param pData the data
     * @returns APR_BADARG on bad functions arguments
     * @returns APR_EINTR the blocking was interrupted (try again)
     * @returns APR_EOF the queue has been terminated
     * @returns APR_SUCCESS on a successful push
     */
    W_SYSTEM_EXPORT
        W_RESULT w_concurrent_queue_push(_Inout_ w_concurrent_queue pQueue, _In_ void* pData);

    /**
     * push/add an object to the queue, returning immediately if the queue is full
     * @param pQueue the queue
     * @param pData the data
     * @returns APR_BADARG on bad functions arguments
     * @returns APR_EINTR the blocking operation was interrupted (try again)
     * @returns APR_EAGAIN the queue is full
     * @returns APR_EOF the queue has been terminated
     * @returns APR_SUCCESS on a successful push
     */
    W_SYSTEM_EXPORT
        W_RESULT w_concurrent_queue_trypush(_Inout_ w_concurrent_queue pQueue, _In_ void* pData);

    /**
     * pop/get an object from the queue, blocking if the queue is already empty
     *
     * @param pQueue the queue
     * @param pData the data
     * @returns APR_BADARG on bad functions arguments
     * @returns APR_EINTR the blocking was interrupted (try again)
     * @returns APR_EOF if the queue has been terminated
     * @returns APR_SUCCESS on a successful pop
     */
    W_SYSTEM_EXPORT
        W_RESULT w_concurrent_queue_pop(_Inout_ w_concurrent_queue pQueue, _In_ void** pData);

    /**
     * pop/get an object to the queue, returning immediately if the queue is empty
     *
     * @param pQueue the queue
     * @param pData the data
     * @returns APR_BADARG on bad functions arguments
     * @returns APR_EINTR the blocking operation was interrupted (try again)
     * @returns APR_EAGAIN the queue is empty
     * @returns APR_EOF the queue has been terminated
     * @returns APR_SUCCESS on a successful pop
     */
    W_SYSTEM_EXPORT
        W_RESULT w_concurrent_queue_trypop(_Inout_ w_concurrent_queue pQueue, _In_ void** pData);

    /**
     * returns the size of the queue.
     * @warning this is not threadsafe, and is intended for reporting/monitoring
     * of the queue.
     * @param pQueue the queue
     * @returns the size of the queue
     */
    W_SYSTEM_EXPORT
        uint32_t w_concurrent_queue_size(_In_ w_concurrent_queue pQueue);

    /**
     * returns the size of the queue. this function is threadsafe
     * @param pQueue the queue
     * @returns the size of the queue
     */
    W_SYSTEM_EXPORT
        uint32_t w_concurrent_queue_size_threadsafe(_In_ w_concurrent_queue pQueue);

    /**
     * interrupt all the threads blocking on this queue.
     * @param pQueue the queue
     * @returns APR_BADARG on bad functions arguments
     * @returns APR_SUCCESS on a successful interrupt
     */
    W_SYSTEM_EXPORT
        W_RESULT w_concurrent_queue_interrupt_all(_In_ w_concurrent_queue pQueue);

    /**
     * terminate the queue, sending an interrupt to all the blocking threads
     * @param pQueue the queue
     * @returns APR_BADARG on bad functions arguments
     * @returns APR_SUCCESS on a successful interrupt
     */
    W_SYSTEM_EXPORT
        W_RESULT w_concurrent_queue_fini(_In_ w_concurrent_queue pQueue);

#ifdef __cplusplus
}
#endif

