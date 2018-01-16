/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create one way client side puller in Wolf Engine
					   make sure launch 11_networking_one_way_pusher simultaneously
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace wolf;
using namespace wolf::system;

//Entry point of program 
WOLF_MAIN()
{
    const wchar_t* _name = L"11_networking_one_way_puller";
    WOLF_INIT(_name);
    
    w_signal<void(const int&)> on_bind_established;
	on_bind_established += [](const int& pSocketID)
    {
        logger.write("puller launched with socket ID: " + std::to_string(pSocketID));
        
        char* _buffer = nullptr;
        while (1)
        {
            if(w_network::receive(pSocketID, &_buffer) >= 0)
            {
                logger.write("Roger that, puller received: " + std::string(_buffer));
                break;
            }
        }
    };
    
    w_network _puller;
    _puller.setup_one_way_puller("tcp://127.0.0.1:1234", on_bind_established);
    
    //sleep 5 sec
    w_thread::sleep_current_thread(5000);
    
    _puller.release();
    logger.release();

	return EXIT_SUCCESS;
}
