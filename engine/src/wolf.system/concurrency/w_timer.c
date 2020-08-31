#include "w_timer.h"

//static void timeout_cb (struct ev_loop *loop, ev_timer *w, int revents)
//{
//
//}

//static void clock_cb (struct ev_loop *loop, ev_periodic *w, int revents)
//{
//  ... its now a full hour (UTC, or TAI or whatever your clock follows)
//}

//static ev_tstamp my_scheduler_cb (ev_periodic *w, ev_tstamp now)
//{
//  return now + (3600. - fmod (now, 3600.));
//}

W_RESULT w_timer_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_timer_ptr pTimer,
    _In_ double pStartAfterSec,
    _In_ double pTimeOutInSec,
    _In_ w_timer_callback pCallBack)
{
    const char* _trace_info = "w_timer_init";
    if (!pMemPool || !pTimer)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return W_FAILURE;
    }
    
    pTimer->t = (w_timer_base*)w_malloc(pMemPool, sizeof(w_timer_base));
    if (!pTimer->t)
    {
        W_ASSERT_P(false, "bad args. trace info: %s", _trace_info);
        return W_FAILURE;
    }
    pTimer->l = EV_DEFAULT;
    
    //init, start ev_timer and run the loop
    ev_timer_init (pTimer->t, pCallBack, pStartAfterSec, pTimeOutInSec);
    ev_timer_start (pTimer->l, pTimer->t);
    pTimer->t->data = pTimer;

    ev_run(pTimer->l, 0);
    
    return W_SUCCESS;
}

W_RESULT w_timer_restart(_In_ w_timer_ptr pTimer)
{
    if (!pTimer)
    {
        return W_FAILURE;
    }
    ev_timer_again(pTimer->l, pTimer->t);
    return W_SUCCESS;
}

W_RESULT w_timer_break(_In_ w_timer_ptr pTimer, _In_ uint8_t pHow)
{
    if (!pTimer || pHow > 2)
    {
        return W_FAILURE;
    }

    ev_break(pTimer->l, pHow);

    return W_SUCCESS;
}

//w_timer_periodic_ptr w_timer_init_periodic(
//    _In_ double pStartAfterSec,
//    _In_ double pIntervalInSec,
//    _In_ w_timer_periodic_callback pCallBack,
//    _In_ w_timer_periodic_scheduler_callback pSchedulerCallBack)
//{
//    //allocate memory from pool
//    w_timer_periodic* _timer = (w_timer_periodic*)w_malloc(sizeof(w_timer_periodic), "w_timer_init_periodic::w_timer_periodic");
//    if (!_timer)
//    {
//        return NULL;
//    }
//    _timer->t = (w_timer_base_periodic*)w_malloc(sizeof(w_timer_base_periodic), "w_timer_init_periodic::w_timer_base_periodic");
//    if (!_timer->t)
//    {
//        w_free(_timer);
//        return NULL;
//    }
//    _timer->l = EV_DEFAULT;
//    
//    ev_periodic_init (_timer->t, pCallBack, pStartAfterSec, pIntervalInSec, pSchedulerCallBack);
//    ev_periodic_start (_timer->l, _timer->t);
//    
//    return _timer;
//}

void w_timer_fini(_In_ w_timer_ptr pTimer)
{
    ev_loop_destroy(pTimer->l);
}

//void w_timer_periodic_terminate(_In_ w_timer_periodic_ptr pTimer)
//{
//    ev_loop_destroy(pTimer->l);
//}

