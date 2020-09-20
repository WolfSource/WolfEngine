
#include <wolf.h>
#include<net/w_net.h>
#include<concurrency/w_thread.h>

void* send_job(w_thread pThread, void* pArgs)
{
	char * _addr_port = "udp://127.0.0.1:8889";

	w_socket_udp _recv_udp_sock_addr, to;
	w_socket_address _addr = (w_socket_address)w_malloc(sizeof(w_socket_address), "create w_socket_address");

	_recv_udp_sock_addr.sa = _addr;
	_recv_udp_sock_addr.a = (w_aio)w_malloc(sizeof(w_aio), "create w_aio");
	_recv_udp_sock_addr.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "create w_udp_protocol");
	_recv_udp_sock_addr.i = (w_iov)w_malloc(sizeof(w_iov), "create w_iov");

	if (w_net_open_udp_socket(_addr_port, &_recv_udp_sock_addr) == W_FAILURE)
	{
		printf(" can not connect to address udp connection");
	
		return NULL;
	}

	w_socket_address _recv_addr = (w_socket_address)w_malloc(sizeof(w_socket_address), "create w_socket_address");

	char*  _recv_address = "udp://127.0.0.1:8888";

	to.sa = _recv_addr;
	to.a = (w_aio)w_malloc(sizeof(w_aio), "create w_aio");
	to.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "create w_udp_protocol");
	to.i = (w_iov)w_malloc(sizeof(w_iov), "create w_iov");

	char buf[] = "hello from playpod";
	size_t size = 1024;

	w_net_send_msg_udp(_recv_address, &to, buf, size);
}

void* recv_job(w_thread pThread, void* pArgs)
{
	char*  _addr_port = "udp://127.0.0.1:8888";

	w_socket_udp _recv_udp_sock_addr, from;
	w_socket_address _addr = (w_socket_address)w_malloc(sizeof(w_socket_address), "create w_socket_address");

	_recv_udp_sock_addr.sa = _addr;
	_recv_udp_sock_addr.a = (w_aio)w_malloc(sizeof(w_aio), "stream_manager::create w_aio");
	_recv_udp_sock_addr.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "stream_manager::create w_udp_protocol");
	_recv_udp_sock_addr.i = (w_iov)w_malloc(sizeof(w_iov), "stream_manager::create w_iov");

	if (w_net_open_udp_socket(_addr_port, &_recv_udp_sock_addr) == W_FAILURE)
	{
		printf("can not connect to address udp connection");
		
		return NULL;
	}

	from.sa = (w_socket_address)w_malloc(sizeof(w_socket_address), "stream_manager::create w_socket_address");
	from.a = (w_aio)w_malloc(sizeof(w_aio), "stream_manager::create w_aio");
	from.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "stream_manager::create w_udp_protocol");
	from.i = (w_iov)w_malloc(sizeof(w_iov), "stream_manager::create w_iov");

	char buf[1024];
	size_t size = 1024;
	w_net_receive_msg_udp(&from, buf, &size);
}
int main() {
	wolf_initialize();
	W_RESULT  _net_init = w_net_init();

	w_thread send_thread, recv_thread;
	  


	/*w_thread_create(&recv_thread, &recv_job, NULL);

	w_thread_create(&send_thread, &send_job, NULL);
	while (1) {
		Sleep(500);
	}*/
//	char* _addr_port = "udp://127.0.0.1:8889";
//w_socket_udp _recv_udp_sock_addr, from;
//	w_socket_address _addr = (w_socket_address)w_malloc(sizeof(w_socket_address), "create w_socket_address");
//
//	_recv_udp_sock_addr.sa = _addr;
//	_recv_udp_sock_addr.a = (w_aio)w_malloc(sizeof(w_aio), "stream_manager::create w_aio");
//	_recv_udp_sock_addr.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "stream_manager::create w_udp_protocol");
//	_recv_udp_sock_addr.i = (w_iov)w_malloc(sizeof(w_iov), "stream_manager::create w_iov");
//
//	if (w_net_open_udp_socket(_addr_port, &_recv_udp_sock_addr) == W_FAILURE)
//	{
//		printf("can not connect to address udp connection");
//
//		return NULL;
//	}
//
//	from.sa = (w_socket_address)w_malloc(sizeof(w_socket_address), "stream_manager::create w_socket_address");
//	from.a = (w_aio)w_malloc(sizeof(w_aio), "stream_manager::create w_aio");
//	from.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "stream_manager::create w_udp_protocol");
//	from.i = (w_iov)w_malloc(sizeof(w_iov), "stream_manager::create w_iov");
//
//	char buf[1024];
//	size_t size = 1024;
//	w_net_receive_msg_udp(&from, buf, &size);
	


	const char* pURL = "http://127.0.0.1:5555";
	//char  *pHttpHeaders = "Accept:application/json";
	char* pMessage = NULL;
	size_t pMessageLenght = 0;

	/*struct x {
		w_mem_pool pool;
		int 	elt_size;
		int 	nelts;
		int 	nalloc;
		char* elts;
	};*/
	// w_array y;
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