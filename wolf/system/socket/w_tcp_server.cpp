#ifdef WOLF_SYSTEM_SOCKET

#include "w_tcp_server.hpp"
#include <random>

#include "DISABLE_ANALYSIS_BEGIN"
#include <boost/asio/experimental/awaitable_operators.hpp>
#include "DISABLE_ANALYSIS_END"

using w_tcp_server = wolf::system::socket::w_tcp_server;
using w_session_on_data_callback = wolf::system::socket::w_session_on_data_callback;
using w_session_on_timeout_callback = wolf::system::socket::w_session_on_timeout_callback;
using w_session_on_error_callback = wolf::system::socket::w_session_on_error_callback;
using w_socket_options = wolf::system::socket::w_socket_options;
using steady_clock = std::chrono::steady_clock;
using steady_timer = boost::asio::steady_timer;
using io_context = boost::asio::io_context;
using tcp = boost::asio::ip::tcp;
using time_point = std::chrono::steady_clock::time_point;

using namespace boost::asio::experimental::awaitable_operators;

boost::asio::awaitable<std::errc>
watchdog(_Inout_ const time_point &p_deadline) noexcept {
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

boost::asio::awaitable<void>
on_echo(const boost::asio::io_context &p_io_context, tcp::socket &p_socket,
        const std::string &p_conn_id, time_point &p_deadline,
        steady_clock::duration p_timeout,
        const w_session_on_data_callback &p_on_data_callback,
        const w_session_on_error_callback &p_on_error_callback) noexcept {
  w_buffer _mut_buffer = {};

#ifdef __clang__
#pragma unroll
#endif
  while (!p_io_context.stopped()) {
    p_deadline = steady_clock::now() + p_timeout;

    try {
      _mut_buffer.used_bytes = co_await p_socket.async_receive(
          boost::asio::buffer(_mut_buffer.buf), boost::asio::use_awaitable);

      // call callback
      const auto _res = p_on_data_callback(p_conn_id, _mut_buffer);
      co_await p_socket.async_send(
          boost::asio::buffer(_mut_buffer.buf, _mut_buffer.used_bytes),
          boost::asio::use_awaitable);

      if (_res == boost::system::errc::connection_aborted) {
        break;
      }
    } catch (const std::exception &p_ex) {
      p_on_error_callback(p_conn_id, p_ex);
      break;
    }
  }
}

boost::asio::awaitable<void> handle_connection(
    const boost::asio::io_context &p_io_context, tcp::socket p_socket,
    steady_clock::duration p_timeout,
    w_session_on_data_callback p_on_data_callback,
    w_session_on_timeout_callback p_on_timeout_callback,
    w_session_on_error_callback p_on_error_callback) noexcept {

  constexpr auto _max = 999;
  constexpr auto _min = 9;
  std::default_random_engine _rand_engine {};
  std::uniform_int_distribution<int> _rand_gen(_min, _max);

  auto _rand_num = _rand_gen(_rand_engine);

#ifdef __clang__
// Get UTC time
  time_t _current_time = time(NULL);
  tm* tm_gmt = gmtime(&_current_time);
  
  auto size = std::snprintf(nullptr, 0, "%d-%02d-%02d %02d:%02d:%d_%d",
  	tm_gmt->tm_year + 1900, tm_gmt->tm_mon, tm_gmt->tm_mday,
  	tm_gmt->tm_hour, tm_gmt->tm_min, tm_gmt->tm_sec, 36);
  
  std::string _conn_id(size + 1, '\0');
  
  std::sprintf(&_conn_id[0], "%d-%02d-%02d %02d:%02d:%d_%d",
  	tm_gmt->tm_year + 1900, tm_gmt->tm_mon, tm_gmt->tm_mday,
  	tm_gmt->tm_hour, tm_gmt->tm_min, tm_gmt->tm_sec, 36);
#else  
  const auto _conn_id =
      std::format("{}_{}", std::chrono::utc_clock::now(), _rand_num);
#endif

  time_point _deadline = {};
  auto _ret =
      co_await (on_echo(p_io_context, p_socket, _conn_id, _deadline, p_timeout,
                        p_on_data_callback, p_on_error_callback) ||
                watchdog(_deadline));
  if (std::get<1>(_ret) == std::errc::timed_out) {
    p_on_timeout_callback(_conn_id);
  }
}

boost::asio::awaitable<void>
listen(boost::asio::io_context &p_io_context, tcp::endpoint &p_endpoint,
       steady_clock::duration &p_timeout, w_socket_options &p_socket_options,
       w_session_on_data_callback p_on_data_callback,
       w_session_on_timeout_callback p_on_timeout_callback,
       w_session_on_error_callback p_on_error_callback) noexcept {
  auto _executor = co_await boost::asio::this_coro::executor;
  tcp::acceptor _acceptor(_executor, p_endpoint);

  // set acceptor's options
  const auto _keep_alive_option =
      boost::asio::socket_base::keep_alive(p_socket_options.keep_alive);
  _acceptor.set_option(_keep_alive_option);

  const auto _reuse_address_option =
      boost::asio::socket_base::reuse_address(p_socket_options.reuse_address);
  _acceptor.set_option(_reuse_address_option);

#ifdef __clang__
#pragma unroll
#endif
  while (!p_io_context.stopped()) {
    tcp::socket _socket =
        co_await _acceptor.async_accept(boost::asio::use_awaitable);

    // set socket's options
    const auto _no_delay_opt =
        boost::asio::ip::tcp::no_delay(p_socket_options.no_delay);
    _socket.set_option(_no_delay_opt);
    _socket.set_option(_keep_alive_option);
    _socket.set_option(_reuse_address_option);

    // spawn a coroutinue for handling connection
    co_spawn(_executor,
             handle_connection(p_io_context, std::move(_socket), p_timeout,
                               p_on_data_callback, p_on_timeout_callback,
                               p_on_error_callback),
             boost::asio::detached);
  }
}

void w_tcp_server::run(
    boost::asio::io_context &p_io_context, tcp::endpoint &&p_endpoint,
    std::chrono::steady_clock::duration &&p_timeout,
    w_socket_options &&p_socket_options,
    w_session_on_data_callback p_on_data_callback,
    w_session_on_timeout_callback p_on_timeout_callback,
    w_session_on_error_callback p_on_error_callback) noexcept {
  boost::asio::co_spawn(p_io_context,
                        listen(p_io_context, p_endpoint, p_timeout,
                               p_socket_options, p_on_data_callback,
                               p_on_timeout_callback, p_on_error_callback),
                        boost::asio::detached);
}

#endif
