#ifdef WOLF_SYSTEM_IO

#include "w_tcp_client.hpp"
#include <chrono>

using namespace boost::asio::experimental::awaitable_operators;
using w_tcp_client = wolf::system::io::w_tcp_client;
using tcp = boost::asio::ip::tcp;

w_tcp_client::w_tcp_client(
    std::shared_ptr<boost::asio::io_context> p_io_context,
    const w_socket_options p_socket_options) noexcept :
    _socket(nullptr),
    _context(std::move(p_io_context)),
    _options(p_socket_options),
    _resolver(std::make_unique<boost::asio::ip::tcp::resolver>(*_context.get()))
{
}

w_tcp_client::w_tcp_client(w_tcp_client&& p_other) noexcept
{
    _release();
    _move(std::move(p_other));
}

w_tcp_client& w_tcp_client::operator=(w_tcp_client&& p_other) noexcept
{
    if (this != &p_other)
    {
        _release();
        _move(std::move(p_other));
    }
    return *this;
}

w_tcp_client::~w_tcp_client() noexcept
{
    _release();
}

boost::asio::awaitable<boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp>> 
w_tcp_client::resolve(
    _In_ const boost::asio::ip::tcp::endpoint& p_endpoint)
{
    const auto _res_not_null = gsl::not_null<boost::asio::ip::tcp::resolver*>(this->_resolver.get());
    return _res_not_null->async_resolve(p_endpoint, boost::asio::use_awaitable);
}

//boost::asio::awaitable<w_ret> w_tcp_client::connect(
//    _In_ const boost::asio::ip::tcp::endpoint& p_endpoint)
//{
//    if (this->_context == nullptr)
//    {
//        co_return w_ret("io context was null");
//    }
//
//    if (this->_socket == nullptr)
//    {
//        this->_socket = std::make_unique<tcp::socket>(*this->_context);
//    }
//    else
//    {
//        if (this->_socket->is_open())
//        {
//            this->_socket->close();
//        }
//    }
//
//    // get protocol
//    auto _protocol = p_endpoint.protocol();
//
//    try
//    {
//        // open socket and connect to the endpoint asynchronously
//        this->_socket->open(_protocol);
//        // set socket options
//        this->_socket->set_option(boost::asio::socket_base::reuse_address(this->_options.reuse_address));
//        this->_socket->set_option(boost::asio::socket_base::keep_alive(this->_options.keep_alive));
//        this->_socket->set_option(boost::asio::ip::tcp::no_delay(this->_options.no_delay));
//
//        auto _timer = boost::asio::system_timer(*this->_context);
//        _timer.expires_after(this->_options.timeout);
//
//        co_await(
//            _timer.async_wait(boost::asio::use_awaitable) ||
//            this->_socket->async_connect(p_endpoint,
//                boost::asio::use_awaitable)
//            );
//
//        co_return w_ret();
//    }
//    catch (const boost::system::system_error& p_error)
//    {
//        on_error_callback(p_error);
//
//        co_return w_ret("could not open a socket on endpoint: {}, because: {}",
//            p_endpoint.address().to_string(),
//            p_error.what());
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

void w_tcp_client::_release() noexcept
{
    const auto _res_nn = gsl::not_null<boost::asio::ip::tcp::resolver*>(this->_resolver.get());
    _res_nn->cancel();

    if (this->_socket != nullptr)
    {
        const auto _socket_nn = gsl::not_null<boost::asio::ip::tcp::socket*>(this->_socket.get());
        if (_socket_nn->is_open())
        {
            _socket_nn->close();
        }
        _socket_nn->release();
    }
}

void w_tcp_client::_move(w_tcp_client&& p_other) noexcept
{
    this->_options = std::move(p_other._options);
    this->_context = std::move(p_other._context);
    this->_socket = std::move(p_other._socket);
    this->_resolver = std::move(p_other._resolver);
    //this->on_conn_callback = std::move(p_other.on_conn_callback);
    //this->on_error_callback = std::move(p_other.on_error_callback);
}

#endif
