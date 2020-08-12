#include <wolf.h>
#include<net/w_net.h>

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

	const char* pUrlAddress = "http://127.0.0.1:5555";
	const char* net_url_encoded = w_net_url_encoded(pUrlAddress);

	w_url* pURL_1 = w_malloc(sizeof(w_url), "");
	W_RESULT _net_url_parse = w_net_url_parse(pUrlAddress, &pURL_1);

	w_socket_udp udp_socket;
	const char pURL[] = "localhost";
	const char pPort[] = "5555";
	w_socket_address pSocketAddress = (w_socket_address)w_malloc(sizeof(w_socket_address), "");
	int pBindOrConnect = 0;
	W_RESULT _net_resolve = w_net_resolve(pURL, pPort, IPV4, 1, pSocketAddress);

	udp_socket.sa = (w_socket_address)w_malloc(sizeof(w_socket_address), "");
	udp_socket.a = (w_aio)w_malloc(sizeof(w_aio), "");
	udp_socket.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "");

	char* pMessage_udp = "yes";
	size_t pMessageLength_udp = strlen(pMessage_udp);
	const char* endpoint = "udp://127.0.0.1:5555";
	W_RESULT net_open_udp_socket = w_net_open_udp_socket(endpoint, &udp_socket);

	const char* pEndPoint_1 = "tcp://127.0.0.1:5555";
	const char* pTLSServerName = "127.0.0.1";
	w_socket_tcp pSocket;
	pSocket.s = (w_socket)w_malloc(sizeof(w_socket), "");
	pSocket.l = (w_listener)w_malloc(sizeof(w_listener), "");
	pSocket.d = (w_dialer)w_malloc(sizeof(w_dialer), "");

	W_RESULT net_open_tcp_socket = w_net_open_tcp_socket(pEndPoint_1,
		req_rep_listener, false,
		false, false,
		false,
		0,
		pTLSServerName,
		false,
		"",
		"",
		&pSocket);


	char pMessage[] = "yes";
	size_t pMessageLength = strlen(pMessage);
	W_RESULT _net_send_msg_tcp = w_net_send_msg_tcp(&pSocket, pMessage, pMessageLength, true);

	W_RESULT _net_close_tcp_socket = w_net_close_tcp_socket(&pSocket);


	const char* _net_erro = w_net_error(_net_send_msg_tcp);


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
	
	
	w_array y=w_malloc(sizeof(w_array),"");
		y->elt_size = 1;
		y->nelts = 1;
		y->nalloc = 1;
		y->elts = "Accept:application/json";
		y->pool =w_get_default_memory_pool() ;
		
	size_t pLowSpeedLimit = 12;
	size_t pLowSpeedTimeInSec = 30000;
	long pResponseCode = 2;
	float pTimeOutInSecs = 0.00003;
	char* pResponseMessage = "ok";
	size_t pResponseMessageLength = strlen(pResponseMessage);
	W_RESULT _net_send_http_request = w_net_send_http_request(pURL,
		HTTP_GET,
		y,
		pMessage,
		pMessageLenght,
		pLowSpeedLimit,
		pLowSpeedTimeInSec,
		pTimeOutInSecs,
		&pResponseCode,
		&pResponseMessage,
		&pResponseMessageLength);


	wolf_terminate();
	return 0;
}
