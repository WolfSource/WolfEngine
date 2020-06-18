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

w_timer* w_timer_init(_In_ double pStartAfterSec,
                      _In_ double pTimeOutInSec,
                      _In_ w_timer_callback pCallBack)
{
    //allocate memory from pool
    w_timer* _timer = (w_timer*)w_malloc(sizeof(w_timer), "w_timer_init::_timer");
    if (!_timer)
    {
        return NULL;
    }
    _timer->t = (w_timer_base*)w_malloc(sizeof(w_timer_base), "w_timer_init::w_timer_base");
    if (!_timer->t)
    {
        w_free(_timer);
        return NULL;
    }
    _timer->l = EV_DEFAULT;
    
    //init, start ev_timer and run the loop
    ev_timer_init (_timer->t, pCallBack, pStartAfterSec, pTimeOutInSec);
    ev_timer_start (_timer->l, _timer->t);
    
    ev_run (_timer->l, 0);
    
    return _timer;
}

void w_timer_restart(_In_ w_timer* pTimer)
{
    ev_timer_again(pTimer->l, pTimer->t);
}

w_timer_periodic* w_timer_init_periodic(_In_ double pStartAfterSec,
                                        _In_ double pIntervalInSec,
                                        _In_ w_timer_periodic_callback pCallBack,
                                        _In_ w_timer_periodic_scheduler_callback pSchedulerCallBack)
{
    //allocate memory from pool
    w_timer_periodic* _timer = (w_timer_periodic*)w_malloc(sizeof(w_timer_periodic), "w_timer_init_periodic::w_timer_periodic");
    if (!_timer)
    {
        return NULL;
    }
    _timer->t = (w_timer_base_periodic*)w_malloc(sizeof(w_timer_base_periodic), "w_timer_init_periodic::w_timer_base_periodic");
    if (!_timer->t)
    {
        w_free(_timer);
        return NULL;
    }
    _timer->l = EV_DEFAULT;
    
    ev_periodic_init (_timer->t, pCallBack, pStartAfterSec, pIntervalInSec, pSchedulerCallBack);
    ev_periodic_start (_timer->l, _timer->t);
    
    return _timer;
}

void w_timer_free(_In_ w_timer* pTimer)
{
    ev_loop_destroy(pTimer->l);
}

void w_timer_periodic_free(_In_ w_timer_periodic* pTimer)
{
    ev_loop_destroy(pTimer->l);
}

