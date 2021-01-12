//#include "w_timer.h"
//#include "log/w_log.h"
//
////static void timeout_cb (struct ev_loop *loop, ev_timer *w, int revents)
////{
////
////}
//
////static void clock_cb (struct ev_loop *loop, ev_periodic *w, int revents)
////{
////  ... its now a full hour (UTC, or TAI or whatever your clock follows)
////}
//
////static ev_tstamp my_scheduler_cb (ev_periodic *w, ev_tstamp now)
////{
////  return now + (3600. - fmod (now, 3600.));
////}
//
//typedef struct ev_params_t
//{
//    w_timer timer;
//    w_timer_arg arg;
//    double timeout_in_sec;
//    double repeat_times;
//    w_timer_callback* callback;
//    void* pUserData;
//} ev_params_t;
//typedef ev_params_t* ev_params;
//
//static W_RESULT s_ev_run(_In_ ev_params pParams)
//{
//    if (!pParams)
//    {
//        return W_FAILURE;
//    }
//
//    pParams->timer->l = ev_loop_new(0);// EV_DEFAULT;
//
//    //init, start ev_timer and run the loop
//    ev_timer_init(
//        pParams->timer->ti,
//        *pParams->callback,
//        pParams->timeout_in_sec,
//        pParams->repeat_times);
//
//    //store timer and user data into the arg
//    pParams->arg->t = pParams->timer;
//    pParams->arg->d = pParams->pUserData;
//    pParams->timer->ti->data = (void*)pParams->arg;
//
//    ev_timer_start(pParams->timer->l, pParams->timer->ti);
//    //run and block the current thread
//    ev_run(pParams->timer->l, 0);
//
//    return W_SUCCESS;
//}
//
//static void* s_w_thread_job(_In_ w_thread pThread, void* pArg)
//{
//    s_ev_run((ev_params)pArg);
//    return NULL;
//}
//
//W_RESULT w_timer_init(
//    _Inout_ w_mem_pool pMemPool,
//    _Inout_ w_timer* pTimer,
//    _In_ double pTimeOutInSec,
//    _In_ double pRepeatTimes,
//    _In_ int pIsAsync,
//    _In_ w_timer_callback pCallBack,
//    _In_opt_ void* pUserData)
//{
//    const char* _trace_info = "w_timer_init";
//    if (!pMemPool || !pTimer)
//    {
//        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
//        return W_BAD_ARG;
//    }
//
//    ev_params _ev_param = (ev_params)w_malloc(pMemPool, sizeof(ev_params_t));
//    if (!_ev_param)
//    {
//        W_ASSERT_P(false, "could not allocate memory for ev params. trace info: %s", _trace_info);
//        return W_FAILURE;
//    }
//
//    _ev_param->timer = (w_timer)w_malloc(pMemPool, sizeof(w_timer_t));
//    if (!_ev_param->timer)
//    {
//        W_ASSERT_P(false, "could not allocate memory for timer. trace info: %s", _trace_info);
//        return W_FAILURE;
//    }
//
//    _ev_param->timer->ti = (w_timer_base*)w_malloc(pMemPool, sizeof(w_timer_base));
//    if (!_ev_param->timer->ti)
//    {
//        W_ASSERT_P(false, "could not allocate memory for timer->t. trace info: %s", _trace_info);
//        return W_FAILURE;
//    }
//
//    _ev_param->arg = (w_timer_arg)w_malloc(pMemPool, sizeof(w_timer_arg_t));
//    if (!_ev_param->arg)
//    {
//        W_ASSERT_P(false, "could not allocate memory for timer arg. trace info: %s", _trace_info);
//        return W_FAILURE;
//    }
//
//    _ev_param->callback = &pCallBack;
//    _ev_param->pUserData = pUserData;
//    _ev_param->repeat_times = pRepeatTimes;
//    _ev_param->timeout_in_sec = pTimeOutInSec;
//
//    if (pIsAsync)
//    {
//        *pTimer = _ev_param->timer;
//        return w_thread_init(
//            pMemPool,
//            &_ev_param->timer->t,
//            s_w_thread_job,
//            (void*)_ev_param);
//    }
//
//    return s_ev_run(_ev_param);
//}
//
//W_RESULT w_timer_restart(_In_ w_timer pTimer)
//{
//    if (!pTimer)
//    {
//        return W_FAILURE;
//    }
//    ev_timer_again(pTimer->l, pTimer->ti);
//    return W_SUCCESS;
//}
//
//W_RESULT w_timer_break(_In_ w_timer pTimer, _In_ uint8_t pHow)
//{
//    if (!pTimer || pHow > 2)
//    {
//        return W_FAILURE;
//    }
//
//    ev_break(pTimer->l, pHow);
//
//    return W_SUCCESS;
//}
//
////w_timer_periodic_ptr w_timer_init_periodic(
////    _In_ double pStartAfterSec,
////    _In_ double pIntervalInSec,
////    _In_ w_timer_periodic_callback pCallBack,
////    _In_ w_timer_periodic_scheduler_callback pSchedulerCallBack)
////{
////    //allocate memory from pool
////    w_timer_periodic* _timer = (w_timer_periodic*)w_malloc(sizeof(w_timer_periodic), "w_timer_init_periodic::w_timer_periodic");
////    if (!_timer)
////    {
////        return NULL;
////    }
////    _timer->t = (w_timer_base_periodic*)w_malloc(sizeof(w_timer_base_periodic), "w_timer_init_periodic::w_timer_base_periodic");
////    if (!_timer->t)
////    {
////        w_free(_timer);
////        return NULL;
////    }
////    _timer->l = EV_DEFAULT;
////    
////    ev_periodic_init (_timer->t, pCallBack, pStartAfterSec, pIntervalInSec, pSchedulerCallBack);
////    ev_periodic_start (_timer->l, _timer->t);
////    
////    return _timer;
////}
//
//void w_timer_fini(_In_ w_timer pTimer)
//{
//    ev_loop_destroy(pTimer->l);
//}
//
////void w_timer_periodic_terminate(_In_ w_timer_periodic_ptr pTimer)
////{
////    ev_loop_destroy(pTimer->l);
////}
//
