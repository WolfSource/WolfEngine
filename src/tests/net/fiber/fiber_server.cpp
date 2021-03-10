#include <iostream>
#include <wolf.h>
#include <net/w_net.h>
#include <log/w_log.h>
#include <io/w_io.h>
#include <chrono>
#include <thread>

W_RESULT s_fiber_server_receive_callback_fn(
	_In_ void* pSocket,
	_In_ w_buffer pReceivedBuffer,
	_In_z_ const char* pThreadFiberInfo)
{
	if (pReceivedBuffer)
	{
		std::string _str((char*)pReceivedBuffer->data, pReceivedBuffer->len);
		LOG_P(W_LOG_INFO, "data recieved: %s with thread fiber info: %s", _str.c_str(), pThreadFiberInfo);
	}

	return w_net_fiber_async_write(pSocket, pReceivedBuffer);
}

int main()
{
	auto _server_id = new int();

	if (wolf_init() != W_SUCCESS)
	{
		wolf_fini();
		return W_FAILURE;
	}

	w_mem_pool _mem_pool = nullptr;
	w_mem_pool_init(&_mem_pool);
	if (!_mem_pool)
	{
		wolf_fini();
		return W_FAILURE;
	}

	char* _path = nullptr;
	w_io_dir_get_current_exe(_mem_pool, &_path);
	if (!_path)
	{
		wolf_fini();
		return W_FAILURE;
	}
	w_log_config _c = {};
	_c.app_name = "fiber_server";
	_c.log_to_std_out = true;
	_c.log_dir_path = _path;

	if (w_log_init(_mem_pool, &_c) != W_SUCCESS)
	{
		wolf_fini();
		return W_FAILURE;
	}

	constexpr auto _port = 7777;
	LOG_P(W_LOG_DEBUG, "starting fiber server on port: %d", _port);

	std::thread _t([&]
		{
			if (w_net_fiber_server_run(
				w_socket_family::W_SOCKET_FAMILY_IPV4,
				_port,
				&_server_id,
				s_fiber_server_receive_callback_fn) != W_SUCCESS)
			{
				LOG_P(
					w_log_type::W_LOG_DEBUG,
					"could not start fiber server");
			}

			LOG_P(
				w_log_type::W_LOG_DEBUG,
				"thread exiting");
		});

	//wait for 10 seconds then stop the fiber server
	std::this_thread::sleep_for(std::chrono::seconds(10));
	w_net_fiber_server_stop(*_server_id);
	LOG_P(
		w_log_type::W_LOG_DEBUG,
		"fiber server stopped successfully");
	std::this_thread::sleep_for(std::chrono::seconds(5));

	//release all resources
	w_log_fini();
	wolf_fini();
	_t.join();
	return W_SUCCESS;
}