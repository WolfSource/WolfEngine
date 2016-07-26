/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_time_span.h
	Description		 : time span represents a duration of time, same as TimeSpan of .NetFrameWork
	Comment			 :
*/

#ifndef __W_TIMESPAN_H__
#define __W_TIMESPAN_H__

#include "w_object.h"
#include "w_system_dll.h"
#include <limits>

namespace wolf
{
	namespace system
	{
		class w_time_span
		{
		public:
			SYS_EXP w_time_span();
			SYS_EXP w_time_span(INT64 pTicks);
			SYS_EXP w_time_span(int pHours, int pMinutes, int pSeconds);
			SYS_EXP w_time_span(int pDays, int pHours, int pMinutes, int pSeconds, int pMilliseconds = 0);
			SYS_EXP ~w_time_span();

			//Add two timespans with each other
			SYS_EXP w_time_span add(w_time_span pTS);
			//Return duration in timespan
			SYS_EXP w_time_span duration();
			//Make timespan negative
			SYS_EXP w_time_span negate();
			//Returns time in string
			SYS_EXP std::string to_string();
			//Returns time in wstring
			SYS_EXP std::wstring to_wstring();

			//Returns timespan with zero value
			SYS_EXP static w_time_span zero();
			//Create timespan from now
			SYS_EXP static w_time_span now();
			//Returns timespan with minimum value
			SYS_EXP static w_time_span min_value();
			//Returns timespan with maximum value
			SYS_EXP static w_time_span max_value();
			//Create timeSpan from days
			SYS_EXP static w_time_span from_days(double pValue);
			//Create timeSpan from hours
			SYS_EXP static w_time_span from_hours(double pValue);
			//Create timeSpan from milliSeconds
			SYS_EXP static w_time_span from_milliseconds(double pValue);
			//Create timeSpan from minutes
			SYS_EXP static w_time_span from_minutes(double pValue);
			//Create timespan from seconds
			SYS_EXP static w_time_span from_seconds(double pValue);
			//Create timespan from ticks
			SYS_EXP static w_time_span from_ticks(INT64 pValue);
			//Create timespan from string in the format of "00:00:00:00:000"
			SYS_EXP static w_time_span from_string(std::string pValue);

#pragma region Getters

			//Returns total ticks
			SYS_EXP INT64 get_ticks() const;
			//Returns days
			SYS_EXP int get_days() const;
			//Returns hours
			SYS_EXP int get_hours() const;
			//Returns milliseconds
			SYS_EXP int get_milliseconds() const;
			//Returns minutes
			SYS_EXP int get_minutes() const;
			//Returns seconds
			SYS_EXP int get_seconds() const;
			//Returns total days
			SYS_EXP double get_total_days() const;
			//Returns total hours
			SYS_EXP double get_total_hours() const;
			//Returns total milliseconds
			SYS_EXP double get_total_milliseconds() const;
			//Returns total minutes
			SYS_EXP double get_total_minutes() const;
			//Returns total seconds
			SYS_EXP double get_total_seconds() const;

#pragma endregion

		private:
			static w_time_span _interval(double pValue, int pScale);
			static INT64 _time_to_ticks(int pHour, int pMinute, int pSecond);
		
			INT64 _ticks;
		};

		inline bool operator == (const w_time_span& pLeft, const w_time_span& pRight)
		{
			return pLeft.get_ticks() == pRight.get_ticks();
		}
		
		inline bool operator != (const w_time_span& pLeft, const w_time_span& pRight)
		{
			return !(pLeft == pRight);
		}

		inline w_time_span& operator - (w_time_span& pLeft, w_time_span& pRight)
		{
			auto t1 = pLeft.get_ticks();
			auto t2 = pRight.get_ticks();
			auto result =  t1 - t2;
			// Overflow if signs of operands was different and result's
			// sign was opposite from the first argument's sign.
			// >> 63 gives the sign bit (either 64 1's or 64 0's).
			if ((t1 >> 63 != t2 >> 63) && (t1 >> 63 != result >> 63))
			{
				throw std::exception("Overflow TimeSpan too long");
			}
			return w_time_span(result);
		}

		inline w_time_span& operator -= (w_time_span& pLeft, w_time_span& pRight)
		{
			pLeft = pLeft - pRight;
			return pLeft;
		}

		inline w_time_span& operator + (w_time_span& pLeft, w_time_span& pRight)
		{
			return pLeft.add(pRight);
		}

		inline w_time_span& operator += (w_time_span& pLeft, w_time_span& pRight)
		{
			pLeft = pLeft + pRight;
			return pLeft;
		}

		inline bool operator < (w_time_span& pLeft, w_time_span& pRight)
		{
			return pLeft.get_ticks() < pRight.get_ticks();
		}

		inline bool operator <= (w_time_span& pLeft, w_time_span& pRight)
		{
			return pLeft.get_ticks() <= pRight.get_ticks();
		}

		inline bool operator > (w_time_span& pLeft, w_time_span& pRight)
		{
			return pLeft.get_ticks() > pRight.get_ticks();
		}

		inline bool operator >= (w_time_span& pLeft, w_time_span& pRight)
		{
			return pLeft.get_ticks() >= pRight.get_ticks();
		}
	}
}

#endif