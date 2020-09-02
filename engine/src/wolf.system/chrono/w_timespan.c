#include "w_timespan.h"
#include <time.h>
#include <apr-2/apr_general.h>

#define TICKS_PER_MILLISECOND	10000
#define TICKS_PER_SECOND		10000000
#define TICKS_PER_MINUTE		600000000
#define TICKS_PER_HOUR			36000000000
#define TICKS_PER_DAY			864000000000
#define MILLIS_PER_SECOND		1000
#define MILLIS_PER_MINUTE		60000
#define MILLIS_PER_HOUR			3600000
#define MILLIS_PER_DAY			86400000

static const double MILLI_SECONDS_PER_TICK = 1.0 / TICKS_PER_MILLISECOND;
static const double SECONDS_PER_TICK = 1.0 / TICKS_PER_SECOND;
static const double MINUTES_PER_TICK = 1.0 / TICKS_PER_MINUTE;
static const double HOURS_PER_TICK = 1.0 / TICKS_PER_HOUR;
static const double DAYS_PER_TICK = 1.0 / TICKS_PER_DAY;
static const int64_t MAX_MILLISECONDS = LLONG_MAX / TICKS_PER_MILLISECOND;
static const int64_t MIN_MILLISECONDS = LLONG_MIN / TICKS_PER_MILLISECOND;

long _timespan_w_wcstol(const wchar_t* const pValue, bool* pError)
{
    wchar_t* _endptr;
    long _long = wcstol(pValue, &_endptr, 0);
    if (errno == ERANGE || *_endptr != '\0' || pValue == _endptr)
    {
        W_ASSERT(false, "strtol got error. trace info: _timespan_w_wcstol");
        *pError = true;
        return -1;
    }
    if (_long < INT_MIN || _long > INT_MAX)
    {
        W_ASSERT(false, "strtol got error. trace info: _timespan_w_wcstol");
        *pError = true;
        return -1;
    }
    return _long;
}

int64_t _time_to_ticks(_In_ int64_t pDays,
    _In_ int64_t pHours,
    _In_ int64_t pMinutes,
    _In_ int64_t pSeconds,
    _In_ int64_t pMilliSeconds,
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

int64_t _interval(_In_ double pValue, _In_ int pScale, _Inout_ bool* pOverflowed)
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
    *pOverflowed = false;
    return (int64_t)(_millis)*TICKS_PER_MILLISECOND;
}

w_timespan w_timespan_init_from_zero(_Inout_ w_mem_pool pMemPool)
{
    const char* _trace_info = "w_timespan_init_from_zero";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = 0;
        _timespan->overflowed = false;
    }
    return _timespan;
}

w_timespan w_timespan_init_from_min_value(_Inout_ w_mem_pool pMemPool)
{
    const char* _trace_info = "w_timespan_init_from_min_value";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = LLONG_MIN;
        _timespan->overflowed = false;
    }
    return _timespan;
}

w_timespan w_timespan_init_from_max_value(_Inout_ w_mem_pool pMemPool)
{
    const char* _trace_info = "w_timespan_init_from_max_value";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = LLONG_MAX;
        _timespan->overflowed = false;
    }
    return _timespan;
}

w_timespan w_timespan_init_from_now(_Inout_ w_mem_pool pMemPool)
{
    // get current time
    time_t t = time(0);
#ifdef W_PLATFORM_UNIX
    struct tm* _now = localtime(&t);
    return w_timespan_init_from_shorttime(pMemPool, _now->tm_hour, _now->tm_min, _now->tm_sec);
#else
    struct tm _now;
    localtime_s(&_now, &t);
    return w_timespan_init_from_shorttime(pMemPool, _now.tm_hour, _now.tm_min, _now.tm_sec);
#endif
}

w_timespan w_timespan_init_from_days(
    _Inout_ w_mem_pool pMemPool,
    _In_ double pValue)
{
    const char* _trace_info = "w_timespan_init_from_days";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = _interval(pValue, MILLIS_PER_DAY, &_timespan->overflowed);
    }
    return _timespan;
}

w_timespan w_timespan_init_from_hours(
    _Inout_ w_mem_pool pMemPool,
    _In_ double pValue)
{
    const char* _trace_info = "w_timespan_init_from_hours";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = _interval(pValue, MILLIS_PER_HOUR, &_timespan->overflowed);
    }
    return _timespan;
}

w_timespan w_timespan_init_from_minutes(
    _Inout_ w_mem_pool pMemPool,
    _In_ double pValue)
{
    const char* _trace_info = "w_timespan_init_from_minutes";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = _interval(pValue, MILLIS_PER_MINUTE, &_timespan->overflowed);
    }
    return _timespan;
}

w_timespan w_timespan_init_from_seconds(
    _Inout_ w_mem_pool pMemPool,
    _In_ double pValue)
{
    const char* _trace_info = "w_timespan_init_from_seconds";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = _interval(pValue, MILLIS_PER_SECOND, &_timespan->overflowed);
    }
    return _timespan;
}

