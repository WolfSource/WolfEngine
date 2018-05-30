/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to write async tasks
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace std;
using namespace wolf;
using namespace wolf::system;

WOLF_MAIN()
{
    //initialize logger, and log in to the output debug window of visual studio(just for windows) and Log folder inside running directory
    logger.initialize(L"01_async", wolf::system::io::get_current_directoryW());

    //log to output file
    logger.write("Wolf initialized");

    //execute async task in standard c++
    w_task::execute_async([]()-> W_RESULT
    {
        logger.write("async task 01 started");
        return W_PASSED;

    }, [](W_RESULT pHR)-> void
    {
        logger.write("async task 01 completed");
    });

    //define deferred task
    w_task::execute_deferred([]()-> W_RESULT
    {

        logger.write("deferred task 01 started");
        //wait for 5 sec
        std::this_thread::sleep_for(5s);
        logger.write("deferred task 01 done");
        return W_PASSED;

    });
    //deferred wait for only 1 sec and then continue
    auto _status = w_task::wait_for(std::chrono::seconds(1).count());
    //check status of deferred task
    switch (_status)
    {
    case std::future_status::ready:
        logger.write("deferred task is ready");
        break;
    case std::future_status::timeout:
        logger.write("deferred task timeout");
        break;
    case std::future_status::deferred:
        logger.write("deferred task deferred");
        //call it
        wolf::system::w_task::get();
        break;
    }

    //create another deferred task
    w_task::execute_deferred([]()-> W_RESULT
    {
        logger.write("deferred task 02 started");
        //wait for 1 sec
        w_thread::sleep_current_thread(1000);
        logger.write("deferred task 02 done");
        return W_PASSED;
    });
    //deferred will block until deferred task done
    w_task::wait();

    logger.write("all done");
	
    //release logger
    logger.release();

    return EXIT_SUCCESS;
}


