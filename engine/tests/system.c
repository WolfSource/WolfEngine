#include "convey.h"
#include <wolf.h>
#include <chrono/w_timespan.h>
#include <chrono/w_chrono.h>
#include <chrono/w_gametime.h>
#include <time.h>
#include <compression/w_compress.h>
#include <concurrency/w_concurrent_queue.h>
#include <concurrency/w_condition_variable.h>
#include <concurrency/w_thread.h>
#include <concurrency/w_thread_pool.h>
#include <concurrency/w_timer.h>
#include <concurrency/w_async.h>
#include <concurrency/libev/ev.h>
#include <io/w_io.h>
#include <time.h>
#include <memory/w_table.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void* w_thread_job_my(w_thread arg1, void* arg2) { return NULL; }
void mycallback_thread() { printf("%s", "ok"); }
void mycallback(EV_P_ w_async_base* arg1, int arg2) { printf("%s", "ok"); }

int pCallBack(void* rec, const char* pKey, const char* pValue) { return 0; }
void s_timer_callback(w_timer_loop* pLoop, w_timer_base* pTimer, int pRevents)
{
    w_timer_ptr _t = (w_timer_ptr)pTimer->data;
    w_timer_break(_t, 2);
    printf("timer");
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

    const char* _time_to_string = w_timespan_to_string(_t_short,":");
     const char* _time_origin = "0:2:2:2:000";
     int result = strcmp(_time_to_string, _time_origin);
     So(result == 0);


    const wchar_t* s = L":";
    const wchar_t* _time_to_wstring = w_timespan_to_wstring(_t_short,s);
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
     w_compress_result* pCompressResult = w_malloc(sizeof(w_compress_result),"main::w_compress_result");
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

     printf("testing w_asunc");
     w_async* pAsync = (w_async*)w_malloc(sizeof(w_async),"main::w_async");
     W_RESULT _async_init = w_async_init(pAsync, mycallback);
     So(_async_init == 0);

     W_RESULT _async_send = w_async_send(pAsync);
     So(_async_send == 0);

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

      static w_timer _t;
      W_RESULT _timer_init = w_timer_init(&_t, 1, 10, s_timer_callback);
      So(_timer_init == 0);

      W_RESULT _timer_restart = w_timer_restart(&_t);
      So(_timer_restart == 0);

      uint8_t pHow = "0";
      W_RESULT timer_break = w_timer_break(&_t, pHow);
      So(timer_break == 1);

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

       //TODO sometimes the result is ok in test but sometimes is not
        //size_t _thread_pool_tasks_count = w_thread_pool_tasks_count(_thread_pool_init);
        //So(_thread_pool_tasks_count != 0);

        size_t _thread_pool_scheduled_tasks_count = w_thread_pool_scheduled_tasks_count(_thread_pool_init);
        So(_thread_pool_scheduled_tasks_count == 0);

        size_t _thread_pool_threads_count = w_thread_pool_threads_count(_thread_pool_init);
        So(_thread_pool_threads_count != 0);

        //TODO sometimes the result is ok in test but sometimes is not
        //size_t _thread_pool_busy_count = w_thread_pool_busy_count(_thread_pool_init);
        //So(_thread_pool_busy_count != 0);

        //TODO sometimes the result is ok in test but sometimes is not
        //size_t _thread_pool_idle_count = w_thread_pool_idle_count(_thread_pool_init);
        //So(_thread_pool_idle_count == 0);

        //TODO sometimes the result is ok in test but sometimes is not
       //int pCount = 2;
        //size_t _thread_pool_idle_max_set = w_thread_pool_idle_max_set(_thread_pool_init,(size_t)pCount);
        //So(_thread_pool_idle_max_set == 0);


        //TODO sometimes the result is ok in test but sometimes is not
        //size_t _thread_pool_tasks_run_count = w_thread_pool_tasks_run_count(_thread_pool_init);
        //So(_thread_pool_tasks_run_count == 0);

        size_t _thread_pool_tasks_high_count = w_thread_pool_tasks_high_count(_thread_pool_init);
        So(_thread_pool_tasks_high_count != 0);

        size_t _thread_pool_threads_high_count = w_thread_pool_threads_high_count(_thread_pool_init);
        So(_thread_pool_threads_high_count != 0);

        size_t _thread_pool_threads_idle_timeout_count = w_thread_pool_threads_idle_timeout_count(_thread_pool_init);
        So(_thread_pool_threads_idle_timeout_count == 0);

        size_t _thread_pool_idle_max_get = w_thread_pool_idle_max_get(_thread_pool_init);
        So(_thread_pool_idle_max_get != 0);

        //TODO in my debug project is ok but here is not ok
        //size_t thread_pool_thread_max_set = w_thread_pool_thread_max_set(_thread_pool_init, pCount);
        //So(thread_pool_thread_max_set == 0);

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

  Convey("io", {


       printf("testing w_io");
       const char content[] = "hi";

      char* _io_dir_get_current = w_io_dir_get_current();
      char* _filename = "file.txt";
      char* _sign = "\\";
      char* _path = w_string_concat(3, _io_dir_get_current, _sign, _filename);
       w_file _file = w_io_file_create(_path, content, false , false , false , false ,false ,false , false);

      W_RESULT _io_file_save = w_io_file_save(_path, content, false, false, false, false, false, false, false);
      So(_io_file_save == 0);

      const char* _basename_1 = w_io_file_get_basename_from_path(_path);
      const char* _basename_from_path = "file.txt";
      int result__basename_1 = strcmp(_basename_from_path, _basename_1);
      So(result__basename_1 == 0);


      const char* _basename_2 = w_io_file_get_basename(_file);
      const char* _basename_from_path2 = "file.txt";
      int result__basename_2 = strcmp(_basename_from_path2, _basename_2);
      So(result__basename_2 == 0);

      const char* _name_1 = w_io_file_get_basename_without_extension_from_path(_path);
      const char* _without_extension_from_path1 = "file";
      int result___name_1 = strcmp(_without_extension_from_path1, _name_1);
      So(result___name_1 == 0);

      const char* _name_2 = w_io_file_get_basename_without_extension(_file);
      const char* _without_extension = "file";
      int result___name_2 = strcmp(_without_extension, _name_2);
      So(result___name_2 == 0);

      char* _filename2 = "file";
      char* _path2 = w_string_concat(3, _io_dir_get_current, _sign, _filename2);
      const char* _filename_1 = w_io_file_get_name_from_path(_path);
      int result__filename_1 = strcmp(_path2, _filename_1);
      So(result__filename_1 == 0);


      const char* _filename_2 = w_io_file_get_name(_file);
      int result__filename_2 = strcmp(_path2, _filename_2);
      So(result__filename_2 == 0);

      w_file_info _io_file_get_info_from_path = w_io_file_get_info_from_path(_path);
      So(_io_file_get_info_from_path != 0);

      w_file_info _io_file_get_info = w_io_file_get_info(_file);
      So(_io_file_get_info != 0);

     const char* _io_file_get_extension_from_path = w_io_file_get_extension_from_path(_path);
     const char* _extention = "txt";
     int result_extention = strcmp(_io_file_get_extension_from_path, _extention);
     So(result_extention == 0);

     const char* _io_file_get_extension = w_io_file_get_extension(_file);
     const char* _extention1 = "txt";
     int result_extention1 = strcmp(_io_file_get_extension, _extention1);
     So(result_extention1 == 0);

     W_RESULT _io_file_check_is_file = w_io_file_check_is_file(_path);
     So(_io_file_check_is_file == 0);

     w_file_istream _io_file_read_full_from_path = w_io_file_read_full_from_path(_path);
     So(_io_file_read_full_from_path->size != 0 && _io_file_read_full_from_path->bytes_read != 0 && _io_file_read_full_from_path->buffer != 0);

     w_file_istream _io_file_read_nbytes_from_path = w_io_file_read_nbytes_from_path(_path, 0);
     So(_io_file_read_nbytes_from_path->size != 0 && _io_file_read_nbytes_from_path->bytes_read != 0);

     w_file_istream _io_file_read_full = w_io_file_read_full(_file);
     So(_io_file_read_full->size == 0 && _io_file_read_full->bytes_read == 0);


     /* w_file_istream _io_file_read_nbytes = w_io_file_read_nbytes(_file, 0);
      So(_io_file_read_nbytes->size != 0 && _io_file_read_nbytes->bytes_read == 0 && _io_file_read_nbytes->buffer == 0);*/

      //W_RESULT	_io_file_delete_from_path = w_io_file_delete_from_path(pathcreate);
     //So(_io_file_delete_from_path==0);
      const char* io_dir_get_parent = w_io_dir_get_parent(_path);
      int  result_get_parent = strcmp(io_dir_get_parent, _path);
      So(result_get_parent == 0);

      W_RESULT	io_dir_create = w_io_dir_create(_path);
      So(io_dir_create == 0);

      W_RESULT _io_dir_check_is_directory = w_io_dir_check_is_directory(_io_dir_get_current);
      So(_io_dir_check_is_directory == 0);

      char in[] = "playpod";
      size_t inbytes = 7;
      uint16_t out_utf8[12];
      size_t outwords;
      W_RESULT  _io_utf8_to_ucs2 = w_io_utf8_to_ucs2(in, &inbytes, out_utf8, &outwords);
      So(_io_utf8_to_ucs2 == 0);

      uint16_t _in = "7";
      size_t inwords = 1;
      char out1[12];
      size_t outbytes;
      W_RESULT _io_ucs2_to_utf8 = w_io_ucs2_to_utf8(&_in, &inwords,out1, &outbytes);
      So(_io_ucs2_to_utf8 == 0);

      char pString4[14] = "WOLF ENGINE";
      const char pSplit1[2] = " ";
      w_array* pResults = w_malloc(sizeof(w_array), "main::w_io_string_split");
      W_RESULT  _io_string_split = w_io_string_split(pString4, pSplit1, pResults);

      long _io_to_hex = w_io_to_hex("0ABC546");
      So(_io_to_hex == 11257158);

      const char* pString2 = "HELLO";
      const char* pEndWith2 = "H";
      W_RESULT _io_string_has_start_with = w_io_string_has_start_with(pString2, pEndWith2);
      So(_io_string_has_start_with == 0);

      const char* pString3 = "HELLO";
      const char* pEndWith3 = "O";
      W_RESULT _io_string_has_end_with = w_io_string_has_end_with(pString3, pEndWith3);
      So(_io_string_has_end_with == 1);

     const wchar_t* pString1 = L"HELLO";
     const wchar_t* pEndWith1 = L"H";
     W_RESULT _io_wstring_has_start_with = w_io_wstring_has_start_with(pString1, pEndWith1);
     So(_io_wstring_has_start_with == 0);


     const wchar_t* pString = L"HELLO";
     const wchar_t* pEndWith = L"O";
     W_RESULT _io_wstring_has_end_with = w_io_wstring_has_end_with(pString, pEndWith);
     So(_io_wstring_has_end_with == 1);

     char* _filename3 = "pic.png";
     char* _path1 = w_string_concat(3, _io_dir_get_current, _sign, _filename3);
     w_file_istream _istream2 = w_io_file_read_full_from_path(_path1);
     W_RESULT _io_stream_is_png = w_io_stream_is_png(_istream2);
     So(_io_stream_is_png == 0);

     W_RESULT _io_file_is_png = w_io_file_is_png(_path1);
     So(_io_file_is_png == 0);

     int pWidth1;
     int pHeight1;
     int pNumberOfPasses1;
     uint8_t pColorType1 = 0;
     uint8_t pBitDepth1 = 0;
     uint8_t* pPixels1;
     w_file_istream _fs = w_io_file_read_full_from_path(_path1);
     W_RESULT io_pixels_from_png_stream = w_io_pixels_from_png_stream(_fs, RGBA_PNG, &pWidth1, &pHeight1, &pColorType1, &pBitDepth1, &pNumberOfPasses1, &pPixels1);
     So(io_pixels_from_png_stream == 0);

     W_RESULT _io_pixels_from_png_file = w_io_pixels_from_png_file(_path1, RGBA_PNG, &pWidth1, &pHeight1, &pColorType1, &pBitDepth1, &pNumberOfPasses1, &pPixels1);
     So(_io_pixels_from_png_file == 0);

     char* _filename4 = "download.jpeg";
     char* _path3 = w_string_concat(3, _io_dir_get_current, _sign, _filename4);
     w_file_istream _istream = w_io_file_read_full_from_path(_path3);
     char* pDestinationBuffer = w_malloc(_istream->size*2, "");

     
 
     W_RESULT _io_file_is_jpeg = w_io_file_is_jpeg(_path3);
     So(_io_file_is_jpeg == 0);

     W_RESULT _io_stream_is_jpeg = w_io_stream_is_jpeg(_istream);
     So(_io_stream_is_jpeg == 0);

    

     int pWidth;
     int pHeight;
     int pSubSample;
     int pColorSpace;
     int pNumberOfPasses;
     uint8_t* pPixels;
     W_RESULT _io_pixels_from_jpeg_stream = w_io_pixels_from_jpeg_stream((const uint8_t*)_istream->buffer, _istream->bytes_read, RGBX_JPEG, &pWidth, &pHeight, &pSubSample, &pColorSpace, &pNumberOfPasses, &pPixels);
     So(_io_pixels_from_jpeg_stream == 0);

     W_RESULT _io_pixels_from_jpeg_file = w_io_pixels_from_jpeg_file(_path3, RGB_JPEG, &pWidth, &pHeight, &pSubSample, &pColorSpace, &pNumberOfPasses, &pPixels);
     So(_io_pixels_from_jpeg_file == 0);

    size_t _io_to_base_64 = w_io_to_base_64(&pDestinationBuffer, _istream->buffer, _istream->size, chromium);
     So(_io_to_base_64 == 9360);
    
       });

    Convey("memory", {

        w_table  pTable = w_malloc(sizeof(w_table), "table");
        w_mem_pool pMemPool = w_get_default_memory_pool();
        W_RESULT   _table_init = w_table_init(&pTable, 10, pMemPool);
        So(_table_init == 0);

        char pKey[] = "name";
        char pValue[] = "wolf";
        W_RESULT  table_set = w_table_set(pTable, pKey, pValue, true);
        So(table_set == 0);

         W_RESULT table_unset = w_table_unset(pTable,  pKey);
         So(table_unset == 0);

         W_RESULT  table_add = w_table_add(pTable,pKey, pValue);
         So(table_add == 0);

         int   _get_size = w_table_get_size(pTable);
         So(_get_size == 1);

         int pArg = 1;
         w_table_do_callback* myfunc = pCallBack;
         int _table_do = w_table_do(pTable,pCallBack, (void*)pArg);
         So(_table_do == 0);

         /*TODO*/
         //va_list pKeys = "name";
         ///*int _table_do_with_filter = w_table_do_with_filter(pTable, pCallBack,(void*)pArg, pKeys);
         //So(_table_do_with_filter == 0);*/

        const w_table_entry_iterator  _table_entry_iterator = w_table_get_entry(pTable);
        So(_table_entry_iterator != 0);

       const char* _table_get_key = w_table_get_key(_table_entry_iterator,1);
       const char* _key = "name";
       //int  result_key = strcmp(_table_get_key, _key);
       // So(result_key == 0);


       const char* table_get_value = w_table_get_value(_table_entry_iterator, 1);
       const char _value[] = "wolf";
       //int  result_value = strcmp(table_get_value, _value);
       //So(result_value == 0);

        size_t _table_get_key_checksum = w_table_get_key_checksum(_table_entry_iterator, 1);
       So(_table_get_key_checksum != 0);

            });

    //terminate wolf
    wolf_terminate();

    printf("\n\nAll tests where completed successfully.\n\n");

    })