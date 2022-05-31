/* librist. Copyright © 2019 SipRadius LLC. All right reserved.
 * Author: Kuldeep Singh Dhaka <kuldeep@madresistor.com>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef __PTHREAD_SHIM_H
#define __PTHREAD_SHIM_H

#include "common/attributes.h"
#include "config.h"

#ifdef _WIN32
# include <stdbool.h>
# include <stdint.h>
#include <winsock2.h> //<-- not used here, but included because otherwise our order is broken.
#define _WINSOCKAPI_
# include <windows.h>
#if HAVE_PTHREADS
#include <pthread.h>
# define PTHREAD_START_FUNC(fname,aname) void *fname(void *aname)
typedef void *(*pthread_start_func_t)(void *aname);
RIST_PRIV int pthread_cond_timedwait_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, uint32_t ms);
#else
typedef CRITICAL_SECTION pthread_mutex_t;
typedef void pthread_mutexattr_t;
typedef void pthread_condattr_t;
typedef void pthread_rwlockattr_t;
typedef HANDLE pthread_t;
typedef int pthread_attr_t;
typedef CONDITION_VARIABLE pthread_cond_t;
typedef LPVOID thread_arg_t;
typedef DWORD thread_return_t;

typedef struct {
	SRWLOCK  lock;
	bool    exclusive;
} pthread_rwlock_t;

# define PTHREAD_START_FUNC(fname,aname) DWORD __stdcall fname(LPVOID aname)
typedef DWORD (__stdcall *pthread_start_func_t)(LPVOID aname);
RIST_PRIV int init_mutex_once(pthread_mutex_t *mutex, PINIT_ONCE once_var);

RIST_PRIV int pthread_create(pthread_t *thread, pthread_attr_t *attr, DWORD (__stdcall *start_routine)(LPVOID), void *arg);
RIST_PRIV int pthread_join(pthread_t thread, void **value_ptr);
RIST_PRIV int pthread_detach(pthread_t);

RIST_PRIV int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
RIST_PRIV int pthread_mutex_destroy(pthread_mutex_t *mutex);
RIST_PRIV int pthread_mutex_lock(pthread_mutex_t *mutex);
RIST_PRIV int pthread_mutex_unlock(pthread_mutex_t *mutex);

RIST_PRIV int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);
RIST_PRIV int pthread_cond_destroy(pthread_cond_t *cond);
RIST_PRIV int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
RIST_PRIV int pthread_cond_timedwait_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, uint32_t reltime_ms);

RIST_PRIV int pthread_cond_signal(pthread_cond_t *cond);
RIST_PRIV int pthread_cond_broadcast(pthread_cond_t *cond);

RIST_PRIV int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
RIST_PRIV int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
RIST_PRIV int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
RIST_PRIV int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
RIST_PRIV int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
RIST_PRIV int pthread_rwlock_trywrlock(pthread_rwlock_t  *rwlock);
RIST_PRIV int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

/* Semaphores */
typedef HANDLE sem_t;
RIST_PRIV int sem_init(sem_t *sem, int pshared, unsigned value);
RIST_PRIV int sem_post(sem_t *sem);
RIST_PRIV int sem_wait(sem_t *sem);
#endif
#else
# include <pthread.h>
# include <sys/types.h>
# include <semaphore.h>
# include <stdint.h>
# define PTHREAD_START_FUNC(fname,aname) void *fname(void *aname)
typedef void *(*pthread_start_func_t)(void *aname);
RIST_PRIV int pthread_cond_timedwait_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, uint32_t ms);
#endif

#endif /* __PTHREAD_SHIM_H */
