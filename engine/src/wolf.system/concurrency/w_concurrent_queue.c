/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed
 * with this work for additional information regarding copyright
 * ownership.  The ASF licenses this file to you under the Apache
 * License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License.  You may obtain a copy of
 * the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 */


#include "w_concurrent_queue.h"
#include <apr-2/apu.h>
#include <apr-2/apr_errno.h>
#include <apr-2/apr_portable.h>
#include <apr-2/apr_thread_mutex.h>
#include <apr-2/apr_thread_cond.h>
#include <apr-2/apr_errno.h>
#include <apr-2/apr_pools.h>

/* 
 * define this to get debug messages
 *
#define QUEUE_DEBUG
 */

struct w_concurrent_queue_t
{
    void** data;
    unsigned int        nelts; /**< # elements */
    unsigned int        in;    /**< next empty location */
    unsigned int        out;   /**< next filled location */
    unsigned int        bounds;/**< max size of queue */
    unsigned int        full_waiters;
    unsigned int        empty_waiters;
    apr_thread_mutex_t* one_big_mutex;
    apr_thread_cond_t* not_empty;
    apr_thread_cond_t* not_full;
    int                 terminated;
};

#ifdef W_CONCURRENT_QUEUE_DEBUG
static void Q_DBG(char*msg, apr_queue_t *q) 
{
    fprintf(stderr, "%ld\t#%d in %d out %d\t%s\n", 
                    apr_os_thread_current(),
                    q->nelts, q->in, q->out,
                    msg
                    );
}
#else
#define Q_DBG(x,y) 
#endif

/**
 * Detects when the apr_queue_t is full. This utility function is expected
 * to be called from within critical sections, and is not threadsafe.
 */
#define apr_queue_full(queue) ((queue)->nelts == (queue)->bounds)

/**
 * Detects when the apr_queue_t is empty. This utility function is expected
 * to be called from within critical sections, and is not threadsafe.
 */
#define apr_queue_empty(queue) ((queue)->nelts == 0)

/**
 * Callback routine that is called to destroy this
 * apr_queue_t when its pool is destroyed.
 */
static apr_status_t s_queue_destroy(void* pData)
{
    w_concurrent_queue _queue = pData;

    /* Ignore errors here, we can't do anything about them anyway. */
    apr_thread_cond_destroy(_queue->not_empty);
    apr_thread_cond_destroy(_queue->not_full);
    apr_thread_mutex_destroy(_queue->one_big_mutex);

    return APR_SUCCESS;
}

W_RESULT w_concurrent_queue_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_concurrent_queue* pQueue,
    _In_ uint32_t pQueueCapacity)
{
    if (!pMemPool || !pQueue || w_mem_pool_get_type(pMemPool) != W_MEM_POOL_FAST_EXTEND)
    {
        return APR_BADARG;
    }

    apr_status_t _ret;
    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (!_pool)
    {
        return APR_BADARG;
    }
    w_concurrent_queue _queue = apr_palloc(_pool, sizeof(w_concurrent_queue_t));
    if (!_queue)
    {
        return APR_BADARG;
    }

    *pQueue = _queue;

    _ret = apr_thread_mutex_create(
        &_queue->one_big_mutex,
        APR_THREAD_MUTEX_UNNESTED,
        _pool);
    if (_ret != APR_SUCCESS)
    {
        return _ret;
    }

    _ret = apr_thread_cond_create(&_queue->not_empty, _pool);
    if (_ret != APR_SUCCESS)
    {
        return _ret;
    }

    _ret = apr_thread_cond_create(&_queue->not_full, _pool);
    if (_ret != APR_SUCCESS)
    {
        return _ret;
    }

    /* Set all the data in the queue to NULL */
    _queue->data = apr_pcalloc(_pool, pQueueCapacity * sizeof(void*));
    _queue->bounds = pQueueCapacity;
    _queue->nelts = 0;
    _queue->in = 0;
    _queue->out = 0;
    _queue->terminated = 0;
    _queue->full_waiters = 0;
    _queue->empty_waiters = 0;

    apr_pool_cleanup_register(_pool, _queue, s_queue_destroy, apr_pool_cleanup_null);

    return APR_SUCCESS;
}

