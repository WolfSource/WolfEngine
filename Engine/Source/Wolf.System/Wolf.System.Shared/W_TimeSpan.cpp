#include "Wolf.System.Shared.PCH.h"
#include "W_TimeSpan.h"
#include <exception>

#ifdef _DEBUG
#include "W_Logger.h"
#endif

using namespace std;
using namespace Wolf::System;

#pragma region Constants

const INT64 TicksPerMillisecond = 10000;
const double MillisecondsPerTick = 1.0 / TicksPerMillisecond;

const INT64 TicksPerSecond = TicksPerMillisecond * 1000;
const double SecondsPerTick = 1.0 / TicksPerSecond;

const INT64 TicksPerMinute = TicksPerSecond * 60;
const double MinutesPerTick = 1.0 / TicksPerMinute;

const INT64 TicksPerHour = TicksPerMinute * 60;
const double HoursPerTick = 1.0 / TicksPerHour;

const INT64 TicksPerDay = TicksPerHour * 24;
const double DaysPerTick = 1.0 / TicksPerDay;

const int MillisPerSecond = 1000;
const int MillisPerMinute = MillisPerSecond * 60;
const int MillisPerHour = MillisPerMinute * 60;
const int MillisPerDay = MillisPerHour * 24;

const INT64 MaxSeconds = _I64_MAX / TicksPerSecond;
const INT64 MinSeconds = _I64_MIN / TicksPerSecond;

const INT64 MaxMilliSeconds = _I64_MAX / TicksPerMillisecond;
const INT64 MinMilliSeconds = _I64_MIN / TicksPerMillisecond;

const INT64 TicksPerTenthSecond = TicksPerMillisecond * 100;


#pragma endregion

W_TimeSpan::W_TimeSpan() : _ticks(0)
{
}

W_TimeSpan::W_TimeSpan(INT64 pTicks) : _ticks(pTicks)
{
}

W_TimeSpan::W_TimeSpan(int pHours, int pMinutes, int pSeconds)
{
	this->_ticks = TimeToTicks(pHours, pMinutes, pSeconds);
}

W_TimeSpan::W_TimeSpan(int pDays, int pHours, int pMinutes, int pSeconds, int pMilliseconds)
{
	INT64 totalMilliSeconds = (static_cast<INT64>(pDays) * 3600 * 24 + 
		static_cast<INT64>(pHours) * 3600 + static_cast<INT64>(pMinutes) * 60 + pSeconds) * 1000 + pMilliseconds;

	if (totalMilliSeconds > MaxMilliSeconds || totalMilliSeconds < MinMilliSeconds)
	{
		throw new std::exception("ArgumentOutOfRangeException: Overflow W_TimeSpan is too long");
	}
	this->_ticks = static_cast<INT64>(totalMilliSeconds) * TicksPerMillisecond;

	//Logger.Write(std::to_wstring(pDays).c_str());
	//Logger.Write(std::to_wstring(pHours).c_str());
	//Logger.Write(std::to_wstring(pMinutes).c_str());
	//Logger.Write(std::to_wstring(pSeconds).c_str());
	//Logger.Write(std::to_wstring(pMilliseconds).c_str());
	//Logger.Write(std::to_wstring(totalMilliSeconds).c_str());
	//Logger.Write(std::to_wstring(TicksPerMillisecond).c_str());
	//Logger.Write(std::to_wstring(_ticks).c_str());
}

W_TimeSpan::~W_TimeSpan()
{
}

W_TimeSpan W_TimeSpan::Add(W_TimeSpan pTS)
{
	auto result = this->_ticks + pTS._ticks;
	// Overflow if signs of operands was identical and result's sign was opposite.
	// >> 63 gives the sign bit (either 64 1's or 64 0's).
	if (((this->_ticks >> 63) == (pTS._ticks >> 63)) && ((this->_ticks >> 63) != (result >> 63)))
	{
		throw std::exception("OverflowException: TimeSpan is too Long");
	}
	return W_TimeSpan(result);
}

W_TimeSpan W_TimeSpan::Duration() 
{
	if (this->_ticks <= _I64_MIN || this->_ticks >= _I64_MAX)
	{
		throw std::exception("OverflowException: Overflow Duration");
	}
	return W_TimeSpan(_ticks >= 0 ? _ticks : -_ticks);
}

W_TimeSpan W_TimeSpan::Negate()
{
	return W_TimeSpan(-this->_ticks);
}

