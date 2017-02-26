/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_tcp_client.h
	Description		 : sync client tcp connection to server
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_TCP_CLIENT_H__
#define __W_TCP_CLIENT_H__

#ifdef __WIN32

#if defined(__WIN32)

#ifdef _DEBUG
#pragma comment(lib, "libboost_system-vc140-mt-gd-1_61.lib")
#else
#pragma comment(lib, "libboost_system-vc140-mt-1_61.lib")
#endif

#include "w_target_ver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

#endif

#include "w_system_export.h"
#include <boost/bind.hpp>
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
				WSYS_EXP w_tcp_client(boost::asio::io_service& pService, const char* pIpAddress = "127.0.0.1", unsigned short pPort = 1234);
				WSYS_EXP ~w_tcp_client();

				WSYS_EXP bool connect();
				WSYS_EXP void async_connect();

				WSYS_EXP size_t send(const std::string& pMsg);
				WSYS_EXP void async_send(const std::string& pMsg);

				WSYS_EXP size_t read(std::string& pBuffer);
				WSYS_EXP void async_read();

				WSYS_EXP void close();
				WSYS_EXP void close(boost::system::error_code& pErrorCode);

				WSYS_EXP void register_on_connect_callback(boost::function<void(const boost::system::error_code&)> pCallBack);
				WSYS_EXP void register_on_sent_callback(boost::function<void(const boost::system::error_code&, const size_t)> pCallBack);
				WSYS_EXP void register_on_received_callback(boost::function<void(const boost::system::error_code&, const size_t, std::string)> pCallBack);
				WSYS_EXP void unregister_all_callbacks();

#pragma region Getters

				WSYS_EXP const boost::system::error_code get_error_code() const				{ return this->_error_code; }
				WSYS_EXP const bool get_is_connected() const								{ return this->_is_connected; }

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

#endif // __WIN32

#endif //__W_TCP_CLIENT_H__