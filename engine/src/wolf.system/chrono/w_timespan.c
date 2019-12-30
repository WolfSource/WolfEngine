#include "w_timespan.h"
#include <time.h>

#define TICKS_PER_MILLISECOND	10000i64
#define TICKS_PER_SECOND		TICKS_PER_MILLISECOND * 1000i64
#define TICKS_PER_MINUTE		TICKS_PER_SECOND * 60i64
#define TICKS_PER_HOUR			TICKS_PER_MINUTE * 60i64
#define TICKS_PER_DAY			TICKS_PER_HOUR * 24i64
#define MILLIS_PER_SECOND		1000i64
#define MILLIS_PER_MINUTE		MILLIS_PER_SECOND * 60i64
#define MILLIS_PER_HOUR			MILLIS_PER_MINUTE * 60i64
#define MILLIS_PER_DAY			MILLIS_PER_HOUR * 24i64

static const double MILLI_SECONDS_PER_TICK = 1.0 / TICKS_PER_MILLISECOND;
static const double SECONDS_PER_TICK = 1.0 / TICKS_PER_SECOND;
static const double MINUTES_PER_TICK = 1.0 / TICKS_PER_MINUTE;
static const double HOURS_PER_TICK = 1.0 / TICKS_PER_HOUR;
static const double DAYS_PER_TICK = 1.0 / TICKS_PER_DAY;
static const int64_t MAX_MILLISECONDS = LLONG_MAX / TICKS_PER_MILLISECOND;
static const int64_t MIN_MILLISECONDS = LLONG_MIN / TICKS_PER_MILLISECOND;

const long _timespan_w_wcstol(const wchar_t* const pValue, bool* pError)
{
    wchar_t* _endptr;
    long _long = wcstol(pValue, &_endptr, 0);
    if (errno == ERANGE || *_endptr != '\0' || pValue == _endptr)
    {
        W_ASSERT(false, "strtol got error. trace info: _timespan_w_wcstol");
        *pError = true;
        return -1;
    }
    if(_long < INT_MIN || _long > INT_MAX)
    {
        W_ASSERT(false, "strtol got error. trace info: _timespan_w_wcstol");
        *pError = true;
        return -1;
    }
    return _long;
}

int64_t _time_to_ticks(_In_ const int64_t pDays,
                       _In_ const int64_t pHours,
                       _In_ const int64_t pMinutes,
                       _In_ const int64_t pSeconds,
                       _In_ const int64_t pMilliSeconds,
                       _Inout_ bool* pOverflowed)
{
    int64_t _total_milliSeconds = pDays * 864 * 100000 +
                                  pHours * 36 * 100000 +
                                  pMinutes * 60 * 1000 +
                                  pSeconds * 1000 +
                                  pMilliSeconds;
                              
    if (_total_milliSeconds > MAX_MILLISECONDS || _total_milliSeconds < MIN_MILLISECONDS)
    {
        W_ASSERT(false, "timespan overflowed. trace info: _timespan_time_to_ticks");
        *pOverflowed = true;
        return -1;
    }
    *pOverflowed = false;
    return _total_milliSeconds * TICKS_PER_MILLISECOND;
}

int64_t _interval(_In_ const double pValue, _In_ const int pScale, _Inout_ bool* pOverflowed)
{
    double _tmp = pValue * pScale;
    double _millis = _tmp + (pValue >= 0 ? 0.5 : -0.5);
    if ((_millis > LLONG_MAX / TICKS_PER_MILLISECOND) || (_millis < LLONG_MIN / TICKS_PER_MILLISECOND))
    {
        //Overflow TimeSpan too long
        W_ASSERT(false, "timespan overflowed. trace info: _timespan_interval");
        *pOverflowed = true;
        return -1;
    }
    return (int64_t)(_millis) * TICKS_PER_MILLISECOND;
}

w_timespan* w_timespan_init_from_zero(void)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = 0;
    _timespan->overflowed = false;
    return _timespan;
}

