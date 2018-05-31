/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create one two way client in Wolf Engine
					   make sure launch 15_networking_broadcast_subscriptore simultaneously
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace wolf;
using namespace wolf::system;

//Entry point of program 
WOLF_MAIN()
{
    const wchar_t* _name = L"14_networking_broadcast_publisher";
    logger.initialize(_name, wolf::system::io::get_current_directoryW());
    
    w_signal<void(const int&)> on_bind_established;
	on_bind_established += [](const int& pSocketID)
    {
        logger.write("publisher server launched with socket ID: {}", pSocketID);
        
		while (1)
		{
			//send
			std::string _msg = "message " + w_time_span::now().to_string();
			if (w_network::send(pSocketID, _msg.c_str(), _msg.size()) >= 0)
			{
				logger.write("message \'{}\' published.", _msg);
			}
			_msg.clear();

			//wait for 1 second
			w_thread::sleep_current_thread(1000);
		}
    };
    
    w_network _publisher;
	_publisher.setup_broadcast_publisher("tcp://127.0.0.1:1236", on_bind_established);
    
    //sleep 5 sec
    w_thread::sleep_current_thread(5000);
    
	_publisher.release();
    logger.release();

	return EXIT_SUCCESS;
}
