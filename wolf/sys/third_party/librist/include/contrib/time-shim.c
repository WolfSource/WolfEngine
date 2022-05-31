/* librist. Copyright © 2019 SipRadius LLC. All right reserved.
 * Author: Antonio Cardce <anto.cardace@gmail.com>
 * Author: Kuldeep Singh Dhaka <kuldeep@madresistor.com>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 * Author: Marvin Scholz <epirat07 at gmail dot com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "time-shim.h"

#if defined(_WIN32) && !HAVE_CLOCK_GETTIME
#define _WINSOCKAPI_
# include <windows.h>
# include <errno.h>
# include <stdlib.h>
# include <limits.h>
# include <stdint.h>
#include "common/attributes.h"


//(un)graciously taken & modified from mingw-w64/misc/gettimeofday.c
/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */


#define FILETIME_1970 116444736000000000ull /* seconds between 1/1/1601 and 1/1/1970 */
#define HECTONANOSEC_PER_SEC 10000000ull

int gettimeofday(struct timeval *tv, void * not_implemented)
{
	RIST_MARK_UNUSED(not_implemented);

	union {
		unsigned long long ns100; /*time since 1 Jan 1601 in 100ns units */
		FILETIME ft;
	}  _now;

	if (tv != NULL)
	{
		struct timespec tp;
		GetSystemTimeAsFileTime (&_now.ft);  /* 100-nanoseconds since 1-1-1601 */
		/* The actual accuracy on XP seems to be 125,000 nanoseconds = 125 microseconds = 0.125 milliseconds */
		_now.ns100 -= FILETIME_1970;        /* 100 nano-seconds since 1-1-1970 */
		tp.tv_sec = _now.ns100 / HECTONANOSEC_PER_SEC;     /* seconds since 1-1-1970 */
		tp.tv_nsec = (long) (_now.ns100 % HECTONANOSEC_PER_SEC) * 100; /* nanoseconds */
		tv->tv_sec = tp.tv_sec;
		tv->tv_usec = tp.tv_nsec/1000;
	}
	return 0;
}


//(un)graciously copied from mingw-w64/winpthreads/src/clock.c
/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the w64 mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */


#define POW10_7                 10000000
#define POW10_9                 1000000000


/* Number of 100ns-seconds between the beginning of the Windows epoch
 * (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970)
 */
#define DELTA_EPOCH_IN_100NS    INT64_C(116444736000000000)

static inline int lc_set_errno(int result)
{
    if (result != 0) {
        errno = result;
        return -1;
    }
    return 0;
}

int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    uint64_t t;
    LARGE_INTEGER pf, pc;
    union {
        uint64_t u64;
        FILETIME ft;
    }  ct, et, kt, ut;

    switch(clock_id) {
    case CLOCK_REALTIME:
        {
            GetSystemTimeAsFileTime(&ct.ft);
            t = ct.u64 - DELTA_EPOCH_IN_100NS;
            tp->tv_sec = t / POW10_7;
            tp->tv_nsec = ((int) (t % POW10_7)) * 100;

            return 0;
        }

    case CLOCK_MONOTONIC:
        {
            if (QueryPerformanceFrequency(&pf) == 0)
                return lc_set_errno(EINVAL);

            if (QueryPerformanceCounter(&pc) == 0)
                return lc_set_errno(EINVAL);

            tp->tv_sec = pc.QuadPart / pf.QuadPart;
            tp->tv_nsec = (int) (((pc.QuadPart % pf.QuadPart) * POW10_9 + (pf.QuadPart >> 1)) / pf.QuadPart);
            if (tp->tv_nsec >= POW10_9) {
                tp->tv_sec ++;
                tp->tv_nsec -= POW10_9;
            }

            return 0;
        }

    case CLOCK_PROCESS_CPUTIME_ID:
        {
        if(0 == GetProcessTimes(GetCurrentProcess(), &ct.ft, &et.ft, &kt.ft, &ut.ft))
            return lc_set_errno(EINVAL);
        t = kt.u64 + ut.u64;
        tp->tv_sec = t / POW10_7;
        tp->tv_nsec = ((int) (t % POW10_7)) * 100;

        return 0;
        }

    case CLOCK_THREAD_CPUTIME_ID: 
        {
            if(0 == GetThreadTimes(GetCurrentThread(), &ct.ft, &et.ft, &kt.ft, &ut.ft))
                return lc_set_errno(EINVAL);
            t = kt.u64 + ut.u64;
            tp->tv_sec = t / POW10_7;
            tp->tv_nsec = ((int) (t % POW10_7)) * 100;

            return 0;
        }

    default:
        break;
    }

    return lc_set_errno(EINVAL);
}

#endif

#ifdef __APPLE__
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/sysctl.h>

/**
 * Get the absolute time at which the system was booted
 * 
 * This time is changed whenever the clock is adjusted to
 * correctly reflect the boot time with the adjusted clock,
 * so just querying it once and reusing the value is not safe.
 *
 * \param[out]  tv   Timeval struct to write the boottime to
 * 
 * \note
 * The boot time only has microsecond precision
 *
 * \return 0 on success, else -1 and errno set
 */
static int rist__get_system_boottime(struct timeval *tv)
{
    int ret = sysctl((int[]){ CTL_KERN, KERN_BOOTTIME }, 2,
                     tv, &(size_t){ sizeof(*tv) }, NULL, 0);

    if (ret != 0)
        return errno;

    return 0;
}

/**
 * Get the monotonic time (CLOCK_MONOTONIC)
 * 
 * Calculates a monotically incrasing time since system boot
 * that continues to increment when the system is asleep.
 * 
 * Warnings to everyone trying to simplify this:
 * - Using mach_continuous_time is not equivalent to this, see
 *   the darwin manpage about CLOCK_MONOTONIC_RAW for an explanation.
 * - Using mach_absolute_time is not equivalent to this, as it does
 *   not continue to increment when the system is asleep.
 * - The boot time is queried twice in a loop and only used if both
 *   match. This is done to detect if the boot time change since we
 *   obtained the current time of day, as the boot time can change
 *   when the system wallclock is adjusted, as that will adjust the
 *   boot time accordingly.
 *
 * \param[out]  tv   Timeval struct to write the monotonic time to
 *
 * \return 0 on success, else -1 and errno set
 */
static int rist__get_monotonic(struct timeval *tv)
{
    int ret;
    struct timeval currenttime;
    struct timeval boottime_begin;
    struct timeval boottime_end;

    do {
        ret = rist__get_system_boottime(&boottime_begin);
        if (ret != 0)
            return ret;

        ret = gettimeofday(&currenttime, NULL);
        if (ret != 0)
            return ret;

        ret = rist__get_system_boottime(&boottime_end);
        if (ret != 0)
            return ret;
    } while (timercmp(&boottime_begin, &boottime_end, !=));

    timersub(&currenttime, &boottime_begin, tv);
    return 0;
}

#if HAVE_CLOCK_GETTIME
int clock_gettime_osx(clock_id_t clock_id, timespec_t *tp)
#else
int clock_gettime_osx(clockid_t_osx clock_id, timespec_t *tp)
#endif
{
    int ret = 0;
    struct timeval tv;

    switch (clock_id) {
        case CLOCK_MONOTONIC_OSX:
            ret = rist__get_monotonic(&tv);
            break;
        case CLOCK_REALTIME_OSX:
            ret = gettimeofday(&tv, NULL);
            break;
        default:
            errno = EINVAL;
            return -1;
    }

    if (ret == 0)
        TIMEVAL_TO_TIMESPEC(&tv, tp);
    return ret;
}
#endif
