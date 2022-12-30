#if defined(WOLF_SYSTEM_HTTP1_WS) && defined(WOLF_SYSTEM_SOCKET)

#pragma once

#include <wolf.hpp>
#include "w_socket_options.hpp"

namespace wolf::system::socket {
class w_ws_client {
public:
  // default constructor
  W_API explicit w_ws_client(boost::asio::io_context &p_io_context) noexcept;

  // move constructor.
  W_API w_ws_client(w_ws_client &&p_other) = default;
  // move assignment operator.
  W_API w_ws_client &operator=(w_ws_client &&p_other) = default;

  // destructor
  W_API virtual ~w_ws_client() noexcept;

  /*
   * resolve an endpoint asynchronously
   * @param p_endpoint, the endpoint
   * @returns a coroutine contains the results of reolver
   */
  W_API
  boost::asio::awaitable<
      boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp>>
  async_resolve(_In_ const boost::asio::ip::tcp::endpoint &p_endpoint);

  /*
   * resolve an address asynchronously
   * @param p_address, the address
   * @param p_port, the port
   * @returns a coroutine contains the results of reolver
   */
  W_API
  boost::asio::awaitable<
      boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp>>
  async_resolve(_In_ const std::string &p_address, _In_ const uint16_t &p_port);

  /*
   * open a websocket and handshake with the endpoint asynchronously
   * @param p_endpoint, the endpoint of the server
   * @param p_socket_options, the socket options
   * @returns a coroutine
   */
  W_API
  boost::asio::awaitable<void>
  async_connect(_In_ const boost::asio::ip::tcp::endpoint &p_endpoint,
                _In_ const w_socket_options &p_socket_options);

  /*
   * write buffer data into the websocket
   * @param p_buffer, the source buffer which should be written
   * @returns a number of the written bytes
   */
  W_API
  boost::asio::awaitable<size_t> async_write(_In_ const w_buffer &p_buffer,
                                             _In_ bool p_is_binary);

  /*
   * read from the websocket into the buffer
   * @param p_mut_buffer, the destination buffer which will contain bytes
   * @returns a coroutine with number of read bytes
   */
  W_API
  boost::asio::awaitable<size_t> async_read(_Inout_ w_buffer &p_mut_buffer);

  /*
   * read from the websocket into the buffer
   * @param p_mut_buffer, the destination buffer which will contain bytes
   * @returns a coroutine with number of read bytes
   */
  W_API
  boost::asio::awaitable<size_t>
  async_read(_Inout_ boost::beast::flat_buffer &p_mut_buffer);

  /*
   * close the websocket asynchronously
   * @param p_close_reason, the close reason
   * @returns a coroutine
   */
  W_API
  boost::asio::awaitable<void>
  async_close(_In_ const boost::beast::websocket::close_reason &p_close_reason);

  /*
   * get whether websocket is open or not
   * @returns true if socket was open
   */
  W_API [[maybe_unused]] bool get_is_open() const;

private:
  // copy constructor
  w_ws_client(const w_ws_client &) = delete;
  // copy operator
  w_ws_client &operator=(const w_ws_client &) = delete;

  std::unique_ptr<w_ws_stream> _ws;
  std::unique_ptr<boost::asio::ip::tcp::resolver> _resolver;
};
} // namespace wolf::system::socket

#endif //defined(WOLF_SYSTEM_HTTP1_WS) && defined(WOLF_SYSTEM_SOCKET)
