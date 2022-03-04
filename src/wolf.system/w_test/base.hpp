#pragma once

#include <wolf.h>
#include "catch.hpp"
#include <iostream>

inline w_mem_pool w_init()
{
	w_mem_pool _mem_pool = nullptr;

	W_RESULT _ret = wolf_init();
	REQUIRE(_ret == W_SUCCESS);

	_ret = w_mem_pool_init(&_mem_pool);
	REQUIRE(_ret == W_SUCCESS);

	return _mem_pool;
}

inline void w_fini(w_mem_pool* pMemPool)
{
	w_mem_pool_fini(pMemPool);
	wolf_fini();
}