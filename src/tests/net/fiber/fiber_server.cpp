#include <iostream>
#include <wolf.h>
#include <net/w_net.h>
#include <log/w_log.h>

W_RESULT s_fiber_server_receive_callback_fn(_In_ void* pSocket, _In_ w_buffer pReceivedBuffer)
{
	if (pReceivedBuffer)
	{
		std::string _str((char*)pReceivedBuffer->data, pReceivedBuffer->len);
		std::cout << "data recieved" << _str << std::endl;
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

	if (w_net_fiber_server_run(
		w_socket_family::W_SOCKET_FAMILY_IPV4,
		8888,
		&_server_id,
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