string W_TimeSpan::ToString()
{
	return to_string(GetDays()) + ":" + to_string(GetHours()) + ":" + to_string(GetMinutes()) + ":" + to_string(GetSeconds()) + ":" + to_string(GetMilliseconds());
}

wstring W_TimeSpan::ToWString()
{
	auto str = ToString();
	auto wstr = std::wstring(str.begin(), str.end());
	str.clear();
	return wstr;
}

W_TimeSpan W_TimeSpan::Zero()
{ 
	return W_TimeSpan(0); 
}

W_TimeSpan W_TimeSpan::MinValue()
{ 
	return W_TimeSpan(_I64_MIN); 
}

W_TimeSpan W_TimeSpan::MaxValue()
{ 
	return W_TimeSpan(_I64_MAX); 
}

W_TimeSpan W_TimeSpan::FromDays(double pValue)
{
	return Interval(pValue, MillisPerDay);
}

W_TimeSpan W_TimeSpan::FromHours(double pValue)
{
	return Interval(pValue, MillisPerHour);
}

W_TimeSpan W_TimeSpan::FromMilliseconds(double pValue)
{
	return Interval(pValue, 1);
}

W_TimeSpan W_TimeSpan::FromMinutes(double pValue)
{
	return Interval(pValue, MillisPerMinute);
}

W_TimeSpan W_TimeSpan::FromSeconds(double pValue)
{
	return Interval(pValue, MillisPerSecond);
}

W_TimeSpan W_TimeSpan::FromTicks(INT64 pValue)
{
	return W_TimeSpan(pValue);
}

W_TimeSpan W_TimeSpan::Interval(double pValue, int pScale)
{
	double tmp = pValue * pScale;
	double millis = tmp + (pValue >= 0 ? 0.5 : -0.5);
	if ((millis > _I64_MAX / TicksPerMillisecond) || (millis < _I64_MIN / TicksPerMillisecond))
	{
		throw std::exception("Overflow TimeSpan too long");
	}
	return W_TimeSpan(static_cast<INT64>(millis) * TicksPerMillisecond);
}

INT64 W_TimeSpan::TimeToTicks(int pHour, int pMinute, int pSecond)
{
	// totalSeconds is bounded by 2^31 * 2^12 + 2^31 * 2^8 + 2^31,
	// which is less than 2^44, meaning we won't overflow totalSeconds.
	long totalSeconds = static_cast<INT64>(pHour)* 3600 + static_cast<INT64>(pMinute)* 60 + static_cast<INT64>(pSecond);
	if (totalSeconds > MaxSeconds || totalSeconds < MinSeconds)
	{
		throw std::exception("ArgumentOutOfRangeException: Overflow TimeSpan too long");
	}
	return totalSeconds * TicksPerSecond;
}

#pragma region Getters

INT64 W_TimeSpan::GetTicks() const
{ 
	return this->_ticks; 
}

int W_TimeSpan::GetDays() const
{ 
	return static_cast<int>(this->_ticks / TicksPerDay); 
}

int W_TimeSpan::GetHours() const
{ 
	return static_cast<int>((this->_ticks / TicksPerHour) % 24);
}

int W_TimeSpan::GetMilliseconds() const
{ 
	return static_cast<int>((_ticks / TicksPerMillisecond) % 1000);
}

int W_TimeSpan::GetMinutes() const
{ 
	return static_cast<int>((_ticks / TicksPerMinute) % 60);
}

int W_TimeSpan::GetSeconds() const
{ 
	return static_cast<int>((_ticks / TicksPerSecond) % 60);
}

double W_TimeSpan::GetTotalDays() const
{ 
	return static_cast<double>(_ticks) * DaysPerTick;
}

double W_TimeSpan::GetTotalHours() const
{ 
	return static_cast<double>(_ticks) * HoursPerTick;
}

double W_TimeSpan::GetTotalMilliseconds() const
{
	double temp = static_cast<double>(_ticks) * MillisecondsPerTick;
	if (temp > MaxMilliSeconds) return static_cast<double>(MaxMilliSeconds);
	if (temp < MinMilliSeconds) return static_cast<double>(MinMilliSeconds);
	return temp;
}

double W_TimeSpan::GetTotalMinutes() const
{ 
	return static_cast<double>(_ticks) * MinutesPerTick;
}

double W_TimeSpan::GetTotalSeconds() const
{ 
	return static_cast<double>(_ticks) * SecondsPerTick;
}

#pragma endregion