#include "w_system_pch.h"
#include "w_time_span.h"

using namespace std;
using namespace wolf::system;

#pragma region Constants

const INT64 TICKS_PER_MILLISECOND = 10000;
const double MILLI_SECONDS_PER_TICK = 1.0 / TICKS_PER_MILLISECOND;

const INT64 TICKS_PER_SECOND = TICKS_PER_MILLISECOND * 1000;
const double SECONDS_PER_TICK = 1.0 / TICKS_PER_SECOND;

const INT64 TICKS_PER_MINUTE = TICKS_PER_SECOND * 60;
const double MINUTES_PER_TICK = 1.0 / TICKS_PER_MINUTE;

const INT64 TICKS_PER_HOUR = TICKS_PER_MINUTE * 60;
const double HOURS_PER_TICK = 1.0 / TICKS_PER_HOUR;

const INT64 TICKS_PER_DAY = TICKS_PER_HOUR * 24;
const double DAYS_PER_TICK = 1.0 / TICKS_PER_DAY;

const int MILLIS_PER_SECOND = 1000;
const int MILLIS_PER_MINUTE = MILLIS_PER_SECOND * 60;
const int MILLIS_PER_HOUR = MILLIS_PER_MINUTE * 60;
const int MILLIS_PER_DAY = MILLIS_PER_HOUR * 24;

const INT64 MaxSeconds = _I64_MAX / TICKS_PER_SECOND;
const INT64 MinSeconds = _I64_MIN / TICKS_PER_SECOND;

const INT64 MaxMilliSeconds = _I64_MAX / TICKS_PER_MILLISECOND;
const INT64 MinMilliSeconds = _I64_MIN / TICKS_PER_MILLISECOND;

const INT64 TicksPerTenthSecond = TICKS_PER_MILLISECOND * 100;

#pragma endregion

w_time_span::w_time_span() : _ticks(0)
{
}

w_time_span::w_time_span(INT64 pTicks) : _ticks(pTicks)
{
}

w_time_span::w_time_span(int pHours, int pMinutes, int pSeconds)
{
	this->_ticks = _time_to_ticks(pHours, pMinutes, pSeconds);
}

w_time_span::w_time_span(int pDays, int pHours, int pMinutes, int pSeconds, int pMilliseconds)
{
	INT64 totalMilliSeconds = (static_cast<INT64>(pDays) * 3600 * 24 +
		static_cast<INT64>(pHours) * 3600 + static_cast<INT64>(pMinutes) * 60 + pSeconds) * 1000 + pMilliseconds;

	if (totalMilliSeconds > MaxMilliSeconds || totalMilliSeconds < MinMilliSeconds)
	{
		throw "ArgumentOutOfRangeException: Overflow W_TimeSpan is too long";
	}
	this->_ticks = static_cast<INT64>(totalMilliSeconds) * TICKS_PER_MILLISECOND;
}

w_time_span::~w_time_span()
{
}

w_time_span w_time_span::add(w_time_span pTS)
{
	auto result = this->_ticks + pTS._ticks;
	// Overflow if signs of operands was identical and result's sign was opposite.
	// >> 63 gives the sign bit (either 64 1's or 64 0's).
	if (((this->_ticks >> 63) == (pTS._ticks >> 63)) && ((this->_ticks >> 63) != (result >> 63)))
	{
		throw "OverflowException: TimeSpan is too Long";
	}
	return w_time_span(result);
}

w_time_span w_time_span::duration()
{
	if ((this->_ticks <= _I64_MIN || this->_ticks >= _I64_MAX))
	{
		throw "OverflowException: Overflow Duration";
	}
	return w_time_span(this->_ticks >= 0 ? this->_ticks : -this->_ticks);
}

w_time_span w_time_span::negate()
{
	return w_time_span(-this->_ticks);
}

string w_time_span::to_string()
{
	return std::to_string(get_days()) + ":" + 
		std::to_string(get_hours()) + ":" + 
		std::to_string(get_minutes()) + ":" + 
		std::to_string(get_seconds()) + ":" + 
		std::to_string(get_milliseconds());
}

wstring w_time_span::to_wstring()
{
	auto _str = this->to_string();
	auto _wstr = std::wstring(_str.begin(), _str.end());
	_str.clear();
	return _wstr;
}

w_time_span w_time_span::zero()
{ 
	return w_time_span(0);
}

w_time_span w_time_span::now()
{
	time_t t = time(0);   // get time now
	struct tm* now = nullptr;
#ifdef __ANDROID
	now = localtime(&t);
#else
	localtime_s(now, &t);
#endif
	return w_time_span(now->tm_hour, now->tm_min, now->tm_sec);
}

w_time_span w_time_span::min_value()
{ 
	return w_time_span(_I64_MIN);
}

