#include "w_system_pch.h"
#include "w_tcp_client.h"

using namespace wolf::system::network;
using namespace boost::asio::ip;

w_tcp_client::w_tcp_client(boost::asio::io_service& pIOService, const char* pIpAddress, unsigned short pPort) :
	_socket(pIOService),
	_ip_address(pIpAddress),
	_port(pPort),
	_on_connect_callback(nullptr),
	_on_sent_callback(nullptr),
	_on_received_callback(nullptr),
	_is_connected(false)
{
}

w_tcp_client::~w_tcp_client()
{
	close();
}

bool w_tcp_client::connect()
{
	tcp::endpoint _end_point(address::from_string(this->_ip_address), this->_port);
	try
	{
		this->_socket.connect(_end_point);
		this->_is_connected = true;
	}
	catch (...)
	{
		this->_is_connected = false;
	}

	return this->_is_connected;
}

void w_tcp_client::async_connect()
{
	this->_error_code.clear();
	tcp::endpoint _end_point(address::from_string(this->_ip_address), this->_port);
	this->_socket.async_connect(_end_point,
		boost::bind(
			&w_tcp_client::_on_connect,
			this,
			boost::asio::placeholders::error));
}

void w_tcp_client::_on_connect(const boost::system::error_code& pErrorCode)
{
	if (pErrorCode)
	{
		this->_is_connected = false;
		this->_error_code = pErrorCode;
	}
	else
	{
		this->_is_connected = true;
	}

	if (this->_on_connect_callback)
	{
		(*this->_on_connect_callback)(pErrorCode);
	}
}

size_t w_tcp_client::send(const std::string& pMsg)
{
	if (!this->_is_connected) return 0;

	return this->_socket.write_some(boost::asio::buffer(pMsg));
}

void w_tcp_client::async_send(const std::string& pMsg)
{
	if (!this->_is_connected) return;

	this->_write_queue.push(pMsg);
	this->_socket.async_write_some(boost::asio::buffer(this->_write_queue.front()),
		boost::bind(
			&w_tcp_client::_on_sent,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void w_tcp_client::_on_sent(const boost::system::error_code& pErrorCode, size_t pBytesTransferred) 
{
	this->_write_queue.pop();
	if (pErrorCode)
	{
		this->_error_code = pErrorCode;
	}
	if (_on_sent_callback)
	{
		(*this->_on_sent_callback)(pErrorCode, pBytesTransferred);
	}
}

size_t w_tcp_client::read(std::string& pMsg)
{
	pMsg = "";
	if (!this->_is_connected) return 0;

	std::fill(this->_buffer, this->_buffer + MAX_MESSAGE_SIZE, '\0');
	auto _size = this->_socket.read_some(boost::asio::buffer(this->_buffer));
	if (_size != 0)
	{
		pMsg = this->_buffer;
		return _size;
	}
	return 0;
}

void w_tcp_client::async_read()
{
	if (!this->_is_connected) return;

	std::fill(this->_buffer, this->_buffer + MAX_MESSAGE_SIZE, '\0');
	this->_socket.async_read_some(boost::asio::buffer(this->_buffer),
		boost::bind(
			&w_tcp_client::_on_received,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void w_tcp_client::_on_received(const boost::system::error_code& pErrorCode, size_t pBytesTransferred)
{
	if (pErrorCode)
	{
		this->_error_code = pErrorCode;
	}
	if (_on_received_callback)
	{
		std::string _msg = this->_buffer;
		(*this->_on_received_callback)(pErrorCode, pBytesTransferred, _msg);
	}
}

void w_tcp_client::close()
{
	if (this->_socket.available() && this->_socket.is_open())
	{
		this->_socket.close();
	}
	unregister_all_callbacks();
}

void w_tcp_client::close(boost::system::error_code& pErrorCode)
{
	if (this->_socket.available() && this->_socket.is_open())
	{
		this->_socket.close(pErrorCode);
	}
	unregister_all_callbacks();
}

void w_tcp_client::register_on_connect_callback(boost::function<void(const boost::system::error_code&)> pCallBack)
{
	if (!this->_on_connect_callback)
	{
		this->_on_connect_callback = new boost::signal1<void, const boost::system::error_code&>();
	}
	if (!this->_on_connect_callback->empty())
	{
		this->_on_connect_callback->disconnect_all_slots();
	}
	this->_on_connect_callback->connect(pCallBack);
}

void w_tcp_client::register_on_sent_callback(boost::function<void(const boost::system::error_code&, const size_t)> pCallBack)
{
	if (!this->_on_sent_callback)
	{
		this->_on_sent_callback = new boost::signal2<void, const boost::system::error_code&, const size_t>();
	}
	if (!this->_on_sent_callback->empty())
	{
		this->_on_sent_callback->disconnect_all_slots();
	}
	this->_on_sent_callback->connect(pCallBack);
}

void w_tcp_client::register_on_received_callback(boost::function<void(const boost::system::error_code&, const size_t, std::string)> pCallBack)
{
	if (!this->_on_received_callback)
	{
		this->_on_received_callback = new boost::signal3<void, const boost::system::error_code&, const size_t, std::string>();
	}
	if (!this->_on_received_callback->empty())
	{
		this->_on_received_callback->disconnect_all_slots();
	}
	this->_on_received_callback->connect(pCallBack);
}

void w_tcp_client::unregister_all_callbacks()
{
	if (this->_on_connect_callback && !this->_on_connect_callback->empty())
	{
		this->_on_connect_callback->disconnect_all_slots();
	}
	if (this->_on_sent_callback && !this->_on_sent_callback->empty())
	{
		this->_on_sent_callback->disconnect_all_slots();
	}
	if (this->_on_received_callback && !this->_on_received_callback->empty())
	{
		this->_on_received_callback->disconnect_all_slots();
	}
}