w_timespan w_timespan_init_from_milliseconds(
    _Inout_ w_mem_pool pMemPool,
    _In_ double pValue)
{
    const char* _trace_info = "w_timespan_init_from_milliseconds";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = _interval(pValue, 1, &_timespan->overflowed);
    }
    return _timespan;
}

w_timespan w_timespan_init_from_ticks(
    _Inout_ w_mem_pool pMemPool,
    _In_ int64_t pTicks)
{
    const char* _trace_info = "w_timespan_init_from_ticks";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        if (pTicks > LLONG_MAX || LLONG_MAX < LLONG_MIN)
        {
            _timespan->overflowed = true;
            W_ASSERT(false, "timespan overflowed. trace info: init_timespan_with_ticks");
        }
        else
        {
            _timespan->overflowed = false;
            _timespan->ticks = pTicks;
        }
    }
    return _timespan;
}

w_timespan w_timespan_init_from_shorttime(
    _Inout_ w_mem_pool pMemPool,
    _In_ int pHours,
    _In_ int pMinutes,
    _In_ int pSeconds)
{
    const char* _trace_info = "w_timespan_init_from_shorttime";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = _time_to_ticks(
            0,
            pHours,
            pMinutes,
            pSeconds,
            0,
            &_timespan->overflowed);
    }
    return _timespan;
}

w_timespan w_timespan_init_from_longtime(
    _Inout_ w_mem_pool pMemPool,
    _In_ int pDays,
    _In_ int pHours,
    _In_ int pMinutes,
    _In_ int pSeconds,
    _In_ int pMilliseconds)
{
    const char* _trace_info = "w_timespan_init_from_longtime";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (_timespan)
    {
        _timespan->ticks = _time_to_ticks(
            pDays,
            pHours,
            pMinutes,
            pSeconds,
            pMilliseconds,
            &_timespan->overflowed);
    }
    return _timespan;
}

w_timespan w_timespan_init_from_string(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* const pValue)
{
    const char* _trace_info = "w_timespan_init_from_string";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }

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
            _c++;
        }
        _number[_j++] = pValue[_c++];
    }

    if (_j)
    {
        _number[_j] = '\0';
        _milliseconds = atoi(_number);
    }

    return w_timespan_init_from_longtime(pMemPool, _days, _hours, _minutes, _seconds, _milliseconds);
}

w_timespan w_timespan_init_from_wstring(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const wchar_t* const pValue)
{
    const char* _trace_info = "w_timespan_init_from_wstring";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }

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
                if (_error)
                {
                    _days = (int)_temp;
                }
            }
            else if (_i == 1)
            {
                _temp = _timespan_w_wcstol(_number, &_error);
                if (_error)
                {
                    _hours = (int)_temp;
                }
            }
            else if (_i == 2)
            {
                _temp = _timespan_w_wcstol(_number, &_error);
                if (_error)
                {
                    _minutes = (int)_temp;
                }
            }
            else if (_i == 3)
            {
                _temp = _timespan_w_wcstol(_number, &_error);
                if (_error)
                {
                    _seconds = (int)_temp;
                }
            }
            _i++;
            _c++;
        }
        _number[_j++] = pValue[_c++];
    }

    if (_j)
    {
        _number[_j] = L'\0';
        _temp = _timespan_w_wcstol(_number, &_error);
        if (_error)
        {
            _milliseconds = (int)_temp;
        }
    }

    return w_timespan_init_from_longtime(pMemPool, _days, _hours, _minutes, _seconds, _milliseconds);
}

w_timespan w_timespan_add(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_timespan pLValue, 
    _In_ const w_timespan pRValue)
{
    W_ASSERT(pLValue || pRValue, "input parameters of timespan_add function are NULL. trace info: timespan_add");

    w_timespan _timespan = (w_timespan)w_malloc(pMemPool, sizeof(w_timespan_t));
    if (!_timespan)
    {
        return NULL;
    }
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
    _timespan->overflowed = false;
    return _timespan;
}

void w_timespan_add_by_ref(_Inout_ w_timespan pLValue, _In_ const w_timespan pRValue)
{
    W_ASSERT(pLValue || pRValue, "input parameters of timespan_add_by_ref function are NULL. trace info: timespan_add");

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
    pLValue->overflowed = false;
    pLValue->ticks = _result;
}

const char* w_timespan_to_string(
    _Inout_ w_mem_pool pMemPool,
    _In_ const w_timespan pValue,
    _In_z_ const char* pSplitCharacter)
{
    const char* _trace_info = "w_timespan_to_string";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    char* _str = (char*)w_malloc(pMemPool, W_MAX_BUFFER_SIZE);
    if (!_str)
    {
        W_ASSERT_P(false, "could not allocate memory for str. trace info: %s", _trace_info);
        return NULL;
    }

    sprintf(_str, "%d%s%d%s%d%s%d%s%03d",
        w_timespan_get_days(pValue),
        pSplitCharacter,
        w_timespan_get_hours(pValue),
        pSplitCharacter,
        w_timespan_get_minutes(pValue),
        pSplitCharacter,
        w_timespan_get_seconds(pValue),
        pSplitCharacter,
        w_timespan_get_milliseconds(pValue));
    return _str;
}

