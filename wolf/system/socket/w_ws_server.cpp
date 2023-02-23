#if defined(WOLF_SYSTEM_HTTP1_WS) && defined(WOLF_SYSTEM_SOCKET)

#include "w_ws_server.hpp"

using w_ws_server = wolf::system::socket::w_ws_server;
using w_session_ws_on_data_callback = wolf::system::socket::w_session_ws_on_data_callback;
using w_session_on_error_callback = wolf::system::socket::w_session_on_error_callback;
using w_socket_options = wolf::system::socket::w_socket_options;
using io_context = boost::asio::io_context;
using w_ws_stream = wolf::system::socket::w_ws_stream;
using tcp = boost::asio::ip::tcp;

static boost::asio::awaitable<void>
s_session(_In_ const boost::asio::io_context &p_io_context,
          _In_ w_ws_stream &&p_ws,
          _In_ const std::string p_conn_id,
          _In_ w_session_ws_on_data_callback p_on_data_callback,
          _In_ w_session_on_error_callback p_on_error_callback) {

  // accept the websocket handshake
  co_await p_ws.async_accept();

  w_buffer _mut_buffer = {};
  // incoming message
  boost::beast::flat_buffer _buffer;

  while (!p_io_context.stopped()) {
    try {
      // This buffer will hold the incoming message
      
      // Read a message
      _mut_buffer.used_bytes = co_await p_ws.async_read(_buffer);

      // an extra copy just for having stable ABI
      const auto _size = std::min(_mut_buffer.buf.size(), _mut_buffer.used_bytes);
      std::memcpy(_mut_buffer.buf.data(),
                  static_cast<char const *>(_buffer.cdata().data()), _size);

      // call callback
      auto _is_binary = p_ws.got_binary();
      const auto _code = p_on_data_callback(p_conn_id, _mut_buffer, _is_binary);
      if (_code != boost::beast::websocket::close_code::none) {
        break;
      }

      // Echo the message back
      if (_is_binary) {
        p_ws.binary(true);
      } else {
        p_ws.text(true);
      }
      co_await p_ws.async_write(
          boost::asio::buffer(_mut_buffer.buf, _mut_buffer.used_bytes));

    } catch (const boost::system::system_error &p_exc) {
      if (p_exc.code() != boost::beast::websocket::error::closed) {
        p_on_error_callback(p_conn_id, p_exc);
      }
    }
  }
}

static boost::asio::awaitable<void>
s_listen(_In_ const boost::asio::io_context &p_io_context,
         _In_ const tcp::endpoint &p_endpoint,
         _In_ const boost::beast::websocket::stream_base::timeout &p_timeout,
         _In_ w_socket_options &p_socket_options,
         _In_ w_session_ws_on_data_callback p_on_data_callback,
         _In_ w_session_on_error_callback p_on_error_callback) {
  // create acceptor from this coroutine
  auto _executor = co_await boost::asio::this_coro::executor;
  auto _acceptor =
      boost::asio::use_awaitable.as_default_on(tcp::acceptor(_executor));

  // open an acceptor
  _acceptor.open(p_endpoint.protocol());

  // allow address reuse
  _acceptor.set_option(
      boost::asio::socket_base::reuse_address(p_socket_options.reuse_address));

  // bind to the server address
  _acceptor.bind(p_endpoint);

  // start listening for connections
  _acceptor.listen(p_socket_options.max_connections);

#ifdef __clang__
#pragma unroll
#endif
  while (!p_io_context.stopped()) {
    auto _ws = w_ws_stream(co_await _acceptor.async_accept());
    // set timeout settings for the websocket
    _ws.set_option(p_timeout);
    // set a decorator to change the Server of the handshake
    _ws.set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::response_type &res) {
          res.set(boost::beast::http::field::server,
                  std::string(BOOST_BEAST_VERSION_STRING) + "wolf-ws-server");
        }));
    const auto _conn_id = wolf::system::socket::make_connection_id();

    boost::asio::co_spawn(_acceptor.get_executor(),
                          s_session(p_io_context, std::move(_ws), _conn_id,
                                    p_on_data_callback, p_on_error_callback));
  }
}

boost::leaf::result<int> w_ws_server::run(
    _In_ boost::asio::io_context &p_io_context,
    _In_ const boost::asio::ip::tcp::endpoint &&p_endpoint,
    _In_ const boost::beast::websocket::stream_base::timeout &p_timeout,
    _In_ w_socket_options &&p_socket_options,
    _In_ w_session_ws_on_data_callback p_on_data_callback,
    _In_ w_session_on_error_callback p_on_error_callback) noexcept {
  try {
    // server with coroutines
    boost::asio::co_spawn(p_io_context,
                          s_listen(p_io_context, p_endpoint, p_timeout,
                                   p_socket_options, std::move(p_on_data_callback),
                                   std::move(p_on_error_callback)),
                          boost::asio::detached);
    return S_OK;

  } catch (_In_ const std::exception &p_ex) {
    return W_FAILURE(std::errc::operation_canceled,
                     "ws server caught an exception : " +
                         std::string(p_ex.what()));
  }
}

#endif //defined(WOLF_SYSTEM_HTTP1_WS) && defined(WOLF_SYSTEM_SOCKET)
