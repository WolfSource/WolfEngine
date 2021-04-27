#pragma once

#include "../w_test/base.hpp"

#include "w_net.h"
#include "../w_io/w_io.h"
#include "../w_concurrency/w_thread_pool.h"
#include "../w_concurrency/w_condition_variable.h"

static w_condition_variable s_cv = nullptr;

static void s_create_tls_1_3_client(w_mem_pool pMemPool)
{
	std::cout << "s_create_tls_1_3_client is going to start" << std::endl;

	const char* _crt_path = "D:\\SourceCodes\\PooyaEimandar\\wolf.streamer.deps\\playpod_certificate\\playpod.crt";
	
	W_RESULT _hr = W_SUCCESS;

	w_socket_options _socekt_option =
	{
		3000000, //timeout_microseconds
		false, //non_blocking
		true, //no delay
		true //keep alive
	};

	//open ssl socket anc connect to server at "127.0.0.1:1111"
	w_ssl_socket _ssl_socket = nullptr;
	_hr = w_net_ssl_socket_open(
		pMemPool,
		"127.0.0.1",
		1111,
		_crt_path,
		nullptr,
		w_socket_protocol::W_SOCKET_PROTOCOL_TCP_DIALER,
		w_socket_type::W_SOCKET_TYPE_STREAM,
		w_socket_family::W_SOCKET_FAMILY_IPV4,
		&_socekt_option,
		&_ssl_socket);
	REQUIRE(_hr == W_SUCCESS);

	//create a buffer
	w_buffer_t _buffer = { 0 };
	_buffer.len = 1024;
	_buffer.data = (uint8_t*)w_malloc(pMemPool, _buffer.len);

	//read from server
	auto _read_bytes = w_net_ssl_socket_read(_ssl_socket, &_buffer);
	REQUIRE(_read_bytes > 0);

	auto _sent_bytes = w_net_ssl_socket_send(_ssl_socket, &_buffer);
	REQUIRE(_sent_bytes > 0);

	REQUIRE(_sent_bytes == _read_bytes);

	_hr = w_net_ssl_socket_close(&_ssl_socket);
	REQUIRE(_hr == W_SUCCESS);

}

static void s_create_tls_1_3_server(w_mem_pool pMemPool)
{
	std::cout << "s_create_tls_1_3_server is going to start" << std::endl;

	const char* _crt_path = "D:\\SourceCodes\\PooyaEimandar\\wolf.streamer.deps\\playpod_certificate\\playpod.crt";
	const char* _pkey_path = "D:\\SourceCodes\\PooyaEimandar\\wolf.streamer.deps\\playpod_certificate\\playpod.key";

	W_RESULT _hr = W_SUCCESS;

	w_socket_options _socekt_option =
	{
		3000000, //timeout_microseconds
		false, //non_blocking
		true, //no delay
		true //keep alive
	};

	//open ssl socket
	w_ssl_socket _ssl_socket = nullptr;
	_hr = w_net_ssl_socket_open(
		pMemPool,
		"127.0.0.1",
		1111,
		_crt_path,
		_pkey_path,
		w_socket_protocol::W_SOCKET_PROTOCOL_TCP_LISTENER,
		w_socket_type::W_SOCKET_TYPE_STREAM,
		w_socket_family::W_SOCKET_FAMILY_IPV4,
		&_socekt_option,
		&_ssl_socket);
	REQUIRE(_hr == W_SUCCESS);

	_hr = w_condition_variable_init(pMemPool, &s_cv);
	REQUIRE(_hr == W_SUCCESS);

	auto _ssl_accept_callback = [](w_thread pThread, void* pArg)-> void*
	{
		std::cout << "ssl accepted" << std::endl;

		auto _ssl_socket = (w_ssl_socket)pArg;
		if (!_ssl_socket) return nullptr;
	
		w_mem_pool _mem_pool = nullptr;
		if (w_mem_pool_init(&_mem_pool) != W_SUCCESS)
		{
			return nullptr;
		}
		
		//write
		const char* _hello_msg = "hello";
		
		w_buffer_t _buffer = { 0 };
		_buffer.len = strlen(_hello_msg); //plus NULL
		_buffer.data = (uint8_t*)w_malloc(_mem_pool, _buffer.len);
		memcpy(_buffer.data, _hello_msg, _buffer.len);
		_buffer.data[_buffer.len] = '\0';

		auto _sent_bytes = w_net_ssl_socket_send(_ssl_socket, &_buffer);
		REQUIRE(_sent_bytes > 0);

		auto _read_bytes = w_net_ssl_socket_read(_ssl_socket, &_buffer);
		REQUIRE(_read_bytes > 0);

		REQUIRE(_read_bytes == _sent_bytes);

		w_mem_pool_fini(&_mem_pool);		
		w_condition_variable_signal(s_cv);

		return nullptr;
	};

	//accept ssl socket
	_hr = w_net_ssl_socket_accept(
		pMemPool,
		_ssl_socket,
		&_socekt_option,
		true,
		_ssl_accept_callback);
	REQUIRE(_hr == W_SUCCESS);


	//wait for condition variable
	_hr = w_condition_variable_wait(s_cv);
	REQUIRE(_hr == W_SUCCESS);

	w_condition_variable_fini(&s_cv);

	_hr = w_net_ssl_socket_close(&_ssl_socket);
	REQUIRE(_hr == W_SUCCESS);
}

TEST_CASE("w_net/http")
{
	w_mem_pool _mem_pool = w_init();
	
#ifdef WOLF_ENABLE_SSL

	constexpr int _number_of_max_threads = 2;
	w_thread_pool _tp = nullptr;
	REQUIRE(
		w_thread_pool_init(
			_mem_pool, 
			&_tp, 
			_number_of_max_threads, _number_of_max_threads) == W_SUCCESS);


	//start server immediately
	auto _hr = w_thread_pool_schedule(
		_tp,
		[](w_thread pThread, void* pArg) -> void*
		{
			auto _mem_pool = (w_mem_pool)pArg;
			if (_mem_pool)
			{
				s_create_tls_1_3_server(_mem_pool);
			}
			return nullptr;

		}, _mem_pool, 0, nullptr);
	REQUIRE(_hr == W_SUCCESS);

	int64_t  _start_after_microsecs = 3 * 1000 * 1000;
	_hr = w_thread_pool_schedule(
		_tp,
		[](w_thread pThread, void* pArg) -> void*
		{
			auto _mem_pool = (w_mem_pool)pArg;
			if (_mem_pool)
			{
				s_create_tls_1_3_client(_mem_pool);
			}
			return nullptr;

		}, _mem_pool, _start_after_microsecs, nullptr);
	REQUIRE(_hr == W_SUCCESS);

	w_thread_pool_wait_all(_tp);

	//wait for all threads
	REQUIRE(w_thread_pool_fini(&_tp) == W_SUCCESS);

#endif

#ifdef WOLF_ENABLE_HTTP1_1_WS

	w_net_run_http1_1_server(
		false,
		nullptr,
		nullptr,
		nullptr,
		nullptr, //will use current path as the default root
		"/*",
		80,
		[](void* pSocket, const char* pRoot, const int pPort)
		{
			std::cout << "listening on port " << pPort << std::endl;
		});
#endif

	w_fini(&_mem_pool);
}
