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

w_timer* w_timer_init(_In_ const double pStartAfterSec,
                  _In_ const double pTimeOutInSec,
                  _In_ w_timer_callback pCallBack)
{
    struct ev_loop* _loop = EV_DEFAULT;
    
    w_timer* _timer = (w_timer*)w_alloc(sizeof(w_timer));
    ev_timer_init (_timer, pCallBack, pStartAfterSec, pTimeOutInSec);
    ev_timer_start (_loop, _timer);
    
    ev_run (_loop, 0);
    
    return _timer;
}

void w_timer_restart(_In_ w_timer* pTimer)
{
    struct ev_loop* _loop = EV_DEFAULT;
    ev_timer_again(_loop, pTimer);
}

w_timer_periodic* w_timer_init_periodic(_In_ const double pStartAfterSec,
                                        _In_ const double pIntervalInSec,
                                        _In_ w_timer_periodic_callback pCallBack,
                                        _In_ w_timer_periodic_scheduler_callback pSchedulerCallBack)
{
    struct ev_loop* _loop = EV_DEFAULT;
    
    w_timer_periodic* _timer = (w_timer_periodic*)w_alloc(sizeof(w_timer_periodic));
    ev_periodic_init (_timer, pCallBack, pStartAfterSec, pIntervalInSec, pSchedulerCallBack);
    ev_periodic_start (_loop, _timer);
    
    return _timer;
}
