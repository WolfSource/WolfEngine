/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create one way server side pusher in Wolf Engine
					   make sure launch 10_networking_one_way_pusher simultaneously	
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace wolf;
using namespace wolf::system;

//Entry point of program 
WOLF_MAIN()
{
    const wchar_t* _name = L"10_networking_one_way_pusher";
    logger.initialize(_name, wolf::system::io::get_current_directoryW());
    
    w_signal<void(const int&)> on_connection_established;
    on_connection_established += [](const int& pSocketID)
    {
        logger.write("pusher launched with socket ID: {}", pSocketID);
        
        const char* _msg = "Hello, I'm pusher!";
        auto _len = strlen(_msg) + 1;//1 for "\0" end of message
        if(w_network::send(pSocketID, _msg, _len) != _len)
        {
            logger.error("count of sent bytes not equal to count of message bytes");
            return;
        }
        
        logger.write("message sent");
    };
    
    w_network _pusher;
    _pusher.setup_one_way_pusher("tcp://127.0.0.1:1234", on_connection_established);
    
    //sleep 5 sec
    w_thread::sleep_current_thread(5000);
    
    _pusher.release();
    logger.release();

	return EXIT_SUCCESS;
}