W_RESULT w_concurrent_queue_push(_Inout_ w_concurrent_queue pQueue, _In_ void* pData)
{
    if (!pQueue)
    {
        return APR_BADARG;
    }

    apr_status_t _ret;

    if (pQueue->terminated)
    {
        return APR_EOF; /* no more elements ever again */
    }

    _ret = apr_thread_mutex_lock(pQueue->one_big_mutex);
    if (_ret != APR_SUCCESS)
    {
        return _ret;
    }

    if (apr_queue_full(pQueue))
    {
        if (!pQueue->terminated)
        {
            pQueue->full_waiters++;
            _ret = apr_thread_cond_wait(pQueue->not_full, pQueue->one_big_mutex);
            pQueue->full_waiters--;
            if (_ret != APR_SUCCESS)
            {
                apr_thread_mutex_unlock(pQueue->one_big_mutex);
                return _ret;
            }
        }
        // If we wake up and it's still empty, then we were interrupted
        if (apr_queue_full(pQueue))
        {
            Q_DBG("queue full (intr)", pQueue);
            _ret = apr_thread_mutex_unlock(pQueue->one_big_mutex);
            if (_ret != APR_SUCCESS)
            {
                return _ret;
            }
            if (pQueue->terminated)
            {
                return APR_EOF; // no more elements ever again
            }
            else
            {
                return APR_EINTR;
            }
        }
    }

    pQueue->data[pQueue->in] = pData;
    pQueue->in++;
    if (pQueue->in >= pQueue->bounds)
    {
        pQueue->in -= pQueue->bounds;
    }
    pQueue->nelts++;

    if (pQueue->empty_waiters)
    {
        Q_DBG("sig !empty", pQueue);
        _ret = apr_thread_cond_signal(pQueue->not_empty);
        if (_ret != APR_SUCCESS)
        {
            apr_thread_mutex_unlock(pQueue->one_big_mutex);
            return _ret;
        }
    }

    _ret = apr_thread_mutex_unlock(pQueue->one_big_mutex);
    return _ret;
}

W_RESULT w_concurrent_queue_trypush(_Inout_ w_concurrent_queue pQueue, _In_ void* pData)
{
    if (!pQueue)
    {
        return APR_BADARG;
    }

    apr_status_t _ret;
    if (pQueue->terminated)
    {
        return APR_EOF; //no more elements ever again
    }

    _ret = apr_thread_mutex_lock(pQueue->one_big_mutex);
    if (_ret != APR_SUCCESS)
    {
        return _ret;
    }

    if (apr_queue_full(pQueue))
    {
        _ret = apr_thread_mutex_unlock(pQueue->one_big_mutex);
        return APR_EAGAIN;
    }

    pQueue->data[pQueue->in] = pData;
    pQueue->in++;
    if (pQueue->in >= pQueue->bounds)
    {
        pQueue->in -= pQueue->bounds;
    }
    pQueue->nelts++;

    if (pQueue->empty_waiters)
    {
        Q_DBG("sig !empty", queue);
        _ret = apr_thread_cond_signal(pQueue->not_empty);
        if (_ret != APR_SUCCESS)
        {
            apr_thread_mutex_unlock(pQueue->one_big_mutex);
            return _ret;
        }
    }

    _ret = apr_thread_mutex_unlock(pQueue->one_big_mutex);
    return _ret;
}

W_RESULT w_concurrent_queue_pop(_Inout_ w_concurrent_queue pQueue, _In_ void** pData)
{
    if (!pQueue)
    {
        return APR_BADARG;
    }
    apr_status_t _ret;

    if (pQueue->terminated)
    {
        return APR_EOF; /* no more elements ever again */
    }

    _ret = apr_thread_mutex_lock(pQueue->one_big_mutex);
    if (_ret != APR_SUCCESS)
    {
        return _ret;
    }

    /* Keep waiting until we wake up and find that the queue is not empty. */
    if (apr_queue_empty(pQueue))
    {
        if (!pQueue->terminated)
        {
            pQueue->empty_waiters++;
            _ret = apr_thread_cond_wait(pQueue->not_empty, pQueue->one_big_mutex);
            pQueue->empty_waiters--;
            if (_ret != APR_SUCCESS)
            {
                apr_thread_mutex_unlock(pQueue->one_big_mutex);
                return _ret;
            }
        }
        // If we wake up and it's still empty, then we were interrupted
        if (apr_queue_empty(pQueue))
        {
            Q_DBG("queue empty (intr)", pQueue);
            _ret = apr_thread_mutex_unlock(pQueue->one_big_mutex);
            if (_ret != APR_SUCCESS)
            {
                return _ret;
            }
            if (pQueue->terminated)
            {
                return APR_EOF; // no more elements ever again
            }
            else
            {
                return APR_EINTR;
            }
        }
    }

    *pData = pQueue->data[pQueue->out];
    pQueue->nelts--;

    pQueue->out++;
    if (pQueue->out >= pQueue->bounds)
    {
        pQueue->out -= pQueue->bounds;
    }
    if (pQueue->full_waiters)
    {
        Q_DBG("signal !full", pQueue);
        _ret = apr_thread_cond_signal(pQueue->not_full);
        if (_ret != APR_SUCCESS)
        {
            apr_thread_mutex_unlock(pQueue->one_big_mutex);
            return _ret;
        }
    }

    _ret = apr_thread_mutex_unlock(pQueue->one_big_mutex);
    return _ret;
}

