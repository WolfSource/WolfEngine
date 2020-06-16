/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_timespan.h
	Description		 : w_timespan represents a duration of time, same as TimeSpan of .NetFrameWork
	Comment			 :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

//#include "python_exporter/w_boost_python_helper.h"

//forward declaration
typedef struct
{
    int64_t                         ticks;
    bool                            overflowed;
} w_timespan;

/**
 * Initialize timespan with zero.
 * @return a pointer to w_timespan object
 */
w_timespan* w_timespan_init_from_zero(void);

/**
 * Initialize timespan with min value.
 * @return a pointer to w_timespan object
 */
w_timespan* w_timespan_init_from_min_value(void);

/**
 * Initialize timespan with max value.
 * @return a pointer to w_timespan object
 */
w_timespan* w_timespan_init_from_max_value(void);

/**
 * Initialize timespan with current now
 * @return a pointer to w_timespan object
*/
w_timespan* w_timespan_init_from_now(void);

/**
 * Returns timespan from days
 * @param pValue is days
 * @return a pointer to w_timespan object
*/
w_timespan* w_timespan_init_from_days(_In_ double pValue);

/**
 * Returns timespan from hours
 * @param pValue is hours
 * @return a pointer to w_timespan object
*/
w_timespan* w_timespan_init_from_hours(_In_ double pValue);

/**
 * Returns timespan from minutes
 * @param pValue is minutes
 * @return a pointer to w_timespan object
*/
w_timespan* w_timespan_init_from_minutes(_In_ double pValue);

/**
 * Returns timespan from seconds
 * @param pValue is seconds
 * @return a pointer to w_timespan object
*/
w_timespan* w_timespan_init_from_seconds(_In_ double pValue);

/**
 * Returns timespan from milliseconds
 * @param pValue is milliseconds
 * @return a pointer to w_timespan object
*/
w_timespan* w_timespan_init_from_milliseconds(_In_ double pValue);

/**
 * Returns timespan from const char*
 * @param pValue is const char* with the following format "00:00:00:00:000"
 * @return a pointer to w_timespan object
*/
w_timespan* w_timespan_init_from_string(_In_z_ const char* const pValue);

/**
 * Initialize timespan with ticks
 * @param pValue is ticks
 * @return a pointer to w_timespan object
 */
w_timespan* w_timespan_init_from_ticks(_In_ int64_t pValue);

/**
 * Initialize timespan with short time (hours, minutes, seconds)
 * @param pHours is number of hours
 * @param pMinutes is number of minutes
 * @param pSeconds is number of seconds
 * @return a pointer to w_timespan object
 */
w_timespan* w_timespan_init_from_shorttime(_In_ int pHours,
                                           _In_ int pMinutes,
                                           _In_ int pSeconds);

/**
 * Initialize timespan with long time (days, hours, minutes, seconds, milliseconds)
 * @param pDays is number of days
 * @param pHours is number of hours
 * @param pMinutes is number of minutes
 * @param pSeconds is number of seconds
 * @param pMilliseconds is number of milliseconds
 * @return a pointer to w_timespan object
 */
w_timespan* w_timespan_init_from_longtime(_In_ int pDays,
                                          _In_ int pHours,
                                          _In_ int pMinutes,
                                          _In_ int pSeconds,
                                          _In_ int pMilliseconds);

/**
 * Initialize timespan from string (in the following format "00:00:00:00:000")
 * @param pValue is number of days
 * @return a pointer to w_timespan object
*/
w_timespan* w_timespan_init_from_string(const char* const pValue);


/**
 * Initialize timespan from string (in the following format L"00:00:00:00:000")
 * @param pValue is number of days
 * @return a pointer to w_timespan object
*/
w_timespan* w_timespan_init_from_wstring(const wchar_t* const pValue);

/**
 * Add two timespans with each other
 * @param pLValue left parameter
 * @param pRValue right parameter which will be added to left parameter
 * @return result of pLValue + pRValue
*/
w_timespan* w_timespan_add(_Inout_ w_timespan* pLValue, _In_ const w_timespan* const pRValue);

/**
 * Add two timespans with each other (pLValue += pRValue)
 * @param pLValue left parameter
 * @param pRValue right parameter which will be added to left parameter
*/
void w_timespan_add_by_ref(_Inout_ w_timespan* pLValue, _In_ const w_timespan* const pRValue);


/**
 * convert timespan to characters
 * @param pValue timespan parameter
 * @return string as const char*
*/
const char* w_timespan_to_string(_In_ const w_timespan* const pValue);

/**
 * convert timespan to wide characters
 * @param pValue timespan parameter
 * @return string as const char*
*/
const wchar_t* w_timespan_to_wstring(_In_ const w_timespan* const pValue);


#pragma region Getters

/**
 * get days of timespan
 * @param pValue timespan parameter
 * @return days as a constant int
*/
int w_timespan_get_days(_In_ const w_timespan* const pValue);

/**
 * get total days of timespan
 * @param pValue timespan parameter
 * @return total days as a constant double
*/
double w_timespan_get_total_days(_In_ const w_timespan* const pValue);

/**
 * get hours of timespan
 * @param pValue timespan parameter
 * @return hours as a constant int
*/
int w_timespan_get_hours(_In_ const w_timespan* const pValue);

/**
 * get total hours of timespan
 * @param pValue timespan parameter
 * @return total hours as a constant double
*/
double w_timespan_get_total_hours(_In_ const w_timespan* const pValue);

/**
 * get minutes of timespan
 * @param pValue timespan parameter
 * @return minutes as a constant int
*/
int w_timespan_get_minutes(_In_ const w_timespan* const pValue);

/**
 * get total minutes of timespan
 * @param pValue timespan parameter
 * @return total minutes as a constant double
*/
double w_timespan_get_total_minutes(_In_ const w_timespan* const pValue);

/**
 * get seconds of timespan
 * @param pValue timespan parameter
 * @return seconds as a constant int
*/
int w_timespan_get_seconds(_In_ const w_timespan* const pValue);

/**
 * get total seconds of timespan
 * @param pValue timespan parameter
 * @return total seconds as a constant double
*/
double w_timespan_get_total_seconds(_In_ const w_timespan* const pValue);

/**
 * get milliseconds of timespan
 * @param pValue timespan parameter
 * @return milliseconds as a constant int
*/
int w_timespan_get_milliseconds(_In_ const w_timespan* const pValue);

/**
 * get total milliseconds of timespan
 * @param pValue timespan parameter
 * @return total milliseconds as a constant double
*/
double w_timespan_get_total_milliseconds(_In_ const w_timespan* const pValue);

/**
 * get current date time in chars
 * @return current time in the following Day-Month-Year Hour:Min:Sec
 */
const char* w_timespan_get_current_date_time_string(void);

/**
 * get current date time in wide chars
 * @return current time in the following Day-Month-Year Hour:Min:Sec
*/
const wchar_t* w_timespan_get_current_date_time_wstring(void);

#pragma endregion

#ifdef __cplusplus
}
#endif

/*

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
#endif*/

	//private:
		//static w_time_span _interval(double pValue, int pScale);
		//static int64_t _time_to_ticks(int pHour, int pMinute, int pSecond);

//#include "python_exporter/py_time_span.h"
