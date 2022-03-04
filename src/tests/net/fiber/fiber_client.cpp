#include <wolf.h>
#include <net/w_net.h>
#include <log/w_log.h>

W_RESULT s_fiber_server_receive_callback_fn(
	_In_ void* pSocket,
	_In_z_ const char* pThreadFiberInfo)
{
	const char* _msg = "hello";
	const auto _msg_size = strlen(_msg);

	w_buffer_t _b = { 0 };
	_b.data = (uint8_t*)malloc(_msg_size + 1);//plus null
	_b.len = _msg_size;
	_b.data[_msg_size] = '\0';

	if (_b.data)
	{
		//copy message
		memcpy(_b.data, _msg, _msg_size);
		for (size_t i = 0; i < 100; i++)
		{
			w_net_fiber_async_write(pSocket, &_b);
		}
	}

	return W_SUCCESS;
}

int main()
{
	W_RESULT _ret = W_SUCCESS;
	auto _server_id = new int();

	if (wolf_init() != W_SUCCESS)
	{
		_ret = W_FAILURE;
		goto exit;
	}

	if (w_net_fiber_clients_connect(
		w_socket_family::W_SOCKET_FAMILY_IPV4,
		"127.0.0.1",
		(uint16_t)7777,
		100,
		s_fiber_server_receive_callback_fn) != W_SUCCESS)
	{
		LOG_P(
			w_log_type::W_LOG_DEBUG,
			"could not start fiber server");
	}

exit:
	wolf_fini();
	return _ret;
}