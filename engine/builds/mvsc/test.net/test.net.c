#include <wolf.h>
#include<net/w_net.h>
void ws_open()
{ printf("%s","ok"); }
void ws_listened (int x){ printf("%s", "ok"); }
//ws_message
int main() {
	wolf_initialize();
	W_RESULT  _net_init = w_net_init();

	const char* pUrlAddress = "http://127.0.0.1:5555";
	const char* net_url_encoded = w_net_url_encoded( pUrlAddress);

	w_url* pURL_1 = w_malloc(sizeof(w_url), "");
	W_RESULT _net_url_parse = w_net_url_parse(pUrlAddress, &pURL_1);

	w_socket_udp udp_socket;
	const char pURL[] = "localhost";
	const char pPort []= "5555";
	w_socket_address pSocketAddress = (w_socket_address)w_malloc(sizeof(w_socket_address), "");
	int pBindOrConnect = 0;
	W_RESULT _net_resolve= w_net_resolve( pURL, pPort,IPV4, 1,  pSocketAddress);

	udp_socket.sa = (w_socket_address)w_malloc(sizeof(w_socket_address), "");
	udp_socket.a = (w_aio)w_malloc(sizeof(w_aio), "");
	udp_socket.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "");

	char *pMessage_udp = "yes";
	size_t pMessageLength_udp = strlen(pMessage_udp);
	const char* endpoint = "udp://127.0.0.1:5555";
	W_RESULT net_open_udp_socket = w_net_open_udp_socket(endpoint, &udp_socket);

	//udp_socket.i = (w_iov)w_malloc(sizeof(w_iov), "");

	//W_RESULT _send_msg_udp = w_net_send_msg_udp(&udp_socket,
	//	pMessage,
	//	&pMessageLength);
	
     w_net_close_udp_socket(&udp_socket);
	



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

	pSocket.s = (w_socket)w_malloc(sizeof(w_socket), "");

	char pMessage[] = "yes";
	size_t pMessageLength = strlen(pMessage);
	W_RESULT _net_send_msg_tcp = w_net_send_msg_tcp(&pSocket, pMessage, pMessageLength, true);

	W_RESULT _net_close_tcp_socket = w_net_close_tcp_socket(&pSocket);

	
	const char* _net_erro = w_net_error(_net_send_msg_tcp);

	//char* buf = NULL;
	//size_t   sz;
	//W_RESULT _receive_msg_tcp =w_net_receive_msg_tcp(&pSocket, &buf,&sz);
	/*
	ws_on_opened_fn ws_on_open = &ws_open;
	ws_on_listened_fn ws_On_Listened = &ws_listened;
	ws_on_message_fn ws_On_Messag = &ws_message;
	_In_ ws_on_message_fn pOnMessage,
	_In_ ws_on_closed_fn pOnClosed);
	W_RESULT _net_run_websocket_server =w_net_run_websocket_server(false,
		nullptr,
		nullptr,
		_In_z_ const char* pPassPhrase,
		_In_z_ const char* pRoot,
		_In_ int pPort,
		_In_ int pCompression,
		_In_ int pMaxPayloadLength,
		_In_ int pIdleTimeout,
		_In_ int pMaxBackPressure,
		ws_On_Listened,
		ws_on_open,
	 ws_on_message_fn pOnMessage,
		_In_ ws_on_closed_fn pOnClosed);*/


	int a = 2;


	wolf_terminate();
	return 0;
}