w_timespan* w_timespan_init_from_min_value(void)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = LLONG_MIN;
    _timespan->overflowed = false;
    return _timespan;
}

w_timespan* w_timespan_init_from_max_value(void)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = LLONG_MAX;
    _timespan->overflowed = false;
    return _timespan;
}

w_timespan* w_timespan_init_from_now(void)
{
    // get current time
    time_t t = time(0);
#ifdef W_PLATFORM_UNIX
    struct tm* _now = localtime(&t);
	return w_timespan_init_from_shorttime(_now->tm_hour, _now->tm_min, _now->tm_sec);
#else
    struct tm _now;
    localtime_s(&_now, &t);
	return w_timespan_init_from_shorttime(_now.tm_hour, _now.tm_min, _now.tm_sec);
#endif
}

w_timespan* w_timespan_init_from_days(_In_ const double pValue)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = _interval(pValue, MILLIS_PER_DAY, &_timespan->overflowed);
    return _timespan;
}

w_timespan* w_timespan_init_from_hours(_In_ const double pValue)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = _interval(pValue, MILLIS_PER_HOUR, &_timespan->overflowed);
    return _timespan;
}

w_timespan* w_timespan_init_from_minutes(_In_ const double pValue)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = _interval(pValue, MILLIS_PER_MINUTE, &_timespan->overflowed);
    return _timespan;
}

w_timespan* w_timespan_init_from_seconds(_In_ const double pValue)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = _interval(pValue, MILLIS_PER_SECOND, &_timespan->overflowed);
    return _timespan;
}

w_timespan* w_timespan_init_from_milliseconds(_In_ const double pValue)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = _interval(pValue, 1, &_timespan->overflowed);
    return _timespan;
}

w_timespan* w_timespan_init_from_ticks(_In_ const int64_t pTicks)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    if(pTicks > LLONG_MAX || LLONG_MAX < LLONG_MIN)
    {
        _timespan->overflowed = true;
        W_ASSERT(false, "timespan overflowed. trace info: init_timespan_with_ticks");
    }
    else
    {
        _timespan->overflowed = false;
        _timespan->ticks = pTicks;
    }
    return _timespan;
}

w_timespan* w_timespan_init_from_shorttime(_In_ const int pHours,
                                           _In_ const int pMinutes,
                                           _In_ const int pSeconds)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = _time_to_ticks(
                                      0,
                                      pHours,
                                      pMinutes,
                                      pSeconds,
                                      0,
                                      &_timespan->overflowed);
    return _timespan;
}

w_timespan* w_timespan_init_from_longtime(_In_ const int pDays,
                                        _In_ const int pHours,
                                        _In_ const int pMinutes,
                                        _In_ const int pSeconds,
                                        _In_ const int pMilliseconds)
{
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    _timespan->ticks = _time_to_ticks(
                                      pDays,
                                      pHours,
                                      pMinutes,
                                      pSeconds,
                                      pMilliseconds,
                                      &_timespan->overflowed);
    return _timespan;
}

w_timespan* w_timespan_init_from_string(const char* const pValue)
{
    int _days = 0, _hours = 0, _minutes = 0, _seconds = 0, _milliseconds = 0;
    int _c = 0, _i = 0, _j = 0;
    char _number[256];
    while (pValue[_c] != '\0')
    {
        if (pValue[_c] == ':')
        {
            _number[_j] = '\0';
            _j = 0;
            
            if (_i == 0)
            {
                _days = atoi(_number);
            }
            else if (_i == 1)
            {
                _hours = atoi(_number);
            }
            else if (_i == 2)
            {
                _minutes = atoi(_number);
            }
            else if (_i == 3)
            {
                _seconds = atoi(_number);
            }
            _i++;
        }
        _number[_j++] = pValue[_c++];
    }
    
    if(_j)
    {
        _number[_j] = '\0';
        _milliseconds = atoi(_number);
    }

    return w_timespan_init_from_longtime(_days, _hours, _minutes, _seconds, _milliseconds);
}

