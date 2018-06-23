/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to use timer and timer callback
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace std;
using namespace wolf;
using namespace wolf::system;

WOLF_MAIN()
{
    w_logger_config _log_config;
    _log_config.app_name = L"04_timer";
    _log_config.log_path = wolf::system::io::get_current_directoryW();
#ifdef __WIN32
    _log_config.log_to_std_out = false;
#else
    _log_config.log_to_std_out = true;
#endif
    logger.initialize(_log_config);

    //log to output file
    logger.write(L"Wolf initialized");

    struct signal_args
    {
        int id = -1;
        std::string message;
    };

    //create a timer and start it
    w_timer _timer;
    _timer.start();
    
    //write to file
    io::write_text_file("test.txt", "this is test from 04-timer");
    
    auto _time = _timer.get_seconds();
    logger.write("writing file took: {} (s)", _time);
    _timer.reset();
       
    //start callback timer
    w_timer_callback _timer_with_callback;
    _timer_with_callback.do_sync(1000, []()
    {
        logger.write("callback invoked");
    });

    //start callback timer
    _timer_with_callback.do_async(1000, []()
    {
        logger.write("async callback invoked");
    });

    //wait for 2s to execute last timer's job
    w_thread::sleep_current_thread(2000);

    //release logger
    logger.release();

    return EXIT_SUCCESS;
}


