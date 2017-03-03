/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to write async tasks in Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace std;

//global variables

//Entry point of program
#ifdef __WIN32
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
#elif defined(__linux) || defined(__APPLE__) 
int main(int pArgc, const char * pArgv[])
#endif
{
	//initialize logger, and log in to the output debug window of visual studio and Log folder inside running directory
#ifdef __WIN32
	logger.initialize(L"00_01_async.Win32", wolf::system::io::get_current_directoryW());
#elif defined(__APPLE__) || defined(__linux)
    logger.initialize("00_01_async.OSX", wolf::system::io::get_current_directory());
#endif
	//log to output file
	logger.write(L"Starting Wolf");

    //test tbb task
#ifndef __ANDROID
        
        std::function<void(void)> _task_work = []() -> void
        {
             logger.write("tbb task executed");
        };
        tbb::Task _tbb_task(_task_work);
        _tbb_task.execute();
    
#endif
    
#if defined(__WIN32) || defined(__UNIVERSAL)
	//execute async tasks with ppl
	wolf::system::w_task::execute_async_ppl([]()-> void
	{
		logger.write("First task of Async_ppl done");
	}, []()
	{
		logger.write("Second task of Async_ppl done");
	});
#endif

	//execute async task in standard c++
	wolf::system::w_task::execute_async([]()-> void
	{
		logger.write("Async task 01 done");
	}, []()
	{
		logger.write("Async task 02 done");
	});

	//define deferred task
	wolf::system::w_task::execute_deferred([]()-> void
	{
		logger.write("Deferred task 01 started");
		//wait for 5 sec
                std::this_thread::sleep_for(5s);
		logger.write("Deferred task 01 done");
	});
	//main thread will wait for only 1 sec and then continue
	auto _status = wolf::system::w_task::wait_for(std::chrono::seconds(1).count());
	//check status of deferred task
	switch (_status)
	{
	case std::future_status::ready:
		logger.write("Deferred task is ready");
		break;
	case std::future_status::timeout:
		logger.write("Deferred task timeout");
		break;
	case std::future_status::deferred:
		logger.write("Deferred task deferred");
		//call it
		wolf::system::w_task::get();
		break;
	}

	//create another deferred task
	wolf::system::w_task::execute_deferred([]()-> void
	{
		logger.write("Deferred task 02 started");
		//wait for 1 sec
                std::this_thread::sleep_for(1s);
		logger.write("Deferred task 02 done");
	});
	//main thread will block until deferred task done
	wolf::system::w_task::wait();

	logger.write("All done");

	//output a message to the log file
	logger.write(L"Shutting down Wolf");

        //release logger
        logger.release();
    
	//exit
	return EXIT_SUCCESS;
}
