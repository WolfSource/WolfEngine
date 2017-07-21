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
using namespace wolf::system;

WOLF_MAIN()
{
    //initialize logger, and log in to the output debug window of visual studio and Log folder inside running directory
    logger.initialize(L"01_system-04_timer", wolf::system::io::get_current_directoryW());

    //log to output file
    logger.write(L"starting Wolf");

    struct signal_args
    {
        int id = -1;
        std::string message;
    };

    //create a timer and start it
    w_timer _timer;
    _timer.start();
    
    //write a file
    io::write_text_file("test.txt", "this is test from 01_system-04-timer");
    
    auto _time = _timer.get_seconds();
    
    char _buffer[256];
    sprintf_s(_buffer, "writing file took: %d (s)", _time);
    logger.write(_buffer);
        
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

    //output a message to the log file
    logger.write(L"shutting down Wolf");

    //release logger
    logger.release();

    return EXIT_SUCCESS;
}


