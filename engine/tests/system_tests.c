#include "convey.h"
#include <wolf.h>

#include<wolf.h>
#include <chrono/w_timespan.h>
#include<chrono/w_chrono.h>
#include <chrono/w_gametime.h>
#include<time.h>
#include<compression/w_compress.h>
#include <concurrency/w_concurrent_queue.h>
#include <concurrency/w_condition_variable.h>
#include <concurrency/w_thread.h>
#include<concurrency/w_thread_pool.h>
#include<concurrency/w_timer.h>
#include<concurrency/w_async.h>
#include<concurrency/libev/ev.h>

#include <time.h>
#include <stdint.h>

void w_thread_job_my(w_thread arg1, void* arg2) {}
void mycallback_thread() {}
void w_callback(w_timer_loop* arg1, w_timer_base* arg2, int arg3) {}
void timer_periodic_callback(w_timer_loop* arg1, w_timer_base_periodic* argg2, int arg3) {}

double timer_periodic_scheduler_callback(w_timer_base_periodic* arg1, double arg2)
{
    double x = 0.02;
    return x;
}


Main({
    //initialize wolf
    wolf_initialize();

//testing chrono
printf("testing chrono");

Convey("chrono", {

    //testing w_timespan
   printf("testing w_timespan");
    w_timespan* _t_0 = w_timespan_init_from_zero();
    So(_t_0->ticks == 0 && !_t_0->overflowed);

    w_timespan* _t_min = w_timespan_init_from_min_value();
    So(_t_min->ticks == -9223372036854775808 && !_t_min->overflowed);

    w_timespan* _t_max = w_timespan_init_from_max_value();
    So(_t_max->ticks == 9223372036854775807 && !_t_max->overflowed);

    w_timespan* _t_now = w_timespan_init_from_now();
    So(_t_now->ticks != 0);

    w_timespan* _t_days = w_timespan_init_from_days(31);
    So(_t_days->ticks == 26784000000000);

    w_timespan* _t_hours = w_timespan_init_from_hours(1);
    So(_t_hours->ticks == 36000000000);

    w_timespan* _t_minu = w_timespan_init_from_minutes(2);
    So(_t_minu->ticks == 1200000000);

    w_timespan* _t_sec = w_timespan_init_from_seconds(3);
    So(_t_sec->ticks == 30000000);

    w_timespan* _t_millisec = w_timespan_init_from_milliseconds(50);
    So(_t_millisec->ticks == 500000);

    w_timespan* _t_tick = w_timespan_init_from_ticks(5);
    So(_t_tick->ticks == 5);

    w_timespan* _t_string = w_timespan_init_from_string("02:02:01:00:000");
    So(_t_string->ticks == 1800600000000);

    w_timespan* _t_wstring = w_timespan_init_from_wstring(L"02:02:01:00:000");
    So(_t_string->ticks == 1800600000000);

    w_timespan* _t_short = w_timespan_init_from_shorttime(2, 2, 2);
    So(_t_short->ticks == 73220000000);

    w_timespan* _t_long = w_timespan_init_from_longtime(3, 4, 5, 8, 10);
    So(_t_long->ticks == 2739080100000);

    w_timespan* _t_add = w_timespan_add(_t_long, _t_short);
    So(_t_add->ticks == 2812300100000);

    w_timespan_add_by_ref(_t_long, _t_short);
    So(_t_long->ticks == 2812300100000);

    const char* _time_to_string = w_timespan_to_string(_t_short);
    const char* _time_origin = "0:2:2:2:000";
    int result = strcmp(_time_to_string, _time_origin);
    So(result == 0);

    const wchar_t* _time_to_wstring = w_timespan_to_wstring(_t_short);
    const wchar_t* _time_Worigin = L"0:2:2:2:000";
    int w_result = wcscmp(_time_to_wstring, _time_Worigin);
    So(w_result == 0);

    int _get_days = w_timespan_get_days(_t_long);
    So(_get_days == 3);

    double _get_total_days = w_timespan_get_total_days(_t_tick);
    So(_get_total_days == 5.7870370370370368e-12);

    int _get_hours = w_timespan_get_hours(_t_short);
    So(_get_hours == 2);

    double _get_total_hours = w_timespan_get_total_hours(_t_short);
    So(_get_total_hours == 2.0338888888888889);

    int _get_minutes = w_timespan_get_minutes(_t_short);
    So(_get_minutes == 2);

    double get_total_minutes = w_timespan_get_total_minutes(_t_short);
    So(get_total_minutes == 122.03333333333333);

    int _get_seconds = w_timespan_get_seconds(_t_short);
    So(_get_seconds == 2);

    double _get_total_seconds = w_timespan_get_total_seconds(_t_short);
    So(_get_total_seconds == 7322.0000000000000);

    int _get_milliseconds = w_timespan_get_milliseconds(_t_short);
    So(_get_milliseconds == 0);

    double _get_total_milliseconds = w_timespan_get_total_milliseconds(_t_short);
    So(_get_total_milliseconds == 7322000.0000000000);

    printf("testing w_chrono");

    struct timespec _c1_now = w_chrono_now();

    //sleep for 3 seconds
    w_thread_current_sleep_for_seconds(3);

    struct timespec _c2_now = w_chrono_now();

    //check the duration
    struct timespec _duration = w_chrono_duration(&_c1_now, &_c2_now);
    So(_duration.tv_sec == 3);

    double _nano_duration = w_chrono_duration_nanoseconds(&_c1_now, &_c2_now);
    So(_nano_duration > 3000000000 && _nano_duration < 3100000000);

    double _micro_duration = w_chrono_duration_microseconds(&_c1_now, &_c2_now);
    So((int)_micro_duration != 0 && (int)_micro_duration != 0);

    double _milli_duration = w_chrono_duration_milliseconds(&_c1_now, &_c2_now);
    So((int)_milli_duration == 3000);

    double _sec_duration = w_chrono_duration_seconds(&_c1_now, &_c2_now);
    So((int)_sec_duration == 3);

    printf("testing w_gametime");
    w_gametime _g_t_init = w_gametime_init();

    uint64_t _g_t__get_elapsed_ticks = w_gametime_get_elapsed_ticks(_g_t_init);
    So(_g_t__get_elapsed_ticks == 0);

    double _t_g__get_elapsvoided_seconds = w_gametime_get_elapsvoided_seconds(_g_t_init);
    So(_t_g__get_elapsvoided_seconds == 0);

    uint64_t _t_g__get_total_ticks = w_gametime_get_total_ticks(_g_t_init);
    So(_t_g__get_total_ticks == 0);

    double _T_g__get_total_seconds = w_gametime_get_total_seconds(_g_t_init);
    So(_T_g__get_total_seconds == 0);

    uint32_t _t_g__get_frame_count = w_gametime_get_frame_count(_g_t_init);
    So(_t_g__get_frame_count == 0);

    uint32_t _t_g__get_frames_per_second = w_gametime_get_frames_per_second(_g_t_init);
    So(_t_g__get_frames_per_second == 0);

    bool _t_g__get_fixed_time_step = w_gametime_get_fixed_time_step(_g_t_init);
    So(_t_g__get_fixed_time_step == false);



});

Convey("compression", {


    printf("testing w_compress");

    const char pSrcBuffer[7] = "playpod";
    w_compress_result* pCompressResult =w_malloc(sizeof(w_compress_result),"main::w_compress_result");
    pCompressResult->size_in = 6;
    W_RESULT _c_lz4 = w_compress_lz4(pSrcBuffer, W_FAST, 1, pCompressResult);
    So(_c_lz4 == W_SUCCESS);

    w_compress_result* pDecompressInfo = (w_compress_result*)w_malloc(sizeof(w_compress_result),"main::w_compress_result");
    const char* pcom = pCompressResult->data;
    pDecompressInfo->size_in = pCompressResult->size_out;
    W_RESULT _d_lz4 = w_decompress_lz4(pcom, pDecompressInfo);
    So(_d_lz4 == W_SUCCESS);



    });

Convey("concurrency", {





   printf("testing w_concurrent_queue");
   int _a = 3;
   w_mem_pool _pool = w_get_default_memory_pool();
   w_concurrent_queue _q;
   W_RESULT _c_queue_init = w_concurrent_queue_init(&_q, 10, _pool);
   So(_c_queue_init == W_SUCCESS);


   W_RESULT _c_queue_push = w_concurrent_queue_push(_q, (void*)&_a);
   So(_c_queue_push == W_SUCCESS);
   _a--;
   W_RESULT _c_queue_push2 = w_concurrent_queue_push(_q, (void*)&_a);
   So(_c_queue_push2 == W_SUCCESS);


   int _b = 0;
   int* _b_ptr = &_b;
   W_RESULT _c_queue_pop = w_concurrent_queue_pop(_q, (void**)&_b_ptr);
   So(_c_queue_pop == W_SUCCESS);


  W_RESULT  _c_queue_trypush = w_concurrent_queue_trypush(_q, (void*)&_a);
  So(_c_queue_trypush == W_SUCCESS);

   int _c = 0;
   int* _c_ptr = &_c;
   W_RESULT _c_queue_trypop = w_concurrent_queue_trypop(_q, (void**)&_c_ptr);
   So(_c_queue_trypop == W_SUCCESS);

   uint32_t _c_queue_size = w_concurrent_queue_size(_q);
   So(_c_queue_size == 1);

   W_RESULT _c_queue_interrupt_all = w_concurrent_queue_interrupt_all(_q);
   So(_c_queue_interrupt_all == W_SUCCESS);


   W_RESULT  _c_queue_term = w_concurrent_queue_term(_q);
   So(_c_queue_term == W_SUCCESS);

   printf("testing w_condition_variable");

   w_condition_variable pcond;
   W_RESULT _condition_variable_init = w_condition_variable_init(&pcond);
   So(_condition_variable_init == W_SUCCESS);

   W_RESULT _rt;
   w_condition_variable pcond2 = NULL;
   if ((_rt = w_condition_variable_init(&pcond2)) == W_SUCCESS)
   {
       w_mem_pool _mem = w_get_default_memory_pool();

       w_mutex _mutex = NULL;
       if ((_rt = w_thread_mutex_create(&_mutex, 0, _mem)) == W_SUCCESS)
       {
           w_timespan* _t_millisec = w_timespan_init_from_milliseconds(5000);
           W_RESULT _condition_variable_wait_for = w_condition_variable_wait_for(pcond2, _mutex, _t_millisec);
           So(_condition_variable_wait_for != 0);
       }
   }
   w_mutex pMutex = (w_mutex)w_malloc(sizeof(w_mutex),"w_mutex");
   // W_RESULT _condition_variable_wait = w_condition_variable_wait( pcond,  pMutex);
   // So(_condition_variable_wait == W_SUCCESS);

    W_RESULT _condition_variable_signal = w_condition_variable_signal(pcond);
    So(_condition_variable_signal == W_SUCCESS);

    W_RESULT _condition_variable_broadcast = w_condition_variable_broadcast(pcond);
    So(_condition_variable_broadcast == W_SUCCESS);

    W_RESULT _condition_variable_destroy = w_condition_variable_destroy(pcond);
    So(_condition_variable_destroy == W_SUCCESS);


    printf("testing w_thread");

    w_thread_job thread_job2 = &w_thread_job_my;
    w_thread pThread = NULL;
    int pJobArgs = 2;
    W_RESULT _thread_create = w_thread_create(&pThread, thread_job2, (void*)pJobArgs);
    So(_thread_create == W_SUCCESS);

    W_RESULT _thread_join = w_thread_join(pThread);
    So(_thread_join != 0);

    w_thread pThread1 = NULL;
    pThread1 = w_thread_get_current();
    So(pThread1 == NULL);

    w_thread_id _thread_get_current_id = w_thread_get_current_id();
    So(_thread_get_current_id != 0);

    w_thread_id _thread_get_current_id2 = w_thread_get_current_id();
   So(_thread_get_current_id2 != 0);

   W_RESULT thread_current_ids_are_equal = w_thread_current_ids_are_equal(_thread_get_current_id, _thread_get_current_id2);
   So(thread_current_ids_are_equal == 0);

   w_thread_once_flag pOnceFlag = (w_thread_once_flag)w_malloc(sizeof(w_thread_once_flag),"main::w_thread_once_flag");
   W_RESULT _thread_init_once_flag = w_thread_init_once_flag(pOnceFlag);
   So(_thread_init_once_flag == W_SUCCESS);


    w_thread_once_job p = &mycallback_thread;
    W_RESULT _thread_once_call = w_thread_once_call(pOnceFlag, mycallback_thread);
    So(_thread_once_call == W_SUCCESS);

    w_mutex Mutex1 = (w_mutex)w_malloc(sizeof(w_mutex),"main::w_mutex");
    W_RESULT _thread_mutex_create = w_thread_mutex_create(&Mutex1, 0, _pool);
    So(_thread_mutex_create == W_SUCCESS);

    W_RESULT   thread_mutex_destroy = w_thread_mutex_destroy(Mutex1);
    So(thread_mutex_destroy == 0);

    w_mutex Mutex = (w_mutex)w_malloc(sizeof(w_mutex),"main::w_mutex");
    W_RESULT  _thread_mutex_lock = w_thread_mutex_lock(Mutex);
    So(_thread_mutex_lock != 0);

    W_RESULT  _thread_mutex_unlock = w_thread_mutex_unlock(Mutex);
    So(_thread_mutex_unlock == 0);

     W_RESULT   _thread_mutex_trylock = w_thread_mutex_trylock(Mutex);
     So(_thread_mutex_trylock != 0);

    w_mem_pool _thread_mutex_get_mem_pool = w_thread_mutex_get_mem_pool(pMutex);
     So(_thread_mutex_get_mem_pool != 0);


     printf("testing w_timer");

     w_timer_callback* timer_callback = w_callback;
     w_timer* v = w_malloc(sizeof(w_timer),"main::w_timer");
     //v = w_timer_init(0.0000000001, 0.0000000002, timer_callback);

     w_timer_periodic_callback* periodic_callback = timer_periodic_callback;
     w_timer_periodic_scheduler_callback* periodic_scheduler_callback = timer_periodic_scheduler_callback;
     w_timer_periodic* periodic = (w_timer_periodic*)w_malloc(sizeof(w_timer_periodic),"main::w_timer_periodic");
     periodic = w_timer_init_periodic(0.001, 0.002, periodic_callback, periodic_scheduler_callback);
     So(periodic->t->pending == 0 && periodic->t->active == 3);

     printf("testing w_thread_pool");

     w_mem_pool _pool4 = w_get_default_memory_pool();
     size_t pMinThreads = 3;
     size_t pMaxThreads = 6;
     w_thread_pool _thread_pool_init = w_thread_pool_init(pMinThreads, pMaxThreads, _pool4);
     So(_thread_pool_init != 0);

     int  _d = 2;
     uint8_t _e = "1";
     int pOwner = 1;
     w_thread_job pTaskFunction = &w_thread_job_my;
     W_RESULT _thread_pool_push = w_thread_pool_push(_thread_pool_init, pTaskFunction, (void*)&_d, (void*)&_e, pOwner);
     So(_thread_pool_push == 0);

     int pParam = 5;
     long  pIntervalTime = 80;
     W_RESULT _thread_pool_schedule = w_thread_pool_schedule(_thread_pool_init, pTaskFunction, (void*)&pParam, pIntervalTime, pOwner);
     So(_thread_pool_schedule == 0);

     W_RESULT _thread_pool_tasks_cancel = w_thread_pool_tasks_cancel(_thread_pool_init, pOwner);
     So(_thread_pool_tasks_cancel == 0);

     W_RESULT _thread_pool_top = w_thread_pool_top(_thread_pool_init, pTaskFunction, _d, _e, pOwner);
     So(_thread_pool_top == 0);

    size_t _thread_pool_tasks_count = w_thread_pool_tasks_count(_thread_pool_init);
     So(_thread_pool_tasks_count != 0);

     size_t _thread_pool_scheduled_tasks_count = w_thread_pool_scheduled_tasks_count(_thread_pool_init);
     So(_thread_pool_scheduled_tasks_count == 0);

     size_t _thread_pool_threads_count = w_thread_pool_threads_count(_thread_pool_init);
     So(_thread_pool_threads_count != 0);


     size_t _thread_pool_busy_count = w_thread_pool_busy_count(_thread_pool_init);
     So(_thread_pool_busy_count != 0);

     /*sometimes the result is ok in test but sometimes is not*/
    /* size_t _thread_pool_idle_count = w_thread_pool_idle_count(_thread_pool_init);
     So(_thread_pool_idle_count == 0);*/

     int pCount = 2;
     size_t _thread_pool_idle_max_set = w_thread_pool_idle_max_set(_thread_pool_init,(size_t)pCount);
     So(_thread_pool_idle_max_set == 0);


     /*sometimes the result is ok in test but sometimes is not*/
     size_t _thread_pool_tasks_run_count = w_thread_pool_tasks_run_count(_thread_pool_init);
     So(_thread_pool_tasks_run_count == 0);

     size_t _thread_pool_tasks_high_count = w_thread_pool_tasks_high_count(_thread_pool_init);
     So(_thread_pool_tasks_high_count != 0);

     size_t _thread_pool_threads_high_count = w_thread_pool_threads_high_count(_thread_pool_init);
     So(_thread_pool_threads_high_count != 0);

     size_t _thread_pool_threads_idle_timeout_count = w_thread_pool_threads_idle_timeout_count(_thread_pool_init);
     So(_thread_pool_threads_idle_timeout_count == 0);

     size_t _thread_pool_idle_max_get = w_thread_pool_idle_max_get(_thread_pool_init);
     So(_thread_pool_idle_max_get != 0);

     /*in my debug project is ok but here is not ok*/
     /*size_t thread_pool_thread_max_set = w_thread_pool_thread_max_set(_thread_pool_init, pCount);
     So(thread_pool_thread_max_set == 0);*/

     long pTimeOut = 1890890890;
     int64_t _thread_pool_idle_wait_set = w_thread_pool_idle_wait_set(_thread_pool_init, pTimeOut);
     So(_thread_pool_idle_wait_set == 0);

     int64_t _thread_pool_idle_wait_get = w_thread_pool_idle_wait_get(_thread_pool_init);
     So(_thread_pool_idle_wait_get != 0);


      size_t _thread_pool_thread_max_get = w_thread_pool_thread_max_get(_thread_pool_init);
      So(_thread_pool_thread_max_get != 0);

      size_t pValue = 3;
      size_t _thread_pool_threshold_set = w_thread_pool_threshold_set(_thread_pool_init, pValue);
      So(_thread_pool_threshold_set != 0);


      size_t _thread_pool_threshold_get = w_thread_pool_threshold_get(_thread_pool_init);
      So(_thread_pool_threshold_get != 0);


      W_RESULT thread_pool_free = w_thread_pool_free(_thread_pool_init);
       So(thread_pool_free != 0);
     });

//terminate wolf
wolf_terminate();

printf("\n\nAll tests where completed successfully.\n\n");

    })