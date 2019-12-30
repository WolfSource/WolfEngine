#include "w_thread_pool.h"

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

//FIFO queued job
typedef struct job job_t;
struct job
{
    job_t*          job_next;                   // linked list of jobs
    void*           (*job_func)(void*);         // function to call
    void*           job_arg;                    // its argument
};

//List of active worker threads, linked through their stacks.
typedef struct active active_t;
struct active
{
    active_t*           active_next;            // linked list of threads
    pthread_t           active_tid;             // active thread id
};

//The thread pool, opaque to the clients.
struct w_thread_pool
{
    struct w_thread_pool*       pool_forw;              // circular linked list
    struct w_thread_pool*       pool_back;              // of all thread pools
    pthread_mutex_t             pool_mutex;             // protects the pool data
    pthread_cond_t              pool_busycv;            // synchronization in pool_queue
    pthread_cond_t              pool_workcv;            // synchronization with workers
    pthread_cond_t              pool_waitcv;            // synchronization in pool_wait()
    active_t*                   pool_active;            // list of threads performing work
    job_t*                      pool_head;              // head of FIFO job queue
    job_t*                      pool_tail;              // tail of FIFO job queue
    pthread_attr_t              pool_attr;              // attributes of the workers
    int                         pool_flags;             // see below
    uint32_t                    pool_linger;            // seconds before idle workers exit
    int                         pool_minimum;           // minimum number of worker threads
    int                         pool_maximum;           // maximum number of worker threads
    int                         pool_nthreads;          // current number of worker threads
    int                         pool_idle;              // number of idle workers
};

// pool_flags
#define    POOL_WAIT        0x01                // waiting in w_thread_pool_wait_all()
#define    POOL_DESTROY     0x02                // pool is being destroyed

// the list of all created and not yet destroyed thread pools
static struct w_thread_pool* thr_pools = NULL;

// protects thr_pools
static pthread_mutex_t thr_pool_lock = PTHREAD_MUTEX_INITIALIZER;

// set of all signals
static sigset_t fillset;

static void* worker_thread(void*);

static int create_worker(struct w_thread_pool* pool)
{
    sigset_t oset;
    int error;

    pthread_sigmask(SIG_SETMASK, &fillset, &oset);
    error = pthread_create(NULL, &pool->pool_attr, worker_thread, pool);
    pthread_sigmask(SIG_SETMASK, &oset, NULL);
    return (error);
}

static void worker_cleanup(struct w_thread_pool* pool)
{
    --pool->pool_nthreads;
    if (pool->pool_flags & POOL_DESTROY)
    {
        if (pool->pool_nthreads == 0)
        {
            pthread_cond_broadcast(&pool->pool_busycv);
        }
    }
    else if (pool->pool_head != NULL &&
             pool->pool_nthreads < pool->pool_maximum &&
             create_worker(pool) == 0)
    {
        pool->pool_nthreads++;
    }
    pthread_mutex_unlock(&pool->pool_mutex);
}

static void notify_waiters(struct w_thread_pool *pool)
{
    if (pool->pool_head == NULL && pool->pool_active == NULL)
    {
        pool->pool_flags &= ~POOL_WAIT;
        pthread_cond_broadcast(&pool->pool_waitcv);
    }
}

static void job_cleanup(struct w_thread_pool* pool)
{
    pthread_t my_tid = pthread_self();
    active_t* activep;
    active_t** activepp;

    (void) pthread_mutex_lock(&pool->pool_mutex);
    for (activepp = &pool->pool_active; (activep = *activepp) != NULL; activepp = &activep->active_next)
    {
        if (activep->active_tid == my_tid)
        {
            *activepp = activep->active_next;
            break;
        }
    }
    if (pool->pool_flags & POOL_WAIT)
    {
        notify_waiters(pool);
    }
}

