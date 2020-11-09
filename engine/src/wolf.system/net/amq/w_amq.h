#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

	typedef void (*w_call_amq)(
		const char *pMessage);

	W_SYSTEM_EXPORT
	int  w_net_init_producer(_Inout_ w_mem_pool pMemPool);

	W_SYSTEM_EXPORT
		int w_amq_init(_Inout_ w_mem_pool pMemPool);

	W_SYSTEM_EXPORT
		int w_amq_fini(_Inout_ w_mem_pool pMemPool);


	W_SYSTEM_EXPORT
		int  w_net_receive_consumer(_Inout_ w_mem_pool pMemPool, w_call_amq pCallback_tmp);

	W_SYSTEM_EXPORT
		void w_net_send_producer(char * pMessage, int _priority);

	W_SYSTEM_EXPORT
		void w_net_open_producer();

	W_SYSTEM_EXPORT
		void w_net_consumer_fini();

	W_SYSTEM_EXPORT
		void w_net_producer_fini();



#ifdef __cplusplus
}
#endif