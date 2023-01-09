#if defined(WOLF_SYSTEM_HTTP1_WS) && defined(WOLF_SYSTEM_SOCKET)

#include "w_ws_client.hpp"

using w_ws_client = wolf::system::socket::w_ws_client;
using tcp = boost::asio::ip::tcp;

w_ws_client::w_ws_client(boost::asio::io_context &p_io_context) noexcept
    : _resolver(std::make_unique<tcp::resolver>(p_io_context)) {}

w_ws_client::~w_ws_client() noexcept {
  try {
    const auto _resolver_nn =
        gsl::not_null<tcp::resolver *>(this->_resolver.get());
    _resolver_nn->cancel();

    if (this->_ws != nullptr && this->_ws->is_open()) {
      this->_ws->close(boost::beast::websocket::close_code::normal);
    }
  } catch (...) {
  }
}

boost::asio::awaitable<boost::asio::ip::basic_resolver_results<tcp>>
w_ws_client::async_resolve(_In_ const tcp::endpoint &p_endpoint) {
  const auto _resolver_nn =
      gsl::not_null<tcp::resolver *>(this->_resolver.get());
  return _resolver_nn->async_resolve(p_endpoint, boost::asio::use_awaitable);
}

boost::asio::awaitable<boost::asio::ip::basic_resolver_results<tcp>>
w_ws_client::async_resolve(_In_ const std::string &p_address,
                           _In_ const uint16_t &p_port) {
  const auto _resolver_nn =
      gsl::not_null<tcp::resolver *>(this->_resolver.get());

  const auto _address = boost::asio::ip::make_address(p_address);
  const tcp::endpoint _endpoint(_address, p_port);
  return _resolver_nn->async_resolve(_endpoint, boost::asio::use_awaitable);
}

boost::asio::awaitable<void> w_ws_client::async_connect(
    _In_ const boost::asio::ip::tcp::endpoint &p_endpoint,
    _In_ const w_socket_options &p_socket_options) {

  this->_ws =
      std::make_unique<w_ws_stream>(boost::asio::use_awaitable.as_default_on(
          boost::beast::websocket::stream<boost::beast::tcp_stream>(
              co_await boost::asio::this_coro::executor)));

  co_await boost::beast::get_lowest_layer(*this->_ws)
      .async_connect(p_endpoint, boost::asio::use_awaitable);

  // turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  boost::beast::get_lowest_layer(*this->_ws).expires_never();

  // set suggested timeout settings for the websocket
  this->_ws->set_option(
      boost::beast::websocket::stream_base::timeout::suggested(
          boost::beast::role_type::client));

  // set a decorator to change the User-Agent of the handshake
  this->_ws->set_option(boost::beast::websocket::stream_base::decorator(
      [](boost::beast::websocket::request_type &req) {
        req.set(boost::beast::http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) + " wolf-ws-client");
      }));

  // perform the websocket handshake
  co_await this->_ws->async_handshake(p_endpoint.address().to_string(), "/");
}

boost::asio::awaitable<size_t>
w_ws_client::async_write(_In_ const w_buffer &p_buffer, _In_ bool p_is_binary) {
  if (p_is_binary) {
    this->_ws->binary(true);
  } else {
    this->_ws->text(true);
  }
  co_await this->_ws->async_write(
      boost::asio::const_buffer(p_buffer.buf.data(), p_buffer.used_bytes));
}

boost::asio::awaitable<size_t>
w_ws_client::async_read(_Inout_ w_buffer &p_mut_buffer) {
  boost::beast::flat_buffer _buffer = {};
  co_await this->_ws->async_read(_buffer);

  // an extra copy just for having stable ABI
  const auto _size = std::min(_buffer.cdata().size(), p_mut_buffer.buf.size());
  std::memcpy(p_mut_buffer.buf.data(),
              static_cast<char const *>(_buffer.cdata().data()), _size);
}

boost::asio::awaitable<size_t>
w_ws_client::async_read(_Inout_ boost::beast::flat_buffer &p_mut_buffer) {
  co_await this->_ws->async_read(p_mut_buffer);
}

boost::asio::awaitable<void> w_ws_client::async_close(
    _In_ const boost::beast::websocket::close_reason &p_close_reason) {
  if (this->_ws == nullptr) {
    co_return;
  }
  co_await this->_ws->async_close(p_close_reason);
}

bool w_ws_client::get_is_open() const {
  if (this->_ws == nullptr) {
    return false;
  }
  return this->_ws->is_open();
}

#endif //defined(WOLF_SYSTEM_HTTP1_WS) && defined(WOLF_SYSTEM_SOCKET)
