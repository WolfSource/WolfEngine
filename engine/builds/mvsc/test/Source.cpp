#include <wolf.h>
#include<net/w_net.h>

int main() {
	wolf_initialize();

	W_RESULT  _net_init = w_net_init();

	const char* pEndPoint_1 = "tcp://127.0.0.1:5554";
	
	w_socket_tcp pSocket;
	pSocket.s = (w_socket)w_malloc(sizeof(w_socket), "");
	pSocket.l = (w_listener)w_malloc(sizeof(w_listener), "");
	pSocket.d = (w_dialer)w_malloc(sizeof(w_dialer), "");

	W_RESULT net_open_tcp_socket11 = w_net_open_tcp_socket(pEndPoint_1,
		two_way_listener, false,
		false, true,
		false,
		0,
		NULL,
		false,
		NULL,
		NULL,
		&pSocket);
	
	char *buf;
	size_t   sz;
	W_RESULT _net_receive_msg_tcp11 = w_net_receive_msg_tcp(&pSocket,&buf, &sz);
	W_RESULT _net_close_tcp_socket1 = w_net_close_tcp_socket(&pSocket);	
	wolf_terminate();
	return 0;
}