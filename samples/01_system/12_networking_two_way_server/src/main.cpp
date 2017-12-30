/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create one two way server in Wolf Engine
					   make sure launch 13_networking_two_way_client simultaneously
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace wolf;
using namespace wolf::system;

//Entry point of program 
WOLF_MAIN()
{
    const wchar_t* _name = L"12_networking_two_way_server";
    WOLF_INIT(_name);
    
    w_signal<void(const int&)> on_bind_established;
	on_bind_established += [](const int& pSocketID)
    {
        logger.write("two way server launched with socket ID: " + std::to_string(pSocketID));
        
        //char* _buffer = nullptr;
        //while (1)
        //{
        //    if(w_network::receive(pSocketID, &_buffer) >= 0)
        //    {
        //        logger.write("Roger that, puller received: " + std::string(_buffer));
        //        break;
        //    }
        //}
    };
    
    w_network _puller;
    _puller.setup_two_way_server("tcp://127.0.0.1:1235", 100, on_bind_established);
    
    //sleep 5 sec
    w_thread::sleep_current_thread(5000);
    
    _puller.release();
    logger.release();

	return EXIT_SUCCESS;
}
