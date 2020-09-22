/*
    Project            : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source             : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website            : https://WolfEngine.App
    Name               : w_chrono.h
    Description        : w_chrono represents high resolution timer functions
    Comment            :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "w_timespan.h"

    typedef struct timespec w_timespec;

    /**
     * get current timespec
     * @return a pointer time as timespec
     */
    W_SYSTEM_EXPORT
        w_timespec w_chrono_now(void);

    /**
     * get current timespec
     * @param pClockType could be on of the following:
     * <PRE>
     *      1 = CLOCK_MONOTONIC : represents the absolute elapsed wall-clock time since some arbitrary, fixed point in the past. It isn't affected by changes in the system time-of-day clock or NTP.
     *
     *      CLOCK_REALTIME : represents the machine's best-guess as to the current wall-clock, time-of-day time. it can jump forwards and backwards as the system time-of-day clock is changed, including by NTP.
     *
     *      CLOCK_PROCESS_CPUTIME_ID : used for measuring the amount of CPU time consumed by the thread.
     * </PRE>
     * @return a pointer to w_timespan object
     */
    W_SYSTEM_EXPORT
        w_timespec w_chrono_clock_now(_In_ int pClockType);

    /**
     * get current time in seconds
     * @return seconds
     */
    W_SYSTEM_EXPORT
        double w_chrono_now_in_sec(void);

    /**
     * convert timspec to seconds
     * @return seconds
    */
    W_SYSTEM_EXPORT
        double w_chrono_timespec_to_sec(_In_ const w_timespec* pT);

    /**
     * get duration between two times
     * @param pT1 is start time
     * @param pT2 is second time
     * @return a pointer to w_timespan object
     */
    W_SYSTEM_EXPORT
        w_timespec w_chrono_duration(
            _In_ const w_timespec* pT1,
            _In_ const w_timespec* pT2);

    /**
     * get duration in total nanoseconds
     * @param pT1 is start time
     * @param pT2 is second time
     * @return total nanoseconds
     */
    W_SYSTEM_EXPORT
        double w_chrono_duration_nanoseconds(
            _In_ const w_timespec* pT1,
            _In_ const w_timespec* pT2);

    /**
     * get duration in total microseconds
     * @param pT1 is start time
     * @param pT2 is second time
     * @return total microseconds
     */
    W_SYSTEM_EXPORT
        double w_chrono_duration_microseconds(
            _In_ const w_timespec* pT1,
            _In_ const w_timespec* pT2);

    /**
     * get duration in total milliseconds
     * @param pT1 is start time
     * @param pT2 is second time
     * @return total milliseconds
     */
    W_SYSTEM_EXPORT
        double w_chrono_duration_milliseconds(
            _In_ const w_timespec* pT1,
            _In_ const w_timespec* pT2);

    /**
     * get duration in total seconds
     * @param pT1 is start time
     * @param pT2 is second time
     * @return total seconds
     */
    W_SYSTEM_EXPORT
        double w_chrono_duration_seconds(
            _In_ const w_timespec* pT1,
            _In_ const w_timespec* pT2);

#ifdef __cplusplus
}
#endif