const wchar_t* w_timespan_to_wstring(
    _Inout_ w_mem_pool pMemPool,
    _In_ const w_timespan pValue,
    _In_z_ const wchar_t* pSplitCharacter)
{
    const char* _trace_info = "w_timespan_to_wstring";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    wchar_t* _str = (wchar_t*)w_malloc(pMemPool, W_MAX_BUFFER_SIZE);
    if (!_str)
    {
        W_ASSERT_P(false, "could not allocate memory for str. trace info: %s", _trace_info);
        return NULL;
    }

    swprintf(_str, W_MAX_BUFFER_SIZE,L"%d%ls%d%ls%d%ls%d%ls%03d",
        w_timespan_get_days(pValue),
        pSplitCharacter,
        w_timespan_get_hours(pValue),
        pSplitCharacter,
        w_timespan_get_minutes(pValue),
        pSplitCharacter,
        w_timespan_get_seconds(pValue),
        pSplitCharacter,
        w_timespan_get_milliseconds(pValue));

    return _str;
}

#pragma region Getters

int w_timespan_get_days(_In_ const w_timespan pValue)
{
    return (int)(pValue->ticks / TICKS_PER_DAY);
}

double w_timespan_get_total_days(_In_ const w_timespan pValue)
{
    return (double)pValue->ticks * DAYS_PER_TICK;
}

int w_timespan_get_hours(_In_ const w_timespan pValue)
{
    return ((int)(pValue->ticks / TICKS_PER_HOUR) % 24);
}

double w_timespan_get_total_hours(_In_ const w_timespan pValue)
{
    return (double)(pValue->ticks) * HOURS_PER_TICK;
}

int w_timespan_get_minutes(_In_ const w_timespan pValue)
{
    return ((int)(pValue->ticks / TICKS_PER_MINUTE) % 60);
}

double w_timespan_get_total_minutes(_In_ const w_timespan pValue)
{
    return (double)(pValue->ticks) * MINUTES_PER_TICK;
}

int w_timespan_get_seconds(_In_ const w_timespan pValue)
{
    return ((int)(pValue->ticks / TICKS_PER_SECOND) % 60);
}

double w_timespan_get_total_seconds(_In_ const w_timespan pValue)
{
    return (double)(pValue->ticks) * SECONDS_PER_TICK;
}

int w_timespan_get_milliseconds(_In_ const w_timespan pValue)
{
    return ((int)(pValue->ticks / TICKS_PER_MILLISECOND) % 1000);
}

double w_timespan_get_total_milliseconds(_In_ const w_timespan pValue)
{
    double _temp = (double)(pValue->ticks) * MILLI_SECONDS_PER_TICK;
    if (_temp > MAX_MILLISECONDS) return (double)MAX_MILLISECONDS;
    if (_temp < MIN_MILLISECONDS) return (double)MIN_MILLISECONDS;
    return _temp;
}

const char* w_timespan_get_current_date_time_string(_Inout_ w_mem_pool pMemPool)
{
    const char* _trace_info = "w_timespan_get_current_date_time_string";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    //get current time
    char* _str = (char*)w_malloc(
        pMemPool,
        W_MAX_BUFFER_SIZE);
    if (!_str)
    {
        return NULL;
    }
    time_t t = time(0);

#ifdef W_PLATFORM_WIN
    struct tm _now;
    localtime_s(&_now, &t);
    sprintf(_str, "%02d_%02d_%04d__%02d_%02d_%02d",
        _now.tm_mday,
        _now.tm_mon,
        _now.tm_year + 1900,
        _now.tm_hour,
        _now.tm_min,
        _now.tm_sec);
#else
    struct tm* _now = localtime(&t);
    sprintf(_str, "%02d_%02d_%04d__%02d_%02d_%02d",
        _now->tm_mday,
        _now->tm_mon,
        _now->tm_year + 1900,
        _now->tm_hour,
        _now->tm_min,
        _now->tm_sec);
#endif

    return _str;
}

const wchar_t* w_timespan_get_current_date_time_wstring(_Inout_ w_mem_pool pMemPool)
{
    const char* _trace_info = "w_timespan_get_current_date_time_wstring";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
        return NULL;
    }
    //get current time
    wchar_t* _str = (wchar_t*)w_malloc(pMemPool, W_MAX_BUFFER_SIZE);
    if (!_str)
    {
        return NULL;
    }

    time_t t = time(0);
#ifdef W_PLATFORM_WIN
    struct tm _now;
    localtime_s(&_now, &t);
    swprintf(_str, W_MAX_BUFFER_SIZE, L"%d:%d:%d %d:%d:%d",
        _now.tm_mday,
        _now.tm_mon,
        _now.tm_year + 1900,
        _now.tm_hour,
        _now.tm_min,
        _now.tm_sec);
#else
    struct tm* _now = localtime(&t);
    swprintf(_str, W_MAX_BUFFER_SIZE, L"%d:%d:%d %d:%d:%d",
        _now->tm_mday,
        _now->tm_mon,
        _now->tm_year + 1900,
        _now->tm_hour,
        _now->tm_min,
        _now->tm_sec);
#endif

    return _str;
}

#pragma endregion
