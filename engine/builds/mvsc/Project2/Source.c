


#include <wolf.h>
#include<net/w_net.h>
#include<concurrency/w_thread.h>
void pOnListened(int arg1) { printf("%s", "ok_listen"); }
void pOnOpened(void) { printf("%s", "ok_open"); }
void pOnClosed(const char * arg1,int arg2) { printf("%s", "ok_close"); }
const char* pOnMessage(const char* arg1, int* arg2) {
	char* x = "ok_message";
	printf("%s", x);
}




int main() {
	wolf_initialize();
	W_RESULT  _net_init = w_net_init();

	

	//char* _addr_port_2 = "udp://127.0.0.1:8888";

	//w_socket_udp _recv_udp_sock_addr_2, to;
	//w_socket_address _addr_2 = (w_socket_address)w_malloc(sizeof(w_socket_address), "create w_socket_address");

	//_recv_udp_sock_addr_2.sa = _addr_2;
	//_recv_udp_sock_addr_2.a = (w_aio)w_malloc(sizeof(w_aio), "create w_aio");
	//_recv_udp_sock_addr_2.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "create w_udp_protocol");
	//_recv_udp_sock_addr_2.i = (w_iov)w_malloc(sizeof(w_iov), "create w_iov");

	//if (w_net_open_udp_socket(_addr_port_2, &_recv_udp_sock_addr_2) == W_FAILURE)
	//{
	//	printf(" can not connect to address udp connection");

	//	return NULL;
	//}

	//w_socket_address _recv_addr = (w_socket_address)w_malloc(sizeof(w_socket_address), "create w_socket_address");

	//char* _recv_address = "udp://127.0.0.1:8889";

	//to.sa = _recv_addr;
	//to.a = (w_aio)w_malloc(sizeof(w_aio), "create w_aio");
	//to.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "create w_udp_protocol");
	//to.i = (w_iov)w_malloc(sizeof(w_iov), "create w_iov");

	//char buf2[] = "hello from playpod";
	//size_t size_2 = strlen(buf2);

	//w_net_send_msg_udp(_recv_address, &to, buf2, size_2);
	ws_on_listened_fn _listen = &pOnListened;
	ws_on_opened_fn _open = &pOnOpened;
	ws_on_closed_fn _close = &pOnClosed;
	ws_on_message_fn _message = &pOnMessage;
	W_RESULT _net_run_websocket_server =w_net_run_websocket_server(false,
		NULL,
		NULL,
		NULL,
		"/*",
		5555,
		16,
		16*1024,
		120,
		1*1024*1024,
		_listen,
		_open,
		_message,
		_close);








	/*const char* pURL = "http//127.0.0.1:5555";
	w_array pHttpHeaders = "Accept: application/json";
	char pMessage[] = "hello";
	size_t pMessageLenght = strlen(pMessage);
	

	size_t pLowSpeedLimit = 12;
	size_t pLowSpeedTimeInSec = 30000;
	long pResponseCode = 2;
	float pTimeOutInSecs = 0.00003;
	char* pResponseMessage = "ok";
	size_t pResponseMessageLength = strlen(pResponseMessage);
	W_RESULT _net_send_http_request = w_net_send_http_request(pURL,
		HTTP_GET,
		pHttpHeaders,
		pMessage,
		pMessageLenght,
		pLowSpeedLimit,
		pLowSpeedTimeInSec,
		pTimeOutInSecs,
		&pResponseCode,
		&pResponseMessage,
		&pResponseMessageLength);*/

	wolf_terminate();
	return 0;
}