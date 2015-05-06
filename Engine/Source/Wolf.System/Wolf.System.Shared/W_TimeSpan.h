/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_TimeSpan.h
	Description		 : TimeSpan represents a duration of time, same as TimeSpan of .NetFrameWork
	Comment			 :
*/

#pragma once

#include "W_Object.h"
#include <limits>

namespace Wolf
{
	namespace System
	{
		class W_TimeSpan
		{
		public:
			API W_TimeSpan();
			API W_TimeSpan(INT64 pTicks);
			API W_TimeSpan(int pHours, int pMinutes, int pSeconds);
			API W_TimeSpan(int pDays, int pHours, int pMinutes, int pSeconds, int pMilliseconds = 0);
			API ~W_TimeSpan();

			//Add two timespans with each other
			API W_TimeSpan Add(W_TimeSpan pTS);
			//Return duration in timespan
			API W_TimeSpan Duration();
			//Make timespan negative
			API W_TimeSpan Negate();
			//Returns time in string
			API std::string ToString();
			//Returns time in wstring
			API std::wstring ToWString();

			//Returns timespan with zero value
			API static W_TimeSpan Zero();
			//Returns timespan with minimum value
			API static W_TimeSpan MinValue();
			//Returns timespan with maximum value
			API static W_TimeSpan MaxValue();
			//Create timeSpan from days
			API static W_TimeSpan FromDays(double pValue);
			//Create timeSpan from hours
			API static W_TimeSpan FromHours(double pValue);
			//Create timeSpan from milliSeconds
			API static W_TimeSpan FromMilliseconds(double pValue);
			//Create timeSpan from minutes
			API static W_TimeSpan FromMinutes(double pValue);
			//Create timespan from seconds
			API static W_TimeSpan FromSeconds(double pValue);
			//Create timespan from ticks
			API static W_TimeSpan FromTicks(INT64 pValue);


#pragma region Getters

			//Returns total ticks
			API INT64 GetTicks() const;
			//Returns days
			API int GetDays() const;
			//Returns hours
			API int GetHours() const;
			//Returns milliseconds
			API int GetMilliseconds() const;
			//Returns minutes
			API int GetMinutes() const;
			//Returns seconds
			API int GetSeconds() const;
			//Returns total days
			API double GetTotalDays() const;
			//Returns total hours
			API double GetTotalHours() const;
			//Returns total milliseconds
			API double GetTotalMilliseconds() const;
			//Returns total minutes
			API double GetTotalMinutes() const;
			//Returns total seconds
			API double GetTotalSeconds() const;

#pragma endregion

		private:
			static W_TimeSpan Interval(double pValue, int pScale);
			static INT64 TimeToTicks(int hour, int minute, int second);
		
			INT64 _ticks;

		};

		inline bool operator == (const W_TimeSpan& lValue, const W_TimeSpan& rValue)
		{
			return lValue.GetTicks() == rValue.GetTicks();
		}
		
		inline bool operator != (const W_TimeSpan& lValue, const W_TimeSpan& rValue)
		{
			return !(lValue == rValue);
		}

		inline W_TimeSpan& operator - (W_TimeSpan& lValue, W_TimeSpan& rValue)
		{
			auto t1 = lValue.GetTicks();
			auto t2 = rValue.GetTicks();
			auto result =  t1 - t2;
			// Overflow if signs of operands was different and result's
			// sign was opposite from the first argument's sign.
			// >> 63 gives the sign bit (either 64 1's or 64 0's).
			if ((t1 >> 63 != t2 >> 63) && (t1 >> 63 != result >> 63))
			{
				throw std::exception("Overflow TimeSpan too long");
			}
			return W_TimeSpan(result);
		}

		inline W_TimeSpan& operator -= (W_TimeSpan& lValue, W_TimeSpan& rValue)
		{
			lValue = lValue - rValue;
			return lValue;
		}

		inline W_TimeSpan& operator + (W_TimeSpan& lValue, W_TimeSpan& rValue)
		{
			return lValue.Add(rValue);
		}

		inline W_TimeSpan& operator += (W_TimeSpan& lValue, W_TimeSpan& rValue)
		{
			lValue = lValue + rValue;
			return lValue;
		}

		inline bool operator < (W_TimeSpan& lValue, W_TimeSpan& rValue)
		{
			return lValue.GetTicks() < rValue.GetTicks();
		}

		inline bool operator <= (W_TimeSpan& lValue, W_TimeSpan& rValue)
		{
			return lValue.GetTicks() <= rValue.GetTicks();
		}

		inline bool operator > (W_TimeSpan& lValue, W_TimeSpan& rValue)
		{
			return lValue.GetTicks() > rValue.GetTicks();
		}

		inline bool operator >= (W_TimeSpan& lValue, W_TimeSpan& rValue)
		{
			return lValue.GetTicks() >= rValue.GetTicks();
		}

	}
}
