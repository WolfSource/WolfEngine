/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create one two way client in Wolf Engine
					   make sure launch 12_networking_two_way_server simultaneously
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace wolf;
using namespace wolf::system;

//Entry point of program 
WOLF_MAIN()
{
    w_logger_config _log_config;
    _log_config.app_name = L"13_networking_two_way_client";
    _log_config.log_path = wolf::system::io::get_current_directoryW();
#ifdef __WIN32
    _log_config.log_to_std_out = false;
#else
    _log_config.log_to_std_out = true;
#endif
    logger.initialize(_log_config);
    
    w_signal<void(const int&)> on_bind_established;
	on_bind_established += [](const int& pSocketID)
    {
        logger.write("two way client launched with socket ID: {}", pSocketID);
        
		while (true)
		{
			//send
			std::string _msg = "message " + w_time_span::now().to_string();
			if (w_network::nano_send(pSocketID, _msg.c_str(), _msg.size()) >= 0)
			{
				logger.write("message \'{}\' sent.", _msg);
			}
			_msg.clear();

			//wait 0.5 second
			w_thread::sleep_current_thread(500);

			//receive
			char* _rcv_buffer = nullptr;
			auto _rcv_len = w_network::nano_receive(pSocketID, &_rcv_buffer);
			if (_rcv_len >= 0)
			{
				std::string _rcv_msg;
				_rcv_msg.resize(_rcv_len);
				memcpy(&_rcv_msg[0], _rcv_buffer, _rcv_len);

				logger.write("message \'{} \' received", _rcv_msg);

				_rcv_msg.clear();
			}
			else
			{
				logger.write("could not receive");
			}
			w_network::free_buffer(_rcv_buffer);
		}
    };
    
    w_network _puller;
    _puller.setup_two_way_client("tcp://127.0.0.1:1235", on_bind_established, 100, 100);
        
    _puller.release();
    logger.release();

	return EXIT_SUCCESS;
}
