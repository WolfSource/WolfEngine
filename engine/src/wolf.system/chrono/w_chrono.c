#include "w_chrono.h"
#ifdef W_PLATFORM_WIN
#include <WinSock2.h> //just for timeval
//dump define
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC		1			
#endif

LARGE_INTEGER _get_file_time_offset()
{
	SYSTEMTIME _s;
	FILETIME _f;
	LARGE_INTEGER _t;

	_s.wYear = 1970;
	_s.wMonth = 1;
	_s.wDay = 1;
	_s.wHour = 0;
	_s.wMinute = 0;
	_s.wSecond = 0;
	_s.wMilliseconds = 0;
	SystemTimeToFileTime(&_s, &_f);
	_t.QuadPart = _f.dwHighDateTime;
	_t.QuadPart <<= 32;
	_t.QuadPart |= _f.dwLowDateTime;
	return (_t);
}

int clock_gettime(int pX, struct timeval* pTV)
{
	LARGE_INTEGER           _time;
	FILETIME				_file_time;
	double                  _microseconds;
	static LARGE_INTEGER    _offset;
	static double           _frequency_to_microseconds;
	static int              _initialized = 0;
	static BOOL             _use_performance_counter = 0;

	if (!_initialized)
	{
		LARGE_INTEGER _performance_frequency;
		_initialized = 1;
		_use_performance_counter = QueryPerformanceFrequency(&_performance_frequency);
		if (_use_performance_counter)
		{
			QueryPerformanceCounter(&_offset);
			_frequency_to_microseconds = (double)_performance_frequency.QuadPart / 1000000.;
		}
		else 
		{
			_offset = _get_file_time_offset();
			_frequency_to_microseconds = 10.;
		}
	}
	if (_use_performance_counter)
	{
		QueryPerformanceCounter(&_time);
	}
	else 
	{
		GetSystemTimeAsFileTime(&_file_time);
		_time.QuadPart = _file_time.dwHighDateTime;
		_time.QuadPart <<= 32;
		_time.QuadPart |= _file_time.dwLowDateTime;
	}

	_time.QuadPart -= _offset.QuadPart;
	_microseconds = (double)_time.QuadPart / _frequency_to_microseconds;
	_time.QuadPart = _microseconds;
	pTV->tv_sec = _time.QuadPart / 1000000;
	pTV->tv_usec = _time.QuadPart % 1000000;

	return 0;
}

#endif

struct timespec w_chrono_now(void)
{
    struct timespec _time;
    clock_gettime(CLOCK_MONOTONIC, &_time);
    return _time;
}

struct timespec w_chrono_clock_now(_In_ const int pClockType)
{
    struct timespec _time;
    clock_gettime(pClockType, &_time);
    return _time;
}

struct timespec w_chrono_duration(_In_ const struct timespec* pT1,
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

double w_chrono_now_in_sec(void)
{
    struct timespec _now = w_chrono_now();
    return (double)_now.tv_sec + (double)_now.tv_nsec / 1000000000.0;
}

double w_chrono_timespec_to_sec(_In_ const struct timespec* pT)
{
    return (double)pT->tv_sec + (double)pT->tv_nsec / 1000000000.0;
}

double w_chrono_duration_nanoseconds(_In_ const struct timespec* pT1,
                                   _In_ const struct timespec* pT2)
{
    struct timespec _diff = w_chrono_duration(pT1, pT2);
    return (double)(_diff.tv_sec * 1e+9 + _diff.tv_nsec);
}

double w_chrono_duration_microseconds(_In_ const struct timespec* pT1,
                                    _In_ const struct timespec* pT2)
{
    struct timespec _diff = w_chrono_duration(pT1, pT2);
    return (double)_diff.tv_sec * 1e+6 + (double)(_diff.tv_nsec * 1e-3);
}

double w_chrono_duration_milliseconds(_In_ const struct timespec* pT1,
                                    _In_ const struct timespec* pT2)
{
    struct timespec _diff = w_chrono_duration(pT1, pT2);
    return (double)_diff.tv_sec * 1e+3 + (double)(_diff.tv_nsec * 1e-6);
}

double w_chrono_duration_seconds(_In_ const struct timespec* pT1,
                               _In_ const struct timespec* pT2)
{
    struct timespec _diff = w_chrono_duration(pT1, pT2);
    return (double)_diff.tv_sec + (double)(_diff.tv_nsec * 1e-9);
}
