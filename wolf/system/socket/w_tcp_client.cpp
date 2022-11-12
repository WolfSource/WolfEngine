#ifdef WOLF_SYSTEM_SOCKET

#include "w_tcp_client.hpp"
#include <chrono>

#include "DISABLE_ANALYSIS_BEGIN"
#include <boost/asio/experimental/awaitable_operators.hpp>
#include "DISABLE_ANALYSIS_END"

using namespace boost::asio::experimental::awaitable_operators;
using w_tcp_client = wolf::system::socket::w_tcp_client;
using tcp = boost::asio::ip::tcp;

w_tcp_client::w_tcp_client(
    std::shared_ptr<boost::asio::io_context> p_io_context,
    const w_socket_options p_socket_options) noexcept :
    _context(std::move(p_io_context)),
    _options(p_socket_options)
{
    const auto _io_ctx_nn = gsl::not_null<boost::asio::io_context*>(this->_context.get());
    this->_resolver = std::make_unique<boost::asio::ip::tcp::resolver>(*_io_ctx_nn);
    this->_socket = std::make_unique<tcp::socket>(*_io_ctx_nn);
}

w_tcp_client::~w_tcp_client() noexcept
{
    try
    {
        const auto _resolver_nn = gsl::not_null<tcp::resolver*>(this->_resolver.get());
        const auto _socket_nn = gsl::not_null<tcp::socket*>(this->_socket.get());

        _resolver_nn->cancel();
        if (_socket_nn->is_open())
        {
            _socket_nn->close();
        }
        _socket_nn->release();
    }
    catch (...)
    {
    }
}

boost::asio::awaitable<boost::asio::ip::basic_resolver_results<tcp>> 
w_tcp_client::resolve(
    _In_ const tcp::endpoint& p_endpoint)
{
    const auto _resolver_nn = gsl::not_null<tcp::resolver*>(this->_resolver.get());
    return _resolver_nn->async_resolve(
        p_endpoint,
        boost::asio::use_awaitable);
}

//boost::asio::awaitable<w_result>w_tcp_client::connect(
//    _In_ const tcp::endpoint& p_endpoint,
//    _In_ const w_socket_options& p_socket_options)
//{
//    const auto _io_ctx_nn = gsl::not_null<boost::asio::io_context*>(this->_context.get());
//    const auto _socket_nn = gsl::not_null<tcp::socket*>(this->_socket.get());
//    if (_socket_nn->is_open())
//    {
//        _socket_nn->close();
//    }
//
//    try
//    {
//        // get protocol
//        const auto _protocol = p_endpoint.protocol();
//
//        // open socket and connect to the endpoint asynchronously
//        _socket_nn->open(_protocol);
//
//        // set socket options
//        _socket_nn->set_option(boost::asio::socket_base::reuse_address(this->_options.reuse_address));
//        _socket_nn->set_option(boost::asio::socket_base::keep_alive(this->_options.keep_alive));
//        _socket_nn->set_option(boost::asio::ip::tcp::no_delay(this->_options.no_delay));
//
//        auto _timer = boost::asio::system_timer(*_io_ctx_nn);
//        _timer.expires_after(this->_options.timeout);
//
//        co_await(
//            _timer.async_wait(boost::asio::use_awaitable) ||
//            this->_socket->async_connect(p_endpoint,
//                boost::asio::use_awaitable)
//            );
//
//        co_return W_OK();
//    }
//    catch (const std::exception& p_ex)
//    {
//        const auto _msg = std::format(
//            "could not open a socket on the endpoint: {}, because: {}",
//            p_endpoint.address().to_string(),
//            p_ex.what());
//        co_return W_ERR(std::errc::connection_refused, _msg);
//    }
//}

boost::asio::awaitable<std::variant<std::monostate, size_t>> w_tcp_client::write(std::string_view p_src_buffer)
{
    const auto _socket_nn = gsl::not_null<boost::asio::ip::tcp::socket*>(this->_socket.get());

    if (p_src_buffer.empty())
    {
        co_return (size_t)0;
    }

    auto _timer = boost::asio::system_timer(*this->_context);
    _timer.expires_after(this->_options.timeout);

    co_await(
        _timer.async_wait(boost::asio::use_awaitable) &&
        _socket_nn->async_send(
            boost::asio::buffer(p_src_buffer.data(), p_src_buffer.size()),
            boost::asio::use_awaitable)
        );
}

boost::asio::awaitable<std::variant<std::monostate, size_t>> w_tcp_client::read(std::string& p_dst_buffer)
{
    if (this->_context == nullptr || this->_socket == nullptr)
    {
        co_return std::variant<std::monostate, size_t>((size_t)0);
    }

    auto _timer = boost::asio::system_timer(*this->_context);
    _timer.expires_after(this->_options.timeout);

    //try
    //{
        co_await(
            _timer.async_wait(boost::asio::use_awaitable) &&
            this->_socket->async_receive(
                boost::asio::buffer(p_dst_buffer),
                boost::asio::use_awaitable)
            );
    //}
    //catch (const boost::system::system_error& p_error)
    //{
    //    //on_error_callback(p_error);
    //}
}

#endif
