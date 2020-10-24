#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include<string>
	typedef void (*w_call_amq)(
		std::string pMessage);

	W_SYSTEM_EXPORT
	void w_amq_init();

	W_SYSTEM_EXPORT
	void w_amq_fini();

	W_SYSTEM_EXPORT
	void w_net_amq_open_consumer(w_call_amq pCallback_tmp);

	W_SYSTEM_EXPORT
	void w_net_send_producer(std::string pMessage, int _priority);

	W_SYSTEM_EXPORT
	void w_net_open_producer();



#ifdef __cplusplus
}
#endif
