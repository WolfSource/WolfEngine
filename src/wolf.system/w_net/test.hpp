#pragma once

#include "../w_test/base.hpp"

#include "w_net.h"
#include "../w_concurrency/w_thread.h"

TEST_CASE("w_net/http")
{
	//TODO: TEST it again

	w_mem_pool _mem_pool = w_init();
	
	w_net_run_http1_1_server(
		false,
		nullptr,
		nullptr,
		nullptr,
		nullptr, //will use current path as the default root
		"/*",
		80,
		[](void* pSocket, const char* pRoot, const int pPort)
		{
			std::cout << "listening on port " << pPort << std::endl;
		});

	w_fini(&_mem_pool);
}
