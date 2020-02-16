/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_timer.h
    Description      : a ev timer
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include "libev/ev.h"

typedef ev_timer w_timer;
typedef ev_periodic w_timer_periodic;
typedef struct ev_loop w_timer_loop;
typedef void w_timer_callback(w_timer_loop*, w_timer*, int /*revents*/);
typedef void w_timer_periodic_callback(w_timer_loop*, w_timer_periodic*, int /*revents*/);
typedef double w_timer_periodic_scheduler_callback(w_timer_periodic*, double /*now*/);

/**
 * create a timer
 * @param pStartAfterSec time before start in seconds
 * @param pTimeOutInSec timeout in seconds
 * @param pCallBack callback function
 * @return pointer to timer
*/
w_timer* w_timer_init(_In_ const double pStartAfterSec,
                      _In_ const double pTimeOutInSec,
                      _In_ w_timer_callback pCallBack);

/**
 * restart a timer
 * @param pTimer pointer to timer
*/
void w_timer_restart(_In_ w_timer* pTimer);

/**
 * create a periodic timer
 * @param pStartAfterSec time before start in seconds
 * @param pIntervalInSec interval in seconds
 * @param pCallBack callback function
 * @param pSchedulerCallBack scheduler callback
 * @return pointer to timer
*/
w_timer_periodic* w_timer_init_periodic(_In_ const double pStartAfterSec,
                                        _In_ const double pIntervalInSec,
                                        _In_ w_timer_periodic_callback pCallBack,
                                        _In_ w_timer_periodic_scheduler_callback pSchedulerCallBack);

#ifdef __cplusplus
}
#endif