w_timespan* w_timespan_init_from_wstring(const wchar_t* const pValue)
{
    bool _error;
    long _temp;
    int _days = 0, _hours = 0, _minutes = 0, _seconds = 0, _milliseconds = 0;
    int _c = 0, _i = 0, _j = 0;
    wchar_t _number[256];
    while (pValue[_c] != L'\0')
    {
        if (pValue[_c] == L':')
        {
            _number[_j] = L'\0';
            _j = 0;
            
            if (_i == 0)
            {
                _temp = _timespan_w_wcstol(_number, &_error);
                if (!_error)
                {
                    _days = (int)_temp;
                }
            }
            else if (_i == 1)
            {
                _temp = _timespan_w_wcstol(_number, &_error);
                if (!_error)
                {
                    _hours = (int)_temp;
                }
            }
            else if (_i == 2)
            {
                _temp = _timespan_w_wcstol(_number, &_error);
                if (!_error)
                {
                    _minutes = (int)_temp;
                }
            }
            else if (_i == 3)
            {
                _temp = _timespan_w_wcstol(_number, &_error);
                if (!_error)
                {
                    _seconds = (int)_temp;
                }
            }
            _i++;
        }
        _number[_j++] = pValue[_c++];
    }
    
    if(_j)
    {
        _number[_j] = L'\0';
        _temp = _timespan_w_wcstol(_number, &_error);
        if (!_error)
        {
            _milliseconds = (int)_temp;
        }
    }

    return w_timespan_init_from_longtime(_days, _hours, _minutes, _seconds, _milliseconds);
}

w_timespan* w_timespan_add(_Inout_ w_timespan* pLValue, _In_ const w_timespan* pRValue)
{
    W_ASSERT(!pLValue || !pRValue, "input parameters of timespan_add function are NULL. trace info: timespan_add");
    
    w_timespan* _timespan = (w_timespan*)malloc(sizeof(w_timespan));
    
    _timespan->ticks = pLValue->ticks + pRValue->ticks;
    // Overflow if signs of operands was identical and result's sign was opposite.
    // >> 63 gives the sign bit (either 64 1's or 64 0's).
    if (((pLValue->ticks >> 63) == (pRValue->ticks >> 63)) &&
        ((pLValue->ticks >> 63) != (_timespan->ticks >> 63)))
    {
        //OverflowException: TimeSpan is too Long
        _timespan->overflowed = true;
        W_ASSERT(false, "timespan overflowed in timespan_add");
    }
    return _timespan;
}

void w_timespan_add_by_ref(_Inout_ w_timespan* pLValue, _In_ const w_timespan* pRValue)
{
    W_ASSERT(!pLValue || !pRValue, "input parameters of timespan_add_by_ref function are NULL. trace info: timespan_add");
    
    int64_t _result = pLValue->ticks + pRValue->ticks;
    // Overflow if signs of operands was identical and result's sign was opposite.
    // >> 63 gives the sign bit (either 64 1's or 64 0's).
    if (((pLValue->ticks >> 63) == (pRValue->ticks >> 63)) &&
        ((pLValue->ticks >> 63) != (_result >> 63)))
    {
        //OverflowException: TimeSpan is too Long
        pLValue->overflowed = true;
        W_ASSERT(false, "timespan overflowed. trace info: timespan_add");
    }
    pLValue->ticks = _result;
}

const char* w_timespan_to_string(_In_ const w_timespan* const pValue)
{
    char* _str = (char*)malloc(PATH_MAX);
    sprintf(_str, "%d:%d:%d:%d:%03d",
            w_timespan_get_days(pValue),
            w_timespan_get_hours(pValue),
            w_timespan_get_minutes(pValue),
            w_timespan_get_seconds(pValue),
            w_timespan_get_milliseconds(pValue));
    return _str;
}