W_RESULT w_concurrent_queue_trypop(_Inout_ w_concurrent_queue pQueue, _In_ void** pData)
{
    if (!pQueue)
    {
        return APR_BADARG;
    }

    apr_status_t _ret;

    if (pQueue->terminated)
    {
        return APR_EOF; /* no more elements ever again */
    }

    _ret = apr_thread_mutex_lock(pQueue->one_big_mutex);
    if (_ret != APR_SUCCESS)
    {
        return _ret;
    }

    if (apr_queue_empty(pQueue))
    {
        _ret = apr_thread_mutex_unlock(pQueue->one_big_mutex);
        return APR_EAGAIN;
    }

    *pData = pQueue->data[pQueue->out];
    pQueue->nelts--;

    pQueue->out++;
    if (pQueue->out >= pQueue->bounds)
    {
        pQueue->out -= pQueue->bounds;
    }
    if (pQueue->full_waiters)
    {
        Q_DBG("signal !full", queue);
        _ret = apr_thread_cond_signal(pQueue->not_full);
        if (_ret != APR_SUCCESS)
        {
            apr_thread_mutex_unlock(pQueue->one_big_mutex);
            return _ret;
        }
    }
    _ret = apr_thread_mutex_unlock(pQueue->one_big_mutex);
    return _ret;
}

uint32_t w_concurrent_queue_size(_In_ w_concurrent_queue pQueue)
{
    if (!pQueue)
    {
        return 0;
    }
    return pQueue->nelts;
}

uint32_t w_concurrent_queue_size_threadsafe(_In_ w_concurrent_queue pQueue)
{
    uint32_t _size = 0;
    if (!pQueue)
    {
        return _size;
    }

    if (apr_thread_mutex_lock(pQueue->one_big_mutex) == APR_SUCCESS)
    {
        _size = pQueue->nelts;
        apr_thread_mutex_unlock(pQueue->one_big_mutex);
    }

    return _size;
}

W_RESULT w_concurrent_queue_interrupt_all(_In_ w_concurrent_queue pQueue)
{
    if (!pQueue)
    {
        return APR_BADARG;
    }

    apr_status_t _ret;
    Q_DBG("intr all", queue);
    if ((_ret = apr_thread_mutex_lock(pQueue->one_big_mutex)) != APR_SUCCESS)
    {
        return _ret;
    }
    apr_thread_cond_broadcast(pQueue->not_empty);
    apr_thread_cond_broadcast(pQueue->not_full);

    if ((_ret = apr_thread_mutex_unlock(pQueue->one_big_mutex)) != APR_SUCCESS)
    {
        return _ret;
    }

    return APR_SUCCESS;
}

W_RESULT w_concurrent_queue_fini(_In_ w_concurrent_queue pQueue)
{
    if (!pQueue)
    {
        return APR_BADARG;
    }

    apr_status_t _ret;

    if ((_ret = apr_thread_mutex_lock(pQueue->one_big_mutex)) != APR_SUCCESS)
    {
        return _ret;
    }

    /* we must hold one_big_mutex when setting this... otherwise,
     * we could end up setting it and waking everybody up just after a
     * would-be popper checks it but right before they block
     */
    pQueue->terminated = 1;
    if ((_ret = apr_thread_mutex_unlock(pQueue->one_big_mutex)) != APR_SUCCESS)
    {
        return _ret;
    }
    return w_concurrent_queue_interrupt_all(pQueue);
}
