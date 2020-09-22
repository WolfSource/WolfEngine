#include <wolf.h>
#include <net/w_net.h>

#define ENDPOINT "tcp://127.0.0.1:5554"

int main()
{
	W_RESULT  _ret = wolf_init();
	if (_ret)
	{
		printf("could not initialize wolf");
		return EXIT_FAILURE;
	}

	_ret = w_net_init();
	if (_ret)
	{
		printf("could not initialize w_net");
		goto exit;
	}

	w_mem_pool _mem_pool = NULL;
	_ret = w_mem_pool_init(&_mem_pool);
	if (_ret)
	{
		printf("could not initialize memory pool");
		goto exit;
	}

	//create tcp server
	{
		w_socket_tcp _tcp_socket = { 0 };
		_ret = w_net_open_tcp_socket(
			_mem_pool,
			ENDPOINT,
			two_way_listener,
			false, //no delay
			false, //keep alive
			true,//async
			false, //tls
			0, //oauth mode
			NULL, //tls server name
			false, //own certificate
			NULL, // crt file path
			NULL, // key file path
			&_tcp_socket);

		w_buffer_t _buf;
		_buf.data = w_malloc(_mem_pool, 1024);
		_ret = w_net_receive_msg_tcp(&_tcp_socket, &_buf);
		if (_ret == W_SUCCESS)
		{
			printf("%llu bytes recieved via tcp socekt", _buf.len);
		}

		_ret = w_net_free_msg(&_buf);
		if (_ret)
		{
			printf("could not free buffer");
		}

		_ret = w_net_close_tcp_socket(&_tcp_socket);
		if (_ret)
		{
			printf("could not close tcp socket");
		}
	}

exit:
	wolf_fini();

	return EXIT_SUCCESS;
}