const wchar_t* w_timespan_to_wstring(_In_ const w_timespan* const pValue)
{
    wchar_t* _str = (wchar_t*)malloc(PATH_MAX);
    swprintf(_str, PATH_MAX, L"%d:%d:%d:%d:%03d",
            w_timespan_get_days(pValue),
            w_timespan_get_hours(pValue),
            w_timespan_get_minutes(pValue),
            w_timespan_get_seconds(pValue),
            w_timespan_get_milliseconds(pValue));
    return _str;
}

#pragma region Getters

const int w_timespan_get_days(_In_ const w_timespan* const pValue)
{
    return (int)(pValue->ticks / TICKS_PER_DAY);
}

const double w_timespan_get_total_days(_In_ const w_timespan* const pValue)
{
    return (double)pValue->ticks * DAYS_PER_TICK;
}

const int w_timespan_get_hours(_In_ const w_timespan* const pValue)
{
    return ((int)(pValue->ticks / TICKS_PER_HOUR) % 24);
}

const double w_timespan_get_total_hours(_In_ const w_timespan* const pValue)
{
    return (double)(pValue->ticks) * HOURS_PER_TICK;
}

const int w_timespan_get_minutes(_In_ const w_timespan* const pValue)
{
    return ((int)(pValue->ticks / TICKS_PER_MINUTE) % 60);
}

const double w_timespan_get_total_minutes(_In_ const w_timespan* const pValue)
{
    return (double)(pValue->ticks) * MINUTES_PER_TICK;
}

const int w_timespan_get_seconds(_In_ const w_timespan* const pValue)
{
    return ((int)(pValue->ticks / TICKS_PER_SECOND) % 60);
}

const double w_timespan_get_total_seconds(_In_ const w_timespan* const pValue)
{
    return (double)(pValue->ticks) * SECONDS_PER_TICK;
}

const int w_timespan_get_milliseconds(_In_ const w_timespan* const pValue)
{
    return ((int)(pValue->ticks / TICKS_PER_MILLISECOND) % 1000);
}

const double w_timespan_get_total_milliseconds(_In_ const w_timespan* const pValue)
{
    double _temp = (double)(pValue->ticks) * MILLI_SECONDS_PER_TICK;
    if (_temp > MAX_MILLISECONDS) return (double)MAX_MILLISECONDS;
    if (_temp < MIN_MILLISECONDS) return (double)MIN_MILLISECONDS;
    return _temp;
}

const char* w_timespan_get_current_date_time_string()
{
    //get current time
    char* _str = (char*)malloc(PATH_MAX);

    time_t t = time(0);
#if defined(W_PLATFORM_UNIX)
        struct tm* _now = localtime(&t);
        sprintf(_str, "%02d_%02d_%04d__%02d_%02d_%02d",
                _now->tm_mday,
                _now->tm_mon,
                _now->tm_year + 1900,
                _now->tm_hour,
                _now->tm_min,
                _now->tm_sec);

#else
        struct tm _now;
        localtime_s(&_now, &t);
        sprintf(_str, "%02d_%02d_%04d__%02d_%02d_%02d",
            _now.tm_mday,
            _now.tm_mon,
            _now.tm_year + 1900,
            _now.tm_hour,
            _now.tm_min,
            _now.tm_sec);
#endif

    return _str;
}

const wchar_t* w_timespan_get_current_date_time_wstring()
{
    //get current time
    wchar_t* _str = (wchar_t*)malloc(PATH_MAX);

    time_t t = time(0);
#if defined(W_PLATFORM_UNIX)
        struct tm* _now = localtime(&t);
        swprintf(_str, PATH_MAX, L"%d:%d:%d %d:%d:%d",
                _now->tm_mday,
                _now->tm_mon,
                _now->tm_year + 1900,
                _now->tm_hour,
                _now->tm_min,
                _now->tm_sec);
#else
        struct tm _now;
        localtime_s(&_now, &t);
        swprintf(_str, PATH_MAX, L"%d:%d:%d %d:%d:%d",
            _now.tm_mday,
            _now.tm_mon,
            _now.tm_year + 1900,
            _now.tm_hour,
            _now.tm_min,
            _now.tm_sec);
#endif

    return _str;
}

#pragma endregion

