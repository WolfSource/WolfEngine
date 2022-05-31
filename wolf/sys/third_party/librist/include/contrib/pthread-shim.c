/* librist. Copyright © 2019 SipRadius LLC. All right reserved.
 * Author: Kuldeep Singh Dhaka <kuldeep@madresistor.com>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "time-shim.h"
#include "pthread-shim.h"
#include <stdio.h>
#ifdef _WIN32
#if HAVE_PTHREADS
int pthread_cond_timedwait_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, uint32_t ms)
{
	timespec_t ts;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint64_t odd = (tv.tv_usec + (ms * 1000)) * 1000;
	ts.tv_sec = tv.tv_sec + odd / 1000000000ULL;
	ts.tv_nsec = odd % 1000000000ULL;
	return pthread_cond_timedwait(cond, mutex, (const struct timespec *)&ts);
}
#else
#include <errno.h>

static BOOL WINAPI init_mutex_impl(PINIT_ONCE once_var, PVOID mutex, PVOID *param2) {
  RIST_MARK_UNUSED(once_var);
  RIST_MARK_UNUSED(param2);
  if (pthread_mutex_init((pthread_mutex_t *)mutex, NULL) !=
      0) {
    return FALSE;
  }
  return TRUE;
}

int init_mutex_once(pthread_mutex_t *mutex, PINIT_ONCE once_var)
{
  if (!InitOnceExecuteOnce(once_var, init_mutex_impl, mutex, NULL)) {
    return -1;
  }
  return 0;
}

int pthread_create(pthread_t *thread, pthread_attr_t *attr, DWORD (__stdcall *start_routine)(LPVOID), void *arg)
{
	(void)attr;
	if (thread == NULL || start_routine == NULL) {
		return 1;
	}

	*thread = CreateThread(NULL, 0, start_routine, arg, 0, NULL);
	if (*thread == NULL) {
		return 1;
	}

	return 0;
}

int pthread_join(pthread_t thread, void **value_ptr)
{
	(void)value_ptr;

	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
	return 0;
}

void pthread_exit(void *retval)
{
	ExitThread(retval ? (*((DWORD*)retval)) : 0);
}

int pthread_cancel(pthread_t thread)
{
	return TerminateThread(thread, 0) ? 0 : -1;
}

int pthread_detach(pthread_t thread)
{
	return CloseHandle(thread) ? 0 : -1;
}

int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr)
{
	(void)attr;

	if (mutex == NULL) {
		return 1;
	}

	InitializeCriticalSection(mutex);
	return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	if (mutex == NULL) {
		return 1;
	}

	DeleteCriticalSection(mutex);
	return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	if (mutex == NULL) {
		return 1;
	}

	EnterCriticalSection(mutex);
	return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	if (mutex == NULL) {
		return 1;
	}

	LeaveCriticalSection(mutex);
	return 0;
}

int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr)
{
	(void)attr;

	if (cond == NULL) {
		return 1;
	}

	InitializeConditionVariable(cond);
	return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
	/* Windows does not have a destroy for conditionals */
	(void)cond;
	return 0;
}

int pthread_cond_timedwait_broken(pthread_cond_t *cond, pthread_mutex_t *mutex,
	const timespec_t *abstime)
{
	DWORD ms = INFINITE;
	int64_t duration = 0;
	if (cond == NULL || mutex == NULL) {
		return 1;
	}

	if (abstime != NULL) {
		duration = ((abstime->tv_sec - time(NULL)) * 1000) + (abstime->tv_nsec / 1000000);
		if (RIST_UNLIKELY(duration < 0)) {
			duration = 1;
		}
		ms = (DWORD)duration;
	}

	if (!SleepConditionVariableCS(cond, mutex, ms)) {
		return ETIMEDOUT;
	}

	return 0;
}

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex)
{
	if (cond == NULL || mutex == NULL) {
		return 1;
	}

	return pthread_cond_timedwait_broken(cond, mutex, NULL);
}

int pthread_cond_timedwait_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, uint32_t reltime_ms)
{
	if (cond == NULL || mutex == NULL) {
		return 1;
	}

	if (!SleepConditionVariableCS(cond, mutex, reltime_ms)) {
		return ETIMEDOUT;
	}

	return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
	if (cond == NULL) {
		return 1;
	}

	WakeConditionVariable(cond);
	return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
	if (cond == NULL) {
		return 1;
	}

	WakeAllConditionVariable(cond);
	return 0;
}

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr)
{
	(void)attr;

	if (rwlock == NULL) {
		return 1;
	}

	InitializeSRWLock(&(rwlock->lock));
	rwlock->exclusive = false;
	return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
	(void)rwlock;
	return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
	if (rwlock == NULL) {
		return 1;
	}

	AcquireSRWLockShared(&(rwlock->lock));
	return 0;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
	if (rwlock == NULL) {
		return 1;
	}

	return !TryAcquireSRWLockShared(&(rwlock->lock));
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
	if (rwlock == NULL) {
		return 1;
	}

	AcquireSRWLockExclusive(&(rwlock->lock));
	rwlock->exclusive = true;
	return 0;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t  *rwlock)
{
	BOOLEAN ret;

	if (rwlock == NULL) {
		return 1;
	}

	ret = TryAcquireSRWLockExclusive(&(rwlock->lock));
	if (ret) {
		rwlock->exclusive = true;
	}

	return ret;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
	if (rwlock == NULL) {
		return 1;
	}

	if (rwlock->exclusive) {
		rwlock->exclusive = false;
		ReleaseSRWLockExclusive(&(rwlock->lock));
	} else {
		ReleaseSRWLockShared(&(rwlock->lock));
	}

	return 0;
}

/* Semaphores */
#define MAX_SEM_COUNT 32

int sem_init(sem_t *sem, int pshared, unsigned value)
{
	(void)pshared;
	*sem = CreateSemaphore(
		NULL,           // default security attributes
		value,          // initial count
		MAX_SEM_COUNT,  // maximum count
		NULL);          // unnamed semaphore

	if (*sem != NULL) {
		return 0;
	} else {
		errno = ENOSPC;
		return -1;
	}
}

int sem_wait(sem_t *sem)
{
	DWORD wait_result = WaitForSingleObject(*sem, INFINITE);
	switch (wait_result) {
	// The semaphore object was signaled.
	case WAIT_ABANDONED:
	case WAIT_OBJECT_0:
		return 0;
	break;
	case WAIT_FAILED:
		errno = EINVAL;
		return -1;
	break;
	default:
	break;
	}
	return -1;
}

int sem_post(sem_t *sem)
{
	if (!ReleaseSemaphore(*sem, 1, NULL)) {
		errno = EINVAL;
		return -1;
	} else {
		return 0;
	}
}
#endif
#else

#ifdef __APPLE__
#include <sys/time.h>
#include "time-shim.h"
#endif

// This is not part of the POSIX API
// Convience function around pthread
int pthread_cond_timedwait_ms(pthread_cond_t *cond, pthread_mutex_t *mutex, uint32_t ms)
{
	timespec_t ts;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint64_t odd =  (tv.tv_usec + (ms * 1000)) * 1000;
	ts.tv_sec = tv.tv_sec + odd / 1000000000ULL;
	ts.tv_nsec = odd % 1000000000ULL;
	return pthread_cond_timedwait(cond, mutex, (const struct timespec*)&ts);
}

#endif
