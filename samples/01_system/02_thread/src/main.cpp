/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to write thread
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace std;
using namespace wolf::system;

WOLF_MAIN()
{
    //initialize logger, and log in to the output debug window of visual studio(just for windows) and Log folder inside running directory
    WOLF_INIT(L"02_thread");
    
    //log to output file
    logger.write(L"Wolf initialized");

    //get number of hardware thread contexts which is needed for allocating thread pool's memory
    auto _thread_pool_size = w_thread::get_number_of_hardware_thread_contexts();

    //allocate thread pool's memory
    w_thread_pool _thread_pool;
    _thread_pool.allocate(_thread_pool_size);

    for (size_t i = 0; i < _thread_pool.get_pool_size(); ++i)
    {
        _thread_pool.add_job_for_thread(i, [i]()
        {
            auto _sleep_time = 1000 + i * 1000;
            logger.write("thread with id: " + std::to_string(w_thread::get_current_thread_id()) +
                " just started and is going to sleep for: " + std::to_string(_sleep_time));
            w_thread::sleep_current_thread(_sleep_time);
        });
    }

    logger.write("main thread with id: " + std::to_string(w_thread::get_current_thread_id()) + " is going to sleep for 5(s).");
    w_thread::sleep_current_thread(5000);
    logger.write("main thread waked up.");

    logger.write("main is waiting for others threads to execute their jobs");
    _thread_pool.wait_all();

    logger.write("releasing thread pool");
    _thread_pool.release();

    //output a message to the log file
    logger.write(L"shutting down Wolf");

    //release logger
    logger.release();

    return EXIT_SUCCESS;
}


