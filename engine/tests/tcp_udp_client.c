#include <wolf.h>
#include <net/w_net.h>

void pOnListened(int arg1) { printf("%s", "ok_listen"); }
void pOnOpened(void) { printf("%s", "ok_open"); }
void pOnClosed(const char * arg1,int arg2) { printf("%s", "ok_close"); }

const char* pOnMessage(const char* arg1, int* arg2) 
{
	char* x = "ok_message";
	printf("%s", x);
	return NULL;
}

int main()
{
	W_RESULT _ret = wolf_init();
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

	//encode & decode url
	{
		const char* _url_address = "http://127.0.0.1:5555";
		const char* _url_encoded = w_net_url_encoded(_mem_pool, _url_address);

		w_url _url = NULL;
		_ret = w_net_url_parse(
			_mem_pool,
			_url_address,
			&_url);
		if (_ret)
		{
			printf("could not parse url");
			goto exit;
		}
	}

	//tcp client
	{
		const char* _endpoint = "tcp://127.0.0.1:5554";

		w_socket_tcp _socket = { 0 };
		_ret = w_net_open_tcp_socket(
			_mem_pool,
			_endpoint,
			two_way_dialer,
			false,
			false,
			true,
			false,
			0,
			NULL,
			false,
			NULL,
			NULL,
			&_socket);
		if (_ret)
		{
			printf("could not open tcp socket for client");
			goto exit;
		}

		const char* _msg = "hello";
		size_t _msg_len = strlen(_msg);

		w_buffer_t _buf = { 0 };
		_buf.len = _msg_len;
		_buf.data = (uint8_t*)w_malloc(_mem_pool, _msg_len);
		memcpy(_buf.data, _msg, _buf.len);
		_buf.data[_msg_len] = '\0';

		_ret = w_net_send_msg_tcp(&_socket, &_buf, false);
		if (_ret == W_SUCCESS)
		{
			printf("%llu bytes sent via tcp socekt", _buf.len);
		}

		_ret = w_net_close_tcp_socket(&_socket);
		if (_ret)
		{
			printf("could not close tcp socket for client");
		}
	}

	//udp client
	{

	}

	//	w_socket_udp udp_socket;
	//	const char pURL[] = "localhost";
	//	const char pPort[] = "5555";
	//	w_socket_address pSocketAddress = (w_socket_address)w_malloc(sizeof(w_socket_address), "");
	//	int pBindOrConnect = 0;
	//	W_RESULT _net_resolve = w_net_resolve(pURL, pPort, IPV4, 0, pSocketAddress);

	//	udp_socket.sa = (w_socket_address)w_malloc(sizeof(w_socket_address), "");
	//	udp_socket.a = (w_aio)w_malloc(sizeof(w_aio), "");
	//	udp_socket.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "");

	//const char* endpoint = "udp://127.0.0.1:5555";
	//W_RESULT net_open_udp_socket = w_net_open_udp_socket(endpoint, &udp_socket);


	//int request_nbr = 0;

	//char buffer[10];
	//char buf_sender_message[256];
	//const char* s = "|";
	//const char* s1 = "ping";
	//snprintf(buf_sender_message, sizeof buf_sender_message, "%d%s%s", request_nbr, s, s1);

	//ws_on_listened_fn _listen = &pOnListened;
	//ws_on_opened_fn _open = &pOnOpened;
	//ws_on_closed_fn _close = &pOnClosed;
	//ws_on_message_fn _message = &pOnMessage;
	//W_RESULT _net_run_websocket_server = w_net_run_websocket_server(false,
	//	NULL,
	//	NULL,
	//	NULL,
	//	"/*",
	//	5555,
	//	16,
	//	16 * 1024,
	//	120,
	//	1 * 1024 * 1024,
	//	_listen,
	//	_open,
	//	_message,
	//	_close);


	//w_array y = w_malloc(sizeof(w_array), "");
	//y->elt_size = 1;
	//y->nelts = 1;
	//y->nalloc = 1;
	//y->elts = "Accept:application/json";
	//y->pool = w_get_default_memory_pool();

	//size_t pLowSpeedLimit = 12;
	//size_t pLowSpeedTimeInSec = 30000;
	//long pResponseCode = 2;
	//float pTimeOutInSecs = 0.00003;
	//char* pResponseMessage = "ok";
	//size_t pResponseMessageLength = strlen(pResponseMessage);
	//W_RESULT _net_send_http_request = w_net_send_http_request(pURL,
	//	HTTP_GET,
	//	y,
	//	pMessage,
	//	pMessageLenght,
	//	pLowSpeedLimit,
	//	pLowSpeedTimeInSec,
	//	pTimeOutInSecs,
	//	&pResponseCode,
	//	&pResponseMessage,
	//	&pResponseMessageLength);

	//const char* sender_message = buf_sender_message;
	//void* mm = (void*)sender_message;

	//W_RESULT _net_send_msg_tcp = w_net_send_msg_tcp(&pSocket, mm, strlen(mm), false);

	//W_RESULT _net_close_tcp_socket = w_net_close_tcp_socket(&pSocket);*/


exit:
	wolf_fini();
	return 0;
}
