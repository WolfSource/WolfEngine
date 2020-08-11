#include <wolf.h>
#include <net/w_net.h>



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
	W_RESULT _net_resolve = w_net_resolve(pURL, pPort, IPV4, 0, pSocketAddress);

	udp_socket.sa = (w_socket_address)w_malloc(sizeof(w_socket_address), "");
	udp_socket.a = (w_aio)w_malloc(sizeof(w_aio), "");
	udp_socket.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "");

	
	const char* endpoint = "udp://127.0.0.1:5555";
	W_RESULT net_open_udp_socket = w_net_open_udp_socket(endpoint, &udp_socket);
	
	const char* pEndPoint_1 = "tcp://127.0.0.1:5554";
	
	w_socket_tcp pSocket;
	pSocket.s = (w_socket)w_malloc(sizeof(w_socket), "");
	pSocket.l = (w_listener)w_malloc(sizeof(w_listener), "");
	pSocket.d = (w_dialer)w_malloc(sizeof(w_dialer), "");

	W_RESULT net_open_tcp_socket = w_net_open_tcp_socket(pEndPoint_1,
		two_way_dialer, false,
		false, true,
		false,
		0,
		NULL,
		false,
		NULL,
		NULL,
		&pSocket);

	    int request_nbr=0;

	    char buffer[10];
		char buf_sender_message[256];
		const char* s = "|";
		const char* s1 = "ping";
		snprintf(buf_sender_message, sizeof buf_sender_message, "%d%s%s", request_nbr, s, s1);

		const char* sender_message = buf_sender_message;
		void* mm = (void*)sender_message;
		
   W_RESULT _net_send_msg_tcp = w_net_send_msg_tcp(&pSocket, mm, strlen(mm), false);
	
   W_RESULT _net_close_tcp_socket = w_net_close_tcp_socket(&pSocket);
	


   wolf_terminate();
   return 0;
}
