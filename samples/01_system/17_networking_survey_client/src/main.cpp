/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create one two way client in Wolf Engine
					   make sure launch 16_networking_survey_server simultaneously
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace wolf;
using namespace wolf::system;

//Entry point of program 
WOLF_MAIN()
{
    const wchar_t* _name = L"17_networking_survey_client";
    logger.initialize(_name, wolf::system::io::get_current_directoryW());
    
    w_signal<void(const int&)> on_connect_established;
	on_connect_established += [](const int& pSocketID)
	{
		logger.write("survey client launched on socket ID: {}", pSocketID);

		while (1)
		{
			//receive
			char* _rcv_buffer = nullptr;
			auto _rcv_len = w_network::receive(pSocketID, &_rcv_buffer);
			if (_rcv_len >= 0)
			{
				std::string _rcv_msg;
				_rcv_msg.resize(_rcv_len);
				memcpy(&_rcv_msg[0], _rcv_buffer, _rcv_len);

				logger.write("message \'{}\' received", _rcv_msg);

				_rcv_msg.clear();
				
				//send
				std::string _msg = "voting at " + w_time_span::now().to_string();
				if (w_network::send(pSocketID, _msg.c_str(), _msg.size()) >= 0)
				{
					logger.write("message \'{}\' sent.", _msg);
				}
				_msg.clear();
			}
			else
			{
				logger.write("could not receive");
			}
			w_network::free_buffer(_rcv_buffer);

			//wait for 1 second
			w_thread::sleep_current_thread(1000);
		}
	};
    
    w_network _survey_server;
	_survey_server.setup_survey_client("tcp://127.0.0.1:1237", on_connect_established);
    
    //sleep 5 sec
    w_thread::sleep_current_thread(5000);
    
	_survey_server.release();
    logger.release();

	return EXIT_SUCCESS;
}
