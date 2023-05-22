#include "w_time.hpp"

using w_time = wolf::system::w_time;

boost::asio::awaitable<std::errc> w_time::timeout(
    _In_ const std::chrono::steady_clock::time_point& p_deadline) noexcept {
  using steady_timer = boost::asio::steady_timer;
  using steady_clock = std::chrono::steady_clock;

  steady_timer _timer(co_await boost::asio::this_coro::executor);
  auto _now = steady_clock::now();

#ifdef __clang__
#pragma unroll
#endif
  while (p_deadline > _now) {
    _timer.expires_at(p_deadline);
    co_await _timer.async_wait(boost::asio::use_awaitable);
    _now = steady_clock::now();
  }
  co_return std::errc::timed_out;
}