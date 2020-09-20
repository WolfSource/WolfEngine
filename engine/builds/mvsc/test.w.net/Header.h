#include <wolf.h>

#include <time.h>

#include <concurrency/w_thread.h>
#include <concurrency/w_thread_pool.h>

#include <io/w_io.h>
#include <time.h>
#include <memory/w_table.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <net/w_net.h>
void x() {
	w_socket_udp _recv_udp_sock_addr2;
	w_socket_address _addr2 = (w_socket_address)w_malloc(sizeof(w_socket_address), "create w_socket_address");

	_recv_udp_sock_addr2.sa = _addr2;
	_recv_udp_sock_addr2.a = (w_aio)w_malloc(sizeof(w_aio), "create w_aio");
	_recv_udp_sock_addr2.u = (w_udp_protocol)w_malloc(sizeof(w_udp_protocol), "create w_udp_protocol");
	_recv_udp_sock_addr2.i = (w_iov)w_malloc(sizeof(w_iov), "create w_iov");
	const char* endpoint_1 = "udp://127.0.0.1:5554";
}