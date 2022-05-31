/*
 * Copyright © 2020, VideoLAN and librist authors
 * Copyright © 2019-2020 SipRadius LLC
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RIST_COMMON_ATTRIBUTES_H
#define RIST_COMMON_ATTRIBUTES_H

/* Reference: http://gcc.gnu.org/wiki/Visibility */
#if defined(_WIN32) || defined(__CYGWIN__)
# define RIST_PRIV
#else /* defined(_WIN32) || defined(__CYGWIN__) */
# if __GNUC__ >= 4
#  define RIST_PRIV  __attribute__ ((visibility ("hidden")))
# else /* __GNUC__ >= 4 */
#  define RIST_PRIV
# endif /* __GNUC__ >= 4 */
#endif /* defined(_WIN32) || defined(__CYGWIN__) */

#ifdef _WIN32
#if defined(__GNUC__)
# define RIST_PACKED_STRUCT(sname,sbody) \
struct __attribute__((packed)) sname sbody;
#else
# define RIST_PACKED_STRUCT(sname,sbody) \
__pragma( pack(push, 1) ) \
struct sname sbody; \
__pragma( pack(pop) )
#endif
#else
# define RIST_PACKED_STRUCT(sname,sbody) \
struct __attribute__((packed)) sname sbody;
#endif

/* Branch prediction */
#ifdef __GNUC__
# define RIST_LIKELY(p)   __builtin_expect(!!(p), 1)
# define RIST_UNLIKELY(p) __builtin_expect(!!(p), 0)
#else
# define RIST_LIKELY(p)   (!!(p))
# define RIST_UNLIKELY(p) (!!(p))
#endif

#define RIST_MARK_UNUSED(unused_param) ((void)(unused_param))

#if defined(__GNUC__) && __GNUC__ >= 7
#define RIST_FALLTHROUGH ; __attribute__ ((fallthrough))
#else
#define RIST_FALLTHROUGH ((void)0)
#endif /* __GNUC__ >= 7 */

#ifdef _WIN32
#ifdef _WIN64
#ifdef __GNUC__
#ifndef __MINGW32__
typedef long long ssize_t;
enum
{
    POLLERR=0x1,
    POLLHUP=0x2,
    POLLNVAL=0x4,
    POLLWRNORM=0x10,
    POLLWRBAND=0x20,
    POLLRDNORM=0x100,
    POLLRDBAND=0x200,
    POLLPRI=0x400,
};
#define POLLIN  (POLLRDNORM|POLLRDBAND)
#define POLLOUT (POLLWRNORM|POLLWRBAND)
struct pollfd
{
    int fd;
    unsigned events;
    unsigned revents;
};
struct pollfd;
#endif
#else
#include <stdint.h>
typedef int64_t ssize_t;
#endif
#else
typedef signed int ssize_t;
#endif
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#endif
