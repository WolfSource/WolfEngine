/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to send notification between threads with signal
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace std;
using namespace wolf::system;

WOLF_MAIN()
{
    //initialize logger, and log in to the output debug window of visual studio and Log folder inside running directory
    logger.initialize(L"01_system-03_signal", wolf::system::io::get_current_directoryW());

    //log to output file
    logger.write(L"starting Wolf");

    struct signal_args
    {
        int id = -1;
        std::string message;
    };

    //create signal
    w_signal<void(signal_args&)> _signal;

    //create two threads
    w_thread _thread_sender;
    _thread_sender.add_job([&]()
    {
        int _number_of_messages = 10;
        signal_args _sender;
        while (_number_of_messages != -1)
        {
            //send message
            _sender.id = _number_of_messages;
            _sender.message = "hello with id: " + std::to_string(_number_of_messages);
            _signal.emit(_sender);
            
            //decrease number of messages
            _number_of_messages--;

            //sleep for 1 sec
            w_thread::sleep_current_thread(1000);
        }
    });

    w_thread _thread_reciever;
    _thread_reciever.add_job([&]()
    {
        signal_args _last_recieved;
        _signal += [&](signal_args& pArg) ->void
        {
            _last_recieved = pArg;
        };
        while (_last_recieved.id != 0)
        {
            w_thread::sleep_current_thread(1000);
            logger.write("last message recieved: "  + _last_recieved.message);
        }
    });

    //wait for both thread 
    logger.write("main is waiting for others threads to execute their jobs");
    _thread_sender.wait();
    _thread_reciever.wait();
    
    logger.write("releasing threads");
    _thread_sender.release();
    _thread_reciever.release();

    //output a message to the log file
    logger.write(L"shutting down Wolf");

    //release logger
    logger.release();

    return EXIT_SUCCESS;
}


