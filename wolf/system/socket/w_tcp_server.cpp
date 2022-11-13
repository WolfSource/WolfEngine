#ifdef WOLF_SYSTEM_SOCKET

#include "w_tcp_server.hpp"
#include <random>

#include "DISABLE_ANALYSIS_BEGIN"
#include <boost/asio/experimental/awaitable_operators.hpp>
#include "DISABLE_ANALYSIS_END"

using w_tcp_server = wolf::system::socket::w_tcp_server;
using w_session_on_data_callback = wolf::system::socket::w_session_on_data_callback;
using w_session_on_timeout_callback = wolf::system::socket::w_session_on_timeout_callback;
using w_socket_options = wolf::system::socket::w_socket_options;
using steady_clock = std::chrono::steady_clock;
using steady_timer = boost::asio::steady_timer;
using io_context = boost::asio::io_context;
using tcp = boost::asio::ip::tcp;
using time_point = std::chrono::steady_clock::time_point;

using namespace boost::asio::experimental::awaitable_operators;

boost::asio::awaitable<std::errc> watchdog(_Inout_ const time_point& p_deadline) noexcept
{
    steady_timer _timer(co_await boost::asio::this_coro::executor);
    auto _now = steady_clock::now();

#ifdef __clang__
#pragma unroll
#endif
    while (p_deadline > _now)
    {
        _timer.expires_at(p_deadline);
        co_await _timer.async_wait(boost::asio::use_awaitable);
        _now = steady_clock::now();
    }
    co_return std::errc::timed_out;
}

boost::asio::awaitable<void> on_echo(
    const boost::asio::io_context& p_io_context,
    tcp::socket& p_socket,
    const std::string& p_conn_id,
    time_point& p_deadline,
    steady_clock::duration p_timeout,
    const w_session_on_data_callback& p_on_data_callback) noexcept
{
    std::array<char, W_MAX_BUFFER_SIZE> _data = { 0 };

#ifdef __clang__
#pragma unroll
#endif
    while (!p_io_context.stopped())
    {
        p_deadline = steady_clock::now() + p_timeout;

        auto _bytes = co_await p_socket.async_receive(
            boost::asio::buffer(_data),
            boost::asio::use_awaitable);

        // call callback
        const auto _res = p_on_data_callback(p_conn_id, gsl::span(_data), _bytes);
        co_await p_socket.async_send(
            boost::asio::buffer(_data, _bytes),
            boost::asio::use_awaitable);

        if (_res == boost::system::errc::connection_aborted)
        {
            break;
        }
    }
}

boost::asio::awaitable<void> handle_connection(
    const boost::asio::io_context& p_io_context,
    tcp::socket p_socket,
    steady_clock::duration p_timeout,
    w_session_on_data_callback p_on_data_callback,
    w_session_on_timeout_callback p_on_timeout_callback)  noexcept
{
    constexpr auto _max = 999;
    constexpr auto _min = 9;
    std::default_random_engine _rand_engine = {};
    std::uniform_int_distribution<int> _rand_gen(_min, _max);

    auto _rand_num = _rand_gen(_rand_engine);
    const auto _conn_id = std::format("{}_{}", 
        std::chrono::utc_clock::now(), 
        _rand_num);

    time_point _deadline = {};
    auto _ret = co_await(
        on_echo(
            p_io_context,
            p_socket, 
            _conn_id, 
            _deadline, 
            p_timeout, 
            p_on_data_callback) || watchdog(_deadline));
    if (std::get<1>(_ret) == std::errc::timed_out)
    {
        p_on_timeout_callback(_conn_id);
    }
}

boost::asio::awaitable<void> listen(
    boost::asio::io_context& p_io_context,
    tcp::endpoint& p_endpoint,
    steady_clock::duration& p_timeout,
    w_socket_options& p_socket_options,
    w_session_on_data_callback p_on_data_callback,
    w_session_on_timeout_callback p_on_timeout_callback) noexcept
{
    auto _executor = co_await boost::asio::this_coro::executor;
    tcp::acceptor _acceptor(_executor, p_endpoint);

    // set acceptor's options
    const auto _keep_alive_option = boost::asio::socket_base::keep_alive(p_socket_options.keep_alive);
    _acceptor.set_option(_keep_alive_option);

    const auto _reuse_address_option = boost::asio::socket_base::reuse_address(p_socket_options.reuse_address);
    _acceptor.set_option(_reuse_address_option);

#ifdef __clang__
#pragma unroll
#endif
    while (!p_io_context.stopped())
    {
        tcp::socket _socket = co_await _acceptor.async_accept(boost::asio::use_awaitable);

        // set socket's options
        const auto _no_delay_opt = boost::asio::ip::tcp::no_delay(p_socket_options.no_delay);
        _socket.set_option(_no_delay_opt);
        _socket.set_option(_keep_alive_option);
        _socket.set_option(_reuse_address_option);

        // spawn a coroutinue for handling connection
        co_spawn(_executor,
            handle_connection(
                p_io_context,
                std::move(_socket),
                p_timeout,
                p_on_data_callback,
                p_on_timeout_callback),
            boost::asio::detached);
    }
}

//std::chrono::system_clock::duration& timeout = std::chrono::milliseconds(10000);
void w_tcp_server::init(
    boost::asio::io_context& p_io_context,
    tcp::endpoint&& p_endpoint,
    std::chrono::steady_clock::duration&& p_timeout,
    w_socket_options&& p_socket_options,
    w_session_on_data_callback p_on_data_callback,
    w_session_on_timeout_callback p_on_timeout_callback) noexcept
{
    boost::asio::co_spawn(
        p_io_context,
        listen(p_io_context, p_endpoint, p_timeout, p_socket_options, p_on_data_callback, p_on_timeout_callback),
        boost::asio::detached);
}

#endif
