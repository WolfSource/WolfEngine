/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_timer.h
	Description		 : A timer with/without callback
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "w_timespan.h"

/**
 * execute job synchronously with callback
 * @param pInterval the interval time of timer
 * @param pCallBack callback function
*/
void timer_sync_with_callback(_In_ const w_timespan* const pInterval,
                                           _In_ w_job pCallBack,
                                           _In_ void* pArg);

/**
 * execute job asynchronously with callback
 * @param pInterval the interval time of timer
 * @param pCallBack callback function
*/
void timer_async_with_callback(_In_ const w_timespan* const pInterval,
                                            _In_ w_job pCallBack,
                                            _In_ void*  pArg);

#ifdef __cplusplus
}
#endif
