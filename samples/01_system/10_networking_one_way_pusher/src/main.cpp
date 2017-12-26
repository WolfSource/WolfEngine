/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create one way server side pusher in Wolf Engine
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
    WOLF_INIT(_name);
    
    w_signal<void(const int&)> on_connection_established;
    on_connection_established += [](const int& pSocket)
    {
        logger.write("pusher launched with socket ID: " + std::to_string(pSocket));
    };
    
    w_network _pusher;
    _pusher.setup_one_way_pusher("http://127.0.0.1:1234", on_connection_established);
    
    _pusher.release();
    
    logger.release();

	return EXIT_SUCCESS;
}
