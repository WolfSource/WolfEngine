/*
    Project: Wolf Engine. Copyright � 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
#ifdef WOLF_SYSTEM_EXECUTION

#pragma once

// NOLINTBEGIN
#include <unifex/coroutine.hpp>
#include <unifex/static_thread_pool.hpp>
#include <unifex/sync_wait.hpp>
#include <unifex/task.hpp>
#include <unifex/then.hpp>

namespace wolf::system {

/**
 * returns a task from function
 * @param p_scheduler, the scheduler to use for the task
 * @param p_func, the function to call
 * @param p_func_args, the argumans for the function
 * @return coroutine task
 */
// UNIFEX_TEMPLATE(typename w_scheduler)
template <typename R, typename w_scheduler, typename F, typename... Args>
requires unifex::scheduler<w_scheduler>
auto w_task(w_scheduler p_scheduler, F p_func, Args... p_func_args)
    -> unifex::task<R> {
  co_await unifex::schedule(p_scheduler);
  co_return p_func(std::forward<Args>(p_func_args)...);
}

} // namespace wolf::system

// NOLINTEND

#endif
