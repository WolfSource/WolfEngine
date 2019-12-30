#include "w_chrono.h"

struct timespec chrono_now(void)
{
    struct timespec _time;
    clock_gettime(CLOCK_MONOTONIC, &_time);
    return _time;
}

struct timespec chrono_clock_now(_In_ const int pClockType)
{
    struct timespec _time;
    clock_gettime(pClockType, &_time);
    return _time;
}

struct timespec chrono_duration(_In_ const struct timespec* pT1,
                                _In_ const struct timespec* pT2)
{
    struct timespec _diff;
    if (!pT1 || !pT2)
    {
        _diff.tv_sec = -1;
        _diff.tv_nsec = -1;
    }
    else
    {
        if ( pT2->tv_nsec - pT1->tv_nsec < 0)
        {
            _diff.tv_sec = pT2->tv_sec - pT1->tv_sec - 1;
            _diff.tv_nsec = 1e+9 + pT2->tv_nsec - pT1->tv_nsec;
        }
        else
        {
            _diff.tv_sec = pT2->tv_sec - pT1->tv_sec;
            _diff.tv_nsec = pT2->tv_nsec - pT1->tv_nsec;
        }
    }
    return _diff;
}

double chrono_duration_nanoseconds(_In_ const struct timespec* pT1,
                                   _In_ const struct timespec* pT2)
{
    struct timespec _diff = chrono_duration(pT1, pT2);
    return (double)(_diff.tv_sec * 1e+9 + _diff.tv_nsec);
}

double chrono_duration_microseconds(_In_ const struct timespec* pT1,
                                    _In_ const struct timespec* pT2)
{
    struct timespec _diff = chrono_duration(pT1, pT2);
    return (double)_diff.tv_sec * 1e+6 + (double)(_diff.tv_nsec * 1e-3);
}

double chrono_duration_milliseconds(_In_ const struct timespec* pT1,
                                    _In_ const struct timespec* pT2)
{
    struct timespec _diff = chrono_duration(pT1, pT2);
    return (double)_diff.tv_sec * 1e+3 + (double)(_diff.tv_nsec * 1e-6);
}

double chrono_duration_seconds(_In_ const struct timespec* pT1,
                               _In_ const struct timespec* pT2)
{
    struct timespec _diff = chrono_duration(pT1, pT2);
    return (double)_diff.tv_sec + (double)(_diff.tv_nsec * 1e-9);
}
