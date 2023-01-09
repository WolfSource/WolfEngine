#ifdef WOLF_SYSTEM_SOCKET

#include "w_tcp_server.hpp"
#include <random>

#include "DISABLE_ANALYSIS_BEGIN"
#include <boost/asio/experimental/awaitable_operators.hpp>
#include "DISABLE_ANALYSIS_END"

using w_tcp_server = wolf::system::socket::w_tcp_server;
using w_session_on_data_callback = wolf::system::socket::w_session_on_data_callback;
using w_session_on_error_callback = wolf::system::socket::w_session_on_error_callback;
using w_socket_options = wolf::system::socket::w_socket_options;
using steady_clock = std::chrono::steady_clock;
using steady_timer = boost::asio::steady_timer;
using io_context = boost::asio::io_context;
using tcp = boost::asio::ip::tcp;
using time_point = std::chrono::steady_clock::time_point;
using namespace boost::asio::experimental::awaitable_operators;

static boost::asio::awaitable<std::errc>
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

static boost::asio::awaitable<void> on_handle_session(
    const boost::asio::io_context &p_io_context, tcp::socket &p_socket,
    const std::string &p_conn_id, time_point &p_deadline,
    steady_clock::duration p_timeout,
    const w_session_on_data_callback &p_on_data_callback,
    const w_session_on_error_callback &p_on_error_callback) noexcept {
  w_buffer _buffer = {};

#ifdef __clang__
#pragma unroll
#endif
  while (!p_io_context.stopped()) {
    p_deadline = steady_clock::now() + p_timeout;

    try {
      _buffer.used_bytes = co_await p_socket.async_receive(
          boost::asio::buffer(_buffer.buf), boost::asio::use_awaitable);

      // call callback
      const auto _res = p_on_data_callback(p_conn_id, _buffer);
      if (_res == boost::system::errc::connection_aborted) {
        break;
      }
      co_await p_socket.async_send(
          boost::asio::buffer(_buffer.buf, _buffer.used_bytes),
          boost::asio::use_awaitable);
    } catch (const boost::system::system_error &p_ex) {
      p_on_error_callback(p_conn_id, p_ex);
      break;
    }
  }
}

static boost::asio::awaitable<void>
s_session(const boost::asio::io_context &p_io_context, tcp::socket p_socket,
          steady_clock::duration p_timeout,
          w_session_on_data_callback p_on_data_callback,
          w_session_on_error_callback p_on_error_callback) noexcept {

  const auto _conn_id = wolf::system::socket::make_connection_id();

  time_point _deadline = {};
  const auto _ret = co_await (
      on_handle_session(p_io_context, p_socket, _conn_id, _deadline, p_timeout,
                        p_on_data_callback, p_on_error_callback) ||
      watchdog(_deadline));
  if (std::get<1>(_ret) == std::errc::timed_out) {
    const auto _error = boost::system::system_error(
        make_error_code(boost::system::errc::timed_out));
    p_on_error_callback(_conn_id, _error);
  }

  co_return;
}

static boost::asio::awaitable<void>
s_listen(_In_ const boost::asio::io_context &p_io_context,
         _In_ tcp::endpoint &p_endpoint, _In_ const steady_clock::duration &p_timeout,
         _In_ w_socket_options &p_socket_options,
         _In_ w_session_on_data_callback p_on_data_callback,
         _In_ w_session_on_error_callback p_on_error_callback) noexcept {
  // create acceptor from this coroutine
  auto _executor = co_await boost::asio::this_coro::executor;
  tcp::acceptor _acceptor(_executor, p_endpoint);

  // set acceptor's options
  p_socket_options.set_to_acceptor(_acceptor);

  // start listening for connections
  _acceptor.listen(p_socket_options.max_connections);

#ifdef __clang__
#pragma unroll
#endif
  while (!p_io_context.stopped()) {
    tcp::socket _socket =
        co_await _acceptor.async_accept(boost::asio::use_awaitable);

    p_socket_options.set_to_socket(_socket);

    // spawn a coroutinue for handling session
    co_spawn(_executor,
             s_session(p_io_context, std::move(_socket), p_timeout,
                       p_on_data_callback,
                       p_on_error_callback),
             boost::asio::detached);
  }
}

boost::leaf::result<int> w_tcp_server::run(
    _In_ boost::asio::io_context &p_io_context,
    _In_ boost::asio::ip::tcp::endpoint &&p_endpoint,
    _In_ std::chrono::steady_clock::duration &&p_timeout,
    _In_ w_socket_options &&p_socket_options,
    _In_ w_session_on_data_callback p_on_data_callback,
    _In_ w_session_on_error_callback p_on_error_callback) noexcept {
  try {
    // server with coroutines
    boost::asio::co_spawn(p_io_context,
                          s_listen(p_io_context, p_endpoint, p_timeout,
                                   p_socket_options, p_on_data_callback,
                                   p_on_error_callback),
                          boost::asio::detached);
    return S_OK;

  } catch (_In_ const std::exception &p_ex) {
    return W_FAILURE(std::errc::operation_canceled,
                     "tcp server caught an exception : " +
                         std::string(p_ex.what()));
  }
}

#endif // WOLF_SYSTEM_SOCKET
