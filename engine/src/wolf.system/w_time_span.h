/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_time_span.h
	Description		 : time span represents a duration of time, same as TimeSpan of .NetFrameWork
	Comment			 : 10/18/2016 : Optimized for Android	
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_TIME_SPAN_H__
#define __W_TIME_SPAN_H__

#include "w_system_export.h"
#include <assert.h>
#include <string>

#if defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
#include "w_std.h"
#endif

#include "python_exporter/w_boost_python_helper.h"

namespace wolf
{
	namespace system
	{
		class w_time_span
		{
		public:
			WSYS_EXP w_time_span();
            WSYS_EXP w_time_span(_In_ bool pOverFlow);
			WSYS_EXP w_time_span(_In_ int64_t pTicks);
			WSYS_EXP w_time_span(_In_ int pHours, _In_ int pMinutes, _In_ int pSeconds);
			WSYS_EXP w_time_span(_In_ int pDays, _In_ int pHours, _In_ int pMinutes, _In_ int pSeconds, _In_ int pMilliseconds = 0);
			WSYS_EXP ~w_time_span();

			//Add two timespans with each other
			WSYS_EXP w_time_span add(w_time_span pTS);
			//Return duration in timespan
			WSYS_EXP w_time_span duration();
			//Make timespan negative
			WSYS_EXP w_time_span negate();
			//Returns time in string
			WSYS_EXP std::string to_string();
			//Returns time in wstring
			WSYS_EXP std::wstring to_wstring();

			//Returns timespan with zero value
			WSYS_EXP static w_time_span zero();
			//Returns timespan from now
			WSYS_EXP static w_time_span now();
			//Returns timespan with minimum value
			WSYS_EXP static w_time_span min_value();
			//Returns timespan with maximum value
			WSYS_EXP static w_time_span max_value();
			//Returns timeSpan from days
			WSYS_EXP static w_time_span from_days(double pValue);
			//Returns timeSpan from hours
			WSYS_EXP static w_time_span from_hours(double pValue);
			//Returns timeSpan from milliSeconds
			WSYS_EXP static w_time_span from_milliseconds(double pValue);
			//Returns timeSpan from minutes
			WSYS_EXP static w_time_span from_minutes(double pValue);
			//Returns timespan from seconds
			WSYS_EXP static w_time_span from_seconds(double pValue);
			//Returns timespan from ticks
			WSYS_EXP static w_time_span from_ticks(int64_t pValue);
			//Returns timespan from string in the format of "00:00:00:00:000"
			WSYS_EXP static w_time_span from_string(std::string pValue);

#pragma region Getters

            //Returns the overflow status
            WSYS_EXP bool get_has_overflowed() const;
			//Returns total ticks
			WSYS_EXP int64_t get_ticks() const;
			//Returns total days
			WSYS_EXP int get_days() const;
			//Returns total hours
			WSYS_EXP int get_hours() const;
			//Returns total milliseconds
			WSYS_EXP int get_milliseconds() const;
			//Returns total minutes
			WSYS_EXP int get_minutes() const;
			//Returns total seconds
			WSYS_EXP int get_seconds() const;
			//Returns total days
			WSYS_EXP double get_total_days() const;
			//Returns total hours
			WSYS_EXP double get_total_hours() const;
			//Returns total milliseconds
			WSYS_EXP double get_total_milliseconds() const;
			//Returns total minutes
			WSYS_EXP double get_total_minutes() const;
			//Returns total seconds
			WSYS_EXP double get_total_seconds() const;

#pragma endregion

            w_time_span& operator -= (const w_time_span& pRight)
            {
                auto t1 = this->get_ticks();
                auto t2 = pRight.get_ticks();
                auto result =  t1 - t2;
                // Overflow if signs of operands was different and result's
                // sign was opposite from the first argument's sign.
                // >> 63 gives the sign bit (either 64 1's or 64 0's).
                if ((t1 >> 63 != t2 >> 63) && (t1 >> 63 != result >> 63))
                {
                    //overflow TimeSpan too long
                    this->_overflowed = true;
                }
                else
                {
                    this->_ticks = result;
                }
                return *this;
            }
            
            friend w_time_span operator - (w_time_span& pLeft, const w_time_span& pRight)
            {
                auto _t = pLeft;
                _t-= pRight;
                return _t;
            }
            
            friend w_time_span operator + (w_time_span& pLeft, w_time_span& pRight)
            {
                return pLeft.add(pRight);
            }
            
            w_time_span& operator += (const w_time_span& pRight)
            {
                this->add(pRight);
                return *this;
            }
            
            friend bool operator < (const w_time_span& pLeft, const w_time_span& pRight)
            {
                return pLeft.get_ticks() < pRight.get_ticks();
            }
            
            bool operator <= (const w_time_span& pRight)
            {
                return this->get_ticks() <= pRight.get_ticks();
            }
            
            friend bool operator > (const w_time_span& pLeft, const w_time_span& pRight)
            {
                return pLeft.get_ticks() > pRight.get_ticks();
            }
            
            bool operator >= (const w_time_span& pRight)
            {
                return this->get_ticks() >= pRight.get_ticks();
            }
            
            friend bool operator == (const w_time_span& pLeft, const w_time_span& pRight)
            {
                return pLeft.get_ticks() == pRight.get_ticks();
            }
            
            friend bool operator != (const w_time_span& pLeft, const w_time_span& pRight)
            {
                return !(pLeft == pRight);
            }
            
#ifdef __PYTHON__
            void py_set_ticks(int64_t pTicks) { this->_ticks = pTicks; }
            void py_set_hours_min_secs(int pHours, int pMinutes, int pSeconds)
            {
                this->_ticks = _time_to_ticks(pHours, pMinutes, pSeconds);
            }
            void py_set_days_hours_mins_secs_millisecs(int pDays, int pHours, int pMinutes, int pSeconds, int pMilliseconds)
            {
                w_time_span _t_s(pDays, pHours, pMinutes, pSeconds, pMilliseconds);
                this->_ticks = _t_s.get_ticks();
            }
#endif

		private:
			static w_time_span _interval(double pValue, int pScale);
			static int64_t _time_to_ticks(int pHour, int pMinute, int pSecond);
            
            bool    _overflowed;
			int64_t   _ticks;
		};
	}
}

#include "python_exporter/py_time_span.h"

#endif //__W_TIME_SPAN_H__
