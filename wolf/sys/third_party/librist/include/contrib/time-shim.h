/* librist. Copyright © 2019 SipRadius LLC. All right reserved.
 * Author: Kuldeep Singh Dhaka <kuldeep@madresistor.com>
 * Author: Sergio Ammirata, Ph.D. <sergio@ammirata.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef __TIME_SHIM_H
#define __TIME_SHIM_H

#include "config.h"

#if defined(_WIN32)
#include <winsock2.h>
#include <time.h>
#define usleep(a)	Sleep((a)/1000)

typedef struct timespec timespec_t;

#if HAVE_CLOCK_GETTIME

#define gettimeofday mingw_gettimeofday

#else

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID 2
#endif

#ifndef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID 3
#endif

typedef int clockid_t;
int gettimeofday(struct timeval *tv, void * not_implemented);
int clock_gettime(clockid_t clock, timespec_t *tp);

#endif /* HAVE_CLOCK_GETTIME */
#elif defined(__APPLE__)
#  define CLOCK_REALTIME_OSX 0
#  define CLOCK_MONOTONIC_OSX 1
#if !HAVE_CLOCK_GETTIME
typedef int clockid_t_osx;
#else
#ifndef CLOCK_REALTIME
#  define CLOCK_REALTIME CALENDAR_CLOCK
# endif
# ifndef CLOCK_MONOTONIC
#  define CLOCK_MONOTONIC SYSTEM_CLOCK
# endif
#endif
#include <mach/mach_time.h>
#include <time.h>
typedef __darwin_time_t time_t;
#ifndef _STRUCT_TIMESPEC
struct timespec {
  time_t tv_sec;
  long   tv_nsec;
};
#endif
typedef struct timespec timespec_t;
#if HAVE_CLOCK_GETTIME
int clock_gettime_osx(clock_id_t clock_id, timespec_t *tp);
#else
int clock_gettime_osx(clockid_t_osx clock_id, timespec_t *tp);
#endif
#else
# include <sys/time.h>
# include <time.h>
typedef struct timespec timespec_t;
#endif

#endif
