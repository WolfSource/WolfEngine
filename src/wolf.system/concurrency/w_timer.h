///*
//    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
//    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
//    Website          : https://WolfEngine.App
//    Name             : w_timer.h
//    Description      : a timer based on libev
//    Comment          : https://metacpan.org/pod/distribution/EV/libev/ev.pod
//*/
//
//#pragma once
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#include "wolf.h"
//#include <concurrency/w_thread.h>
//#include "libev/ev.h"
//
//    typedef ev_timer w_timer_base;
//    typedef ev_periodic w_timer_base_periodic;
//    typedef struct ev_loop w_timer_loop;
//    typedef void w_timer_callback(w_timer_loop*, w_timer_base*, int /*revents*/);
//    typedef void w_timer_periodic_callback(w_timer_loop*, w_timer_base_periodic*, int /*revents*/);
//    typedef double w_timer_periodic_scheduler_callback(w_timer_base_periodic*, double /*now*/);
//
//    typedef struct w_timer_t
//    {
//        w_timer_base* ti;
//        w_timer_loop* l;
//        w_thread t;//will be used for async mode
//    } w_timer_t;
//    typedef w_timer_t* w_timer;
//
//    typedef struct w_timer_arg_t
//    {
//        w_timer t;
//        void* d;
//    } w_timer_arg_t;
//    typedef w_timer_arg_t* w_timer_arg;
//
//    //typedef struct
//    //{
//    //    w_timer_base_periodic*      t;
//    //    w_timer_loop*               l;
//    //} w_timer_periodic;
//    //typedef w_timer_periodic* w_timer_periodic_ptr;
//
//    /**
//     * create a timer
//     * @param pMemPool the pool from which to allocate the once flag
//     * @param pTimer pointer to timer
//     * @param pTimeOutInSec time out in seconds
//     * @param pRepeatTimes repeat times
//     * @param pIsAsync is async
//     * @param pCallBack callback function
//     * @param pUserData custome user data
//     * @return result
//    */
//    W_SYSTEM_EXPORT
//        W_RESULT w_timer_init(
//            _Inout_ w_mem_pool pMemPool,
//            _Inout_ w_timer* pTimer,
//            _In_ double pTimeOutInSec,
//            _In_ double pRepeatTimes,
//            _In_ int pIsAsync,
//            _In_ w_timer_callback pCallBack,
//            _In_opt_ void* pUserData);
//
//    /**
//     * restart a timer
//     * @param pTimer pointer to a timer
//    */
//    W_SYSTEM_EXPORT
//        W_RESULT w_timer_restart(_In_ w_timer pTimer);
//
//    /**
//     * stop a timer
//     * @param pTimer pointer to a timer
//     * <PRE>
//     *   0  undo unloop
//     *   1  unloop once
//     *   2  unloop all loops
//     *</PRE>
//    */
//    W_SYSTEM_EXPORT
//        W_RESULT w_timer_break(_In_ w_timer pTimer, _In_ uint8_t pHow);
//
//    ///**
//    // * create a periodic timer
//    // * @param pStartAfterSec time before start in seconds
//    // * @param pIntervalInSec interval in seconds
//    // * @param pCallBack callback function
//    // * @param pSchedulerCallBack scheduler callback
//    // * @return pointer to timer
//    //*/
//    //W_SYSTEM_EXPORT
//    //w_timer_periodic_ptr w_timer_init_periodic(
//    //    _In_ double pStartAfterSec,
//    //    _In_ double pIntervalInSec,
//    //    _In_ w_timer_periodic_callback pCallBack,
//    //    _In_ w_timer_periodic_scheduler_callback pSchedulerCallBack);
//
//    /**
//     * release a timer
//     * @param pTimer timer to release
//    */
//    W_SYSTEM_EXPORT
//        void w_timer_fini(_In_ w_timer pTimer);
//
//    /**
//     * release a timer periodic
//     * @param pTimer timer periodic to release
//    */
//    //W_SYSTEM_EXPORT
//    //void w_timer_periodic_terminate(_In_ w_timer_periodic_ptr pTimer);
//
//#ifdef __cplusplus
//}
//#endif