static void* worker_thread(void* arg)
{
    struct w_thread_pool *pool = (struct w_thread_pool*)arg;
    int timedout;
    job_t *job;
    void *(*func)(void *);
    active_t active;
    struct timespec ts;

    pthread_mutex_lock(&pool->pool_mutex);
    pthread_cleanup_push(worker_cleanup, pool);
    active.active_tid = pthread_self();
    for (;;)
    {
        /*
         * We don't know what this thread was doing during
         * its last job, so we reset its signal mask and
         * cancellation state back to the initial values.
         */
        pthread_sigmask(SIG_SETMASK, &fillset, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

        timedout = 0;
        pool->pool_idle++;
        if (pool->pool_flags & POOL_WAIT)
        {
            notify_waiters(pool);
        }
        while (pool->pool_head == NULL && !(pool->pool_flags & POOL_DESTROY))
        {
            if (pool->pool_nthreads <= pool->pool_minimum)
            {
                pthread_cond_wait(&pool->pool_workcv, &pool->pool_mutex);
            }
            else
            {
                (void) clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += pool->pool_linger;
                if (pool->pool_linger == 0 ||
                    pthread_cond_timedwait(&pool->pool_workcv,
                    &pool->pool_mutex, &ts) == ETIMEDOUT)
                {
                    timedout = 1;
                    break;
                }
            }
        }
        pool->pool_idle--;
        if (pool->pool_flags & POOL_DESTROY)
        {
            break;
        }
        if ((job = pool->pool_head) != NULL)
        {
            timedout = 0;
            func = job->job_func;
            arg = job->job_arg;
            pool->pool_head = job->job_next;
            if (job == pool->pool_tail)
            {
                pool->pool_tail = NULL;
            }
            active.active_next = pool->pool_active;
            pool->pool_active = &active;
            (void) pthread_mutex_unlock(&pool->pool_mutex);
            pthread_cleanup_push(job_cleanup, pool);
            free(job);
            //Call the specified job function.
            func(arg);
            pthread_cleanup_pop(1);
        }
        if (timedout && pool->pool_nthreads > pool->pool_minimum)
        {
            /*
             * We timed out and there is no work to be done
             * and the number of workers exceeds the minimum.
             * Exit now to reduce the size of the pool.
             */
            break;
        }
    }
    pthread_cleanup_pop(1);    /* worker_cleanup(pool) */
    return (NULL);
}

static void clone_attributes(pthread_attr_t *new_attr, pthread_attr_t *old_attr)
{
    struct sched_param param;
    void *addr;
    size_t size;
    int value;

    (void) pthread_attr_init(new_attr);

    if (old_attr != NULL)
    {
        pthread_attr_getstack(old_attr, &addr, &size);
        //don't allow a non-NULL thread stack address
        pthread_attr_setstack(new_attr, NULL, size);

        pthread_attr_getscope(old_attr, &value);
        pthread_attr_setscope(new_attr, value);

        pthread_attr_getinheritsched(old_attr, &value);
        pthread_attr_setinheritsched(new_attr, value);

        pthread_attr_getschedpolicy(old_attr, &value);
        pthread_attr_setschedpolicy(new_attr, value);

        pthread_attr_getschedparam(old_attr, &param);
        pthread_attr_setschedparam(new_attr, &param);

        pthread_attr_getguardsize(old_attr, &size);
        pthread_attr_setguardsize(new_attr, size);
    }

    //make all pool threads be detached threads
    pthread_attr_setdetachstate(new_attr, PTHREAD_CREATE_DETACHED);
}

struct w_thread_pool* w_thread_pool_create(_In_ const uint32_t min_threads,
                                           _In_ const uint32_t max_threads,
                                           _In_ const uint32_t linger,
                                           _In_ const pthread_attr_t *attr)
{
    struct w_thread_pool* pool;

    (void) sigfillset(&fillset);

    if (min_threads > max_threads || max_threads < 1) {
        errno = EINVAL;
        return (NULL);
    }

    if ((pool = malloc(sizeof(*pool))) == NULL)
    {
        errno = ENOMEM;
        return (NULL);
    }
    pthread_mutex_init(&pool->pool_mutex, NULL);
    pthread_cond_init(&pool->pool_busycv, NULL);
    pthread_cond_init(&pool->pool_workcv, NULL);
    pthread_cond_init(&pool->pool_waitcv, NULL);
    pool->pool_active = NULL;
    pool->pool_head = NULL;
    pool->pool_tail = NULL;
    pool->pool_flags = 0;
    pool->pool_linger = linger;
    pool->pool_minimum = min_threads;
    pool->pool_maximum = max_threads;
    pool->pool_nthreads = 0;
    pool->pool_idle = 0;

    /*
     * We cannot just copy the attribute pointer.
     * We need to initialize a new pthread_attr_t structure using
     * the values from the caller-supplied attribute structure.
     * If the attribute pointer is NULL, we need to initialize
     * the new pthread_attr_t structure with default values.
     */
    clone_attributes(&pool->pool_attr, attr);

    /* insert into the global list of all thread pools */
    (void) pthread_mutex_lock(&thr_pool_lock);
    if (thr_pools == NULL)
    {
        pool->pool_forw = pool;
        pool->pool_back = pool;
        thr_pools = pool;
    }
    else
    {
        thr_pools->pool_back->pool_forw = pool;
        pool->pool_forw = thr_pools;
        pool->pool_back = thr_pools->pool_back;
        thr_pools->pool_back = pool;
    }
    pthread_mutex_unlock(&thr_pool_lock);
    return (pool);
}

int w_thread_pool_add_job(struct w_thread_pool* pool, void*(*func)(void*), void* arg)
{
    job_t* job;

    if ((job = malloc(sizeof (*job))) == NULL)
    {
        errno = ENOMEM;
        return (-1);
    }
    job->job_next = NULL;
    job->job_func = func;
    job->job_arg = arg;

    (void) pthread_mutex_lock(&pool->pool_mutex);

    if (pool->pool_head == NULL)
    {
        pool->pool_head = job;
    }
    else
    {
        pool->pool_tail->job_next = job;
    }
    pool->pool_tail = job;

    if (pool->pool_idle > 0)
    {
        pthread_cond_signal(&pool->pool_workcv);
    }
    else if (pool->pool_nthreads < pool->pool_maximum && create_worker(pool) == 0)
    {
        pool->pool_nthreads++;
    }
    pthread_mutex_unlock(&pool->pool_mutex);
    return 0;
}

void w_thread_pool_wait_all(struct w_thread_pool* pool)
{
    pthread_mutex_lock(&pool->pool_mutex);
    pthread_cleanup_push(pthread_mutex_unlock, &pool->pool_mutex);
    while (pool->pool_head != NULL || pool->pool_active != NULL)
    {
        pool->pool_flags |= POOL_WAIT;
        pthread_cond_wait(&pool->pool_waitcv, &pool->pool_mutex);
    }
    pthread_cleanup_pop(1);
}

void w_thread_pool_destroy(struct w_thread_pool* pool)
{
    active_t *activep;
    job_t *job;

    pthread_mutex_lock(&pool->pool_mutex);
    pthread_cleanup_push(pthread_mutex_unlock, &pool->pool_mutex);

    // mark the pool as being destroyed; wakeup idle workers
    pool->pool_flags |= POOL_DESTROY;
    pthread_cond_broadcast(&pool->pool_workcv);

    // cancel all active workers
    for (activep = pool->pool_active;
        activep != NULL;
        activep = activep->active_next)
    {
        pthread_cancel(activep->active_tid);
    }
    // wait for all active workers to finish
    while (pool->pool_active != NULL)
    {
        pool->pool_flags |= POOL_WAIT;
        pthread_cond_wait(&pool->pool_waitcv, &pool->pool_mutex);
    }

    // the last worker to terminate will wake us up
    while (pool->pool_nthreads != 0)
    {
        pthread_cond_wait(&pool->pool_busycv, &pool->pool_mutex);
    }
    pthread_cleanup_pop(1);

    //Unlink the pool from the global list of all pools.
    pthread_mutex_lock(&thr_pool_lock);
    if (thr_pools == pool)
    {
        thr_pools = pool->pool_forw;
    }
    if (thr_pools == pool)
    {
        thr_pools = NULL;
    }
    else
    {
        pool->pool_back->pool_forw = pool->pool_forw;
        pool->pool_forw->pool_back = pool->pool_back;
    }
    pthread_mutex_unlock(&thr_pool_lock);

    //There should be no pending jobs, but just in case...
    for (job = pool->pool_head; job != NULL; job = pool->pool_head)
    {
        pool->pool_head = job->job_next;
        free(job);
    }
    pthread_attr_destroy(&pool->pool_attr);
    free(pool);
}
