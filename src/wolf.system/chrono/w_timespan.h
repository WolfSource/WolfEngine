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
	} w_timespan_t;
	typedef w_timespan_t* w_timespan;

	/**
	 * Initialize timespan with zero.
	 * @return a pointer to w_timespan object
	 */
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_zero(_Inout_ w_mem_pool pMemPool);

	/**
	 * Initialize timespan with min value.
	 * @return a pointer to w_timespan object
	 */
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_min_value(_Inout_ w_mem_pool pMemPool);

	/**
	 * Initialize timespan with max value.
	 * @return a pointer to w_timespan object
	 */
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_max_value(_Inout_ w_mem_pool pMemPool);

	/**
	 * Initialize timespan with current now
	 * @return a pointer to w_timespan object
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_now(_Inout_ w_mem_pool pMemPool);

	/**
	 * Returns timespan from days
	 * @param pValue is days
	 * @return a pointer to w_timespan object
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_days(
			_Inout_ w_mem_pool pMemPool,
			_In_ double pValue);

	/**
	 * Returns timespan from hours
	 * @param pValue is hours
	 * @return a pointer to w_timespan object
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_hours(
			_Inout_ w_mem_pool pMemPool,
			_In_ double pValue);

	/**
	 * Returns timespan from minutes
	 * @param pValue is minutes
	 * @return a pointer to w_timespan object
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_minutes(
			_Inout_ w_mem_pool pMemPool,
			_In_ double pValue);

	/**
	 * Returns timespan from seconds
	 * @param pValue is seconds
	 * @return a pointer to w_timespan object
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_seconds(
			_Inout_ w_mem_pool pMemPool,
			_In_ double pValue);

	/**
	 * Returns timespan from milliseconds
	 * @param pValue is milliseconds
	 * @return a pointer to w_timespan object
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_milliseconds(
			_Inout_ w_mem_pool pMemPool,
			_In_ double pValue);

	/**
	 * Returns timespan from const char*
	 * @param pValue is const char* with the following format "00:00:00:00:000"
	 * @return a pointer to w_timespan object
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_string(
			_Inout_ w_mem_pool pMemPool,
			_In_z_ const char* const pValue);

	/**
	 * Initialize timespan with ticks
	 * @param pValue is ticks
	 * @return a pointer to w_timespan object
	 */
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_ticks(
			_Inout_ w_mem_pool pMemPool,
			_In_ int64_t pValue);

	/**
	 * Initialize timespan with short time (hours, minutes, seconds)
	 * @param pHours is number of hours
	 * @param pMinutes is number of minutes
	 * @param pSeconds is number of seconds
	 * @return a pointer to w_timespan object
	 */
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_shorttime(
			_Inout_ w_mem_pool pMemPool,
			_In_ int pHours,
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
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_longtime(
			_Inout_ w_mem_pool pMemPool,
			_In_ int pDays,
			_In_ int pHours,
			_In_ int pMinutes,
			_In_ int pSeconds,
			_In_ int pMilliseconds);

	/**
	 * Initialize timespan from string (in the following format "00:00:00:00:000")
	 * @param pValue is number of days
	 * @return a pointer to w_timespan object
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_string(
			_Inout_ w_mem_pool pMemPool,
			_In_z_ const char* pValue);


	/**
	 * Initialize timespan from string (in the following format L"00:00:00:00:000")
	 * @param pValue is number of days
	 * @return a pointer to w_timespan object
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_init_from_wstring(
			_Inout_ w_mem_pool pMemPool,
			_In_z_ const wchar_t* pValue);

	/**
	 * Add two timespans with each other
	 * @param pLValue left parameter
	 * @param pRValue right parameter which will be added to left parameter
	 * @return result of pLValue + pRValue
	*/
	W_SYSTEM_EXPORT
		w_timespan w_timespan_add(
			_Inout_ w_mem_pool pMemPool,
			_Inout_ w_timespan pLValue,
			_In_ const w_timespan pRValue);

	/**
	 * Add two timespans with each other (pLValue += pRValue)
	 * @param pLValue left parameter
	 * @param pRValue right parameter which will be added to left parameter
	*/
	W_SYSTEM_EXPORT
		void w_timespan_add_by_ref(_Inout_ w_timespan pLValue, _In_ const w_timespan pRValue);


	/**
	 * convert timespan to characters
	 * @param pValue timespan parameter
	 * @param pSplitCharacter split character for example ":"
	 * @return string as const char*
	*/
	W_SYSTEM_EXPORT
		const char* w_timespan_to_string(
			_Inout_ w_mem_pool pMemPool,
			_In_ const w_timespan pValue,
			_In_z_ const char* pSplitCharacter);

	/**
	 * convert timespan to wide characters
	 * @param pValue timespan parameter
	 * @param pSplitCharacter split character for example L":"
	 * @return string as const char*
	*/
	W_SYSTEM_EXPORT
		const wchar_t* w_timespan_to_wstring(
			_Inout_ w_mem_pool pMemPool,
			_In_ const w_timespan pValue,
			_In_z_ const wchar_t* pSplitCharacter);


#pragma region Getters

	/**
	 * get days of timespan
	 * @param pValue timespan parameter
	 * @return days as a constant int
	*/
	W_SYSTEM_EXPORT
		int w_timespan_get_days(_In_ const w_timespan pValue);

	/**
	 * get total days of timespan
	 * @param pValue timespan parameter
	 * @return total days as a constant double
	*/
	W_SYSTEM_EXPORT
		double w_timespan_get_total_days(_In_ const w_timespan pValue);

	/**
	 * get hours of timespan
	 * @param pValue timespan parameter
	 * @return hours as a constant int
	*/
	W_SYSTEM_EXPORT
		int w_timespan_get_hours(_In_ const w_timespan pValue);

	/**
	 * get total hours of timespan
	 * @param pValue timespan parameter
	 * @return total hours as a constant double
	*/
	W_SYSTEM_EXPORT
		double w_timespan_get_total_hours(_In_ const w_timespan pValue);

	/**
	 * get minutes of timespan
	 * @param pValue timespan parameter
	 * @return minutes as a constant int
	*/
	W_SYSTEM_EXPORT
		int w_timespan_get_minutes(_In_ const w_timespan pValue);

	/**
	 * get total minutes of timespan
	 * @param pValue timespan parameter
	 * @return total minutes as a constant double
	*/
	W_SYSTEM_EXPORT
		double w_timespan_get_total_minutes(_In_ const w_timespan pValue);

	/**
	 * get seconds of timespan
	 * @param pValue timespan parameter
	 * @return seconds as a constant int
	*/
	W_SYSTEM_EXPORT
		int w_timespan_get_seconds(_In_ const w_timespan pValue);

	/**
	 * get total seconds of timespan
	 * @param pValue timespan parameter
	 * @return total seconds as a constant double
	*/
	W_SYSTEM_EXPORT
		double w_timespan_get_total_seconds(_In_ const w_timespan pValue);

	/**
	 * get milliseconds of timespan
	 * @param pValue timespan parameter
	 * @return milliseconds as a constant int
	*/
	W_SYSTEM_EXPORT
		int w_timespan_get_milliseconds(_In_ const w_timespan pValue);

	/**
	 * get total milliseconds of timespan
	 * @param pValue timespan parameter
	 * @return total milliseconds as a constant double
	*/
	W_SYSTEM_EXPORT
		double w_timespan_get_total_milliseconds(_In_ const w_timespan pValue);

	/**
	 * get microseconds of timespan
	 * @param pValue timespan parameter
	 * @return microseconds as a constant int
	*/
	W_SYSTEM_EXPORT
		int w_timespan_get_microseconds(_In_ const w_timespan pValue);

	/**
	 * get total microseconds of timespan
	 * @param pValue timespan parameter
	 * @return total microseconds as a constant double
	*/
	W_SYSTEM_EXPORT
		double w_timespan_get_total_microseconds(_In_ const w_timespan pValue);

	/**
	 * get total nanoseconds of timespan
	 * @param pValue timespan parameter
	 * @return total nanoseconds as a constant double
	*/
	W_SYSTEM_EXPORT
		double w_timespan_get_total_nanoseconds(_In_ const w_timespan pValue);

	/**
	 * get current date time in chars
	 * @return current time in the following Day-Month-Year Hour:Min:Sec
	 */
	W_SYSTEM_EXPORT
		const char* w_timespan_get_current_date_time_string(_Inout_ w_mem_pool pMemPool);

	/**
	 * get current date time in wide chars
	 * @return current time in the following Day-Month-Year Hour:Min:Sec
	*/
	W_SYSTEM_EXPORT
		const wchar_t* w_timespan_get_current_date_time_wstring(_Inout_ w_mem_pool pMemPool);

#pragma endregion

#ifdef __cplusplus
}
#endif
