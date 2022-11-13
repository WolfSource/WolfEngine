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
    boost::asio::io_context& p_io_context,
    const w_socket_options p_socket_options) noexcept :
    _options(p_socket_options)
{
    this->_resolver = std::make_unique<tcp::resolver>(p_io_context);
    this->_socket = std::make_unique<tcp::socket>(p_io_context);
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
w_tcp_client::async_resolve(
    _In_ const tcp::endpoint& p_endpoint)
{
    const auto _resolver_nn = gsl::not_null<tcp::resolver*>(this->_resolver.get());
    return _resolver_nn->async_resolve(
        p_endpoint,
        boost::asio::use_awaitable);
}

boost::asio::awaitable<boost::asio::ip::basic_resolver_results<tcp>>
w_tcp_client::async_resolve(
    _In_ const std::string& p_address,
    _In_ const std::string& p_port)
{
    const auto _resolver_nn = gsl::not_null<tcp::resolver*>(this->_resolver.get());

    const tcp::resolver::query _query(p_address, p_port);
    return _resolver_nn->async_resolve(_query, boost::asio::use_awaitable);
}

boost::asio::awaitable<void, boost::asio::any_io_executor> w_tcp_client::async_connect(
    _In_ const tcp::endpoint& p_endpoint,
    _In_ const w_socket_options& p_socket_options)
{
    const gsl::not_null<tcp::socket*> _socket_nn(this->_socket.get());
    if (_socket_nn->is_open())
    {
        _socket_nn->close();
    }

    // get protocol
    const auto _protocol = p_endpoint.protocol();

    // open socket and connect to the endpoint asynchronously
    _socket_nn->open(_protocol);

    // set socket options
    _socket_nn->set_option(boost::asio::socket_base::reuse_address(this->_options.reuse_address));
    _socket_nn->set_option(boost::asio::socket_base::keep_alive(this->_options.keep_alive));
    _socket_nn->set_option(tcp::no_delay(this->_options.no_delay));

    return this->_socket->async_connect(p_endpoint, boost::asio::use_awaitable);
}

boost::asio::awaitable<size_t> w_tcp_client::async_write(_In_ std::string_view p_buffer)
{
    const gsl::not_null<tcp::socket*> _socket_nn(this->_socket.get());

    return _socket_nn->async_send(
        boost::asio::buffer(p_buffer.data(), p_buffer.size()),
        boost::asio::use_awaitable);
}

boost::asio::awaitable<size_t> w_tcp_client::async_read(_Inout_ std::string& p_buffer)
{
    const gsl::not_null<tcp::socket*> _socket_nn(this->_socket.get());

    return
        _socket_nn->async_receive(
            boost::asio::buffer(p_buffer),
            boost::asio::use_awaitable);
}

#endif
