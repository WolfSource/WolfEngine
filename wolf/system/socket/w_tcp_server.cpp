#ifdef WOLF_SYSTEM_SOCKET

#include "w_tcp_server.hpp"

using namespace wolf;
using boost::asio::use_awaitable;

using w_tcp_server = system::io::w_tcp_server;
//using w_socket_options = system::io::w_socket_options;
//using tcp = boost::asio::ip::tcp;
//using time_point = std::chrono::steady_clock::time_point;
//namespace asio = boost::asio;
//
//asio::awaitable<std::errc> watchdog(time_point& p_deadline)
//{
//    asio::steady_timer _timer(co_await asio::this_coro::executor);
//    auto _now = std::chrono::steady_clock::now();
//
//#ifdef __clang__
//#pragma unroll
//#endif
//    while (p_deadline > _now)
//    {
//        _timer.expires_at(p_deadline);
//        co_await _timer.async_wait(use_awaitable);
//        _now = std::chrono::steady_clock::now();
//    }
//
//    co_return std::errc::timed_out;
//}
//
//boost::asio::awaitable<void> s_on_echo(
//    const asio::io_context& p_io_context,
//    tcp::socket& p_socket, 
//    time_point& p_deadline,
//    std::chrono::system_clock::duration p_timeout,
//    std::function<w_result<bool>(std::span<char, W_MAX_BUFFER_SIZE> /*p_data*/, size_t /*p_read_bytes*/)> p_on_io_callback)
//{
//    std::array<char, W_MAX_BUFFER_SIZE> _data = { 0 };
//
//#ifdef __clang__
//#pragma unroll
//#endif
//    while (!p_io_context.stopped())
//    {
//        p_deadline = std::chrono::steady_clock::now() + p_timeout;
//        auto _read_bytes = co_await p_socket.async_receive(asio::buffer(_data), use_awaitable);
//        
//        // send data to callback for io
//        if (!p_on_io_callback(_data, _read_bytes))
//        {
//            break;
//        }
//
//        co_await p_socket.async_send(asio::buffer(_data, _read_bytes), use_awaitable);
//    }
//}
//
//asio::awaitable<void> s_handle_connection(
//    const asio::io_context& p_io_context,
//    tcp::socket p_socket,
//    std::chrono::system_clock::duration p_timeout,
//    std::function<w_result<bool>(std::span<char, W_MAX_BUFFER_SIZE>, size_t)> p_on_io_callback)
//{
//    time_point _deadline{};
//    auto _ret = co_await(s_on_echo(p_io_context, p_socket, _deadline, p_timeout, p_on_io_callback) || watchdog(_deadline));
//    if (std::get<1>(_ret) == std::errc::timed_out)
//    {
//        std::cout << "timeout for s_handle_connection" << std::endl;
//    }
//}
//
//asio::awaitable<void> s_listen(
//    const asio::io_context& p_io_context,
//    tcp::endpoint& p_endpoint,
//    w_socket_options& p_socket_options,
//    std::function<w_result<bool>(std::span<char, W_MAX_BUFFER_SIZE> /*p_data*/, size_t /*p_read_bytes*/)> p_on_io_callback)
//{
//    auto _executor = co_await asio::this_coro::executor;
//    tcp::acceptor _acceptor(_executor, p_endpoint);
//    
//    // set acceptor's options
//    const auto _keep_alive_option = asio::socket_base::keep_alive(p_socket_options.keep_alive);
//    _acceptor.set_option(_keep_alive_option);
//    
//    const auto _reuse_address_option = asio::socket_base::reuse_address(p_socket_options.reuse_address);
//    _acceptor.set_option(_reuse_address_option);
//    
//#ifdef __clang__
//#pragma unroll
//#endif
//    while (!p_io_context.stopped())
//    {
//        tcp::socket _socket = co_await _acceptor.async_accept(use_awaitable);
//
//        // set socket's options
//        const auto _no_delay_opt = asio::ip::tcp::no_delay(p_socket_options.no_delay);
//        _socket.set_option(_no_delay_opt);
//        _socket.set_option(_keep_alive_option);
//        _socket.set_option(_reuse_address_option);
//
//        // spawn a coroutinue for handling connection
//        co_spawn(_executor,
//            s_handle_connection(
//                p_io_context,
//                std::move(_socket),
//                p_socket_options.timeout,
//                p_on_io_callback),
//            asio::detached);
//    }
//}

W_API void w_tcp_server::init(
    boost::asio::io_context& p_io_context,
    boost::asio::ip::tcp::endpoint&& p_endpoint,
    w_socket_options&& p_socket_options,
    std::function<boost::leaf::result<bool>(
        std::span<char, W_MAX_BUFFER_SIZE> /*p_data*/,
        size_t /*p_read_bytes*/)> p_on_io_callback) noexcept
{

    /// how to use w_trace
    boost::leaf::try_handle_all(
        [&]() -> boost::leaf::result<void>
        {
            //BOOST_LEAF_CHECK(_function());
            //std::cout << "success!" << std::endl;
                //co_spawn(p_io_context,
    //    s_listen(p_io_context, p_endpoint, p_socket_options, p_on_io_callback),
    //    asio::detached);
            return { };
        },
        [](w_trace const& p_trace)
        {
            //std::cout << "caught a test exception! trace info:" << std::endl << p_trace;
        },
        [](std::exception_ptr const* p_ex_ptr)
        {
            //std::cerr << "Got unknown error!" << std::endl;

            // Above, why do we take ep as a pointer? Because handle_all
            // requires that the last handler matches any error and, taken
            // as a pointer, if there isn't a std::exception_ptr associated
            // with the error, the handler will still be matched (with 0
            // passed for ep). Had we taken it by value or by const &, the
            // program would not have compiled.
            if (p_ex_ptr)
            {
                boost::leaf::try_catch(
                    [&]
                    {
                        std::rethrow_exception(*p_ex_ptr);
                    },
                        [](boost::leaf::error_info const& p_unmatched)
                    {
                        // write in log
                    });
            }
        },
        []() {
            //std::cout << "caught a test exception!" << std::endl;
        });
}

#endif
