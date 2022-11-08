#ifdef WOLF_SYSTEM_SOCKET

#include "w_tcp_server.hpp"
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <iostream>

using w_tcp_server = wolf::system::socket::w_tcp_server;
using w_socket_on_data_callback = wolf::system::socket::w_socket_on_data_callback;
using w_socket_options = wolf::system::socket::w_socket_options;
using duration = std::chrono::system_clock::duration;
using steady_clock = std::chrono::steady_clock;
using steady_timer = boost::asio::steady_timer;
using io_context = boost::asio::io_context;
using tcp = boost::asio::ip::tcp;
using time_point = std::chrono::steady_clock::time_point;

using namespace boost::asio::experimental::awaitable_operators;

boost::asio::awaitable<std::errc> watchdog(_Inout_ time_point& p_deadline)
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
    time_point& p_deadline,
    duration p_timeout,
    w_socket_on_data_callback& p_on_io_callback)
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
        const auto _res = p_on_io_callback(gsl::span(_data), _bytes);
        const auto _err_code = _res.get_error_id().to_error_code();
        if (_err_code == std::errc::connection_aborted)
        {
            break;
        }

        co_await p_socket.async_send(
            boost::asio::buffer(_data, _bytes),
            boost::asio::use_awaitable);
    }
}

boost::asio::awaitable<void> handle_connection(
    const boost::asio::io_context& p_io_context,
    tcp::socket p_socket,
    std::chrono::system_clock::duration p_timeout,
    w_socket_on_data_callback p_on_io_callback)
{
    time_point _deadline = {};
    auto _ret = co_await(
        on_echo(p_io_context, p_socket, _deadline, p_timeout, p_on_io_callback) || watchdog(_deadline));
    if (std::get<1>(_ret) == std::errc::timed_out)
    {
        // TODO: log
        std::cerr << "timeout for s_handle_connection" << std::endl;
    }
}

boost::asio::awaitable<void> listen(
    const boost::asio::io_context& p_io_context,
    tcp::endpoint& p_endpoint,
    w_socket_options& p_socket_options,
    w_socket_on_data_callback p_on_data_callback)
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
                p_socket_options.timeout,
                p_on_data_callback),
            boost::asio::detached);
    }
}

void w_tcp_server::init(
    boost::asio::io_context& p_io_context,
    boost::asio::ip::tcp::endpoint&& p_endpoint,
    w_socket_options&& p_socket_options,
    w_socket_on_data_callback p_on_data_callback) noexcept
{
    boost::asio::co_spawn(p_io_context,
        listen(p_io_context, p_endpoint, p_socket_options, p_on_data_callback),
        boost::asio::detached);

    ///// how to use w_trace
    //boost::leaf::try_handle_all(
    //    [&]() -> boost::leaf::result<void>
    //    {
    //        BOOST_LEAF_CHECK([]()
    //            {

    //                return {};
    //            });
    //        return { };
    //    },
    //    [](w_trace const& p_trace) -> boost::leaf::result<void>
    //    {
    //        //std::cout << "caught a test exception! trace info:" << std::endl << p_trace;
    //        return {};
    //    },
    //    [](std::exception_ptr const* p_ex_ptr) -> boost::leaf::result<void>
    //    {
    //        //std::cerr << "Got unknown error!" << std::endl;

    //        // Above, why do we take ep as a pointer? Because handle_all
    //        // requires that the last handler matches any error and, taken
    //        // as a pointer, if there isn't a std::exception_ptr associated
    //        // with the error, the handler will still be matched (with 0
    //        // passed for ep). Had we taken it by value or by const &, the
    //        // program would not have compiled.
    //        if (p_ex_ptr)
    //        {
    //            boost::leaf::try_catch(
    //                [&]
    //                {
    //                    std::rethrow_exception(*p_ex_ptr);
    //                },
    //                    [](boost::leaf::error_info const& p_unmatched)
    //                {
    //                    // write in log
    //                });
    //        }
    //        return {};
    //    },
    //    []() ->boost::leaf::result<void> {
    //        //std::cout << "caught a test exception!" << std::endl;
    //        return {};
    //    });
}

#endif
