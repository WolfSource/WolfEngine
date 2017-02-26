/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to write async tasks in Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"
#include <w_logger.h>
#include <w_task.h>

//namespaces
using namespace std;

//global variables

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	//initialize logger, and log in to the output debug window of visual studio and Log folder beside the ".exe".
	logger.initialize(L"00_01_Async.Win32", wolf::system::io::get_current_directoryW());

	//log to output file
	logger.write(L"Starting Wolf");

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
		//wait for 1 sec
		Sleep(5000);
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
		Sleep(1000);
		logger.write("Deferred task 02 done");
	});
	//main thread will block until deferred task done
	wolf::system::w_task::wait();

	logger.write("All done");

	//release logger
	logger.release();

	//output a message to the log file
	logger.write(L"Shutting down Wolf");

	//exit
	return EXIT_SUCCESS;
}