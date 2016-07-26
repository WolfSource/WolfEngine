/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_tcp_client.h
	Description		 : sync client tcp connection to server
	Comment          :
*/

#ifndef __W_TCP_CLIENT_H__
#define __W_TCP_CLIENT_H__

#include "w_system_dll.h"
#include <boost/asio.hpp>
#include <boost/signals/signal1.hpp>
#include <boost/signals/signal2.hpp>
#include <boost/signals/signal3.hpp>
#include <queue>

namespace wolf
{
	namespace system
	{
		namespace network
		{

#define MAX_MESSAGE_SIZE 4096 
			
			class w_tcp_client
			{
			public:
				SYS_EXP w_tcp_client(boost::asio::io_service& pService, const char* pIpAddress = "127.0.0.1", unsigned short pPort = 1234);
				SYS_EXP ~w_tcp_client();

				SYS_EXP bool connect();
				SYS_EXP void async_connect();

				SYS_EXP size_t send(const std::string& pMsg);
				SYS_EXP void async_send(const std::string& pMsg);

				SYS_EXP size_t read(std::string& pBuffer);
				SYS_EXP void async_read();

				SYS_EXP void close();
				SYS_EXP void close(boost::system::error_code& pErrorCode);

				SYS_EXP void register_on_connect_callback(boost::function<void(const boost::system::error_code&)> pCallBack);
				SYS_EXP void register_on_sent_callback(boost::function<void(const boost::system::error_code&, const size_t)> pCallBack);
				SYS_EXP void register_on_received_callback(boost::function<void(const boost::system::error_code&, const size_t, std::string)> pCallBack);
				SYS_EXP void unregister_all_callbacks();

#pragma region Getters

				SYS_EXP const boost::system::error_code get_error_code() const				{ return this->_error_code; }
				SYS_EXP const bool get_is_connected() const									{ return this->_is_connected; }

#pragma endregion

			private:
				void _on_connect(const boost::system::error_code& pError);
				void _on_sent(const boost::system::error_code& pErrorCode, size_t pBytesTransferred);
				void _on_received(const boost::system::error_code& pErrorCode, size_t pBytesTransferred);
				
				boost::asio::ip::tcp::socket														_socket;
				bool																				_is_connected;
				const char*																			_ip_address;
				unsigned short																		_port;
				char																				_buffer[MAX_MESSAGE_SIZE];
				boost::system::error_code															_error_code;
				std::queue<std::string>																_write_queue;

				boost::signal1<void, const boost::system::error_code&>*								_on_connect_callback;
				boost::signal2<void, const boost::system::error_code&, const size_t>*				_on_sent_callback;
				boost::signal3<void, const boost::system::error_code&, const size_t, std::string>*	_on_received_callback;
				
			};
		}
	}
}

#endif