w_time_span w_time_span::max_value()
{ 
	return w_time_span(_I64_MAX);
}

w_time_span w_time_span::from_days(double pValue)
{
	return _interval(pValue, MILLIS_PER_DAY);
}

w_time_span w_time_span::from_hours(double pValue)
{
	return _interval(pValue, MILLIS_PER_HOUR);
}

w_time_span w_time_span::from_milliseconds(double pValue)
{
	return _interval(pValue, 1);
}

w_time_span w_time_span::from_minutes(double pValue)
{
	return _interval(pValue, MILLIS_PER_MINUTE);
}

w_time_span w_time_span::from_seconds(double pValue)
{
	return _interval(pValue, MILLIS_PER_SECOND);
}

w_time_span w_time_span::from_ticks(INT64 pValue)
{
	return w_time_span(pValue);
}

w_time_span w_time_span::from_string(std::string pValue)
{
	//Create timespan from string in the format of "00:00:00:00:000"
	std::stringstream _sstrream(pValue);

	int _days = 0, _hours = 0, _minutes = 0, _seconds = 0, _milliseconds = 0;
	int i = 0;
	char _c;
	std::string _number;
	while (_sstrream >> _c)
	{
		_number += _c;
		if (_sstrream.peek() == ':')
		{
			_sstrream.ignore();

			if (i == 0)
			{
				_days = std::atoi(_number.c_str());
			}
			else if (i == 1)
			{
				_hours = std::atoi(_number.c_str());
			}
			else if (i == 2)
			{
				_minutes = std::atoi(_number.c_str());
			}
			else if (i == 3)
			{
				_seconds = std::atoi(_number.c_str());
			}

			i++;
			_number = "";
		}
	}

	if(!_number.empty())
	{
		_milliseconds = std::atoi(_number.c_str());
	}

	_sstrream.clear();

	return w_time_span(_days, _hours, _minutes, _seconds, _milliseconds);
}

w_time_span w_time_span::_interval(double pValue, int pScale)
{
	double tmp = pValue * pScale;
	double millis = tmp + (pValue >= 0 ? 0.5 : -0.5);
	if ((millis > _I64_MAX / TICKS_PER_MILLISECOND) || (millis < _I64_MIN / TICKS_PER_MILLISECOND))
	{
		throw "Overflow TimeSpan too long";
	}
	return w_time_span(static_cast<INT64>(millis) * TICKS_PER_MILLISECOND);
}

INT64 w_time_span::_time_to_ticks(int pHour, int pMinute, int pSecond)
{
	// totalSeconds is bounded by 2^31 * 2^12 + 2^31 * 2^8 + 2^31,
	// which is less than 2^44, meaning we won't overflow totalSeconds.
	long totalSeconds = static_cast<INT64>(pHour) * 3600 + static_cast<INT64>(pMinute) * 60 + static_cast<INT64>(pSecond);
	if (totalSeconds > MaxSeconds || totalSeconds < MinSeconds)
	{
		throw "ArgumentOutOfRangeException: Overflow TimeSpan too long";
	}
	return totalSeconds * TICKS_PER_SECOND;
}

#pragma region Getters

INT64 w_time_span::get_ticks() const
{ 
	return this->_ticks; 
}

int w_time_span::get_days() const
{ 
	return static_cast<int>(this->_ticks / TICKS_PER_DAY);
}

int w_time_span::get_hours() const
{ 
	return static_cast<int>((this->_ticks / TICKS_PER_HOUR) % 24);
}

int w_time_span::get_milliseconds() const
{ 
	return static_cast<int>((this->_ticks / TICKS_PER_MILLISECOND) % 1000);
}

int w_time_span::get_minutes() const
{ 
	return static_cast<int>((this->_ticks / TICKS_PER_MINUTE) % 60);
}

int w_time_span::get_seconds() const
{ 
	return static_cast<int>((this->_ticks / TICKS_PER_SECOND) % 60);
}

double w_time_span::get_total_days() const
{ 
	return static_cast<double>(this->_ticks) * DAYS_PER_TICK;
}

double w_time_span::get_total_hours() const
{ 
	return static_cast<double>(this->_ticks) * HOURS_PER_TICK;
}

double w_time_span::get_total_milliseconds() const
{
	double temp = static_cast<double>(this->_ticks) * MILLI_SECONDS_PER_TICK;
	if (temp > MaxMilliSeconds) return static_cast<double>(MaxMilliSeconds);
	if (temp < MinMilliSeconds) return static_cast<double>(MinMilliSeconds);
	return temp;
}

double w_time_span::get_total_minutes() const
{ 
	return static_cast<double>(this->_ticks) * MINUTES_PER_TICK;
}

double w_time_span::get_total_seconds() const
{ 
	return static_cast<double>(this->_ticks) * SECONDS_PER_TICK;
}

#pragma endregion                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  