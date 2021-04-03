#pragma once

#ifdef WOLF_ENABLE_FIBER

#include "../w_test/base.hpp"

TEST_CASE("w_fiber/w_fiber")
{
	w_mem_pool _mem_pool = w_init();
	w_fiber _fiber[2] = { nullptr, nullptr };

	for (size_t i = 0; i < 2; ++i)
	{
		auto _ret = w_fiber_init(
			_mem_pool,
			&_fiber[i],
			[](void* pArgs)
			{
				auto _id = w_thread_get_current_id();
				std::cout << "fiber started current thread id: " << _id << " just started" << std::endl;
				w_fiber_current_yield();
				std::cout << "fiber started current thread id: " << _id << " just started" << std::endl;
			}, (void*)&i);
		REQUIRE(_ret == W_SUCCESS);
		REQUIRE(_fiber[i]);
		REQUIRE(w_fiber_detach(_fiber[i]) == W_SUCCESS);
	}

	for (size_t i = 0; i < 2; ++i)
	{
		REQUIRE(w_fiber_join(_fiber[i]) == W_SUCCESS);
		REQUIRE(w_fiber_fini(&_fiber[i]) == W_SUCCESS);
	}
	w_fini(&_mem_pool);
}

#endif