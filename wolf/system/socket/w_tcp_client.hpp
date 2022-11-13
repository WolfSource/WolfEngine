/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_SOCKET

#include <wolf.hpp>
#include <variant>
#include "w_socket_options.hpp"

#include "DISABLE_ANALYSIS_BEGIN"
#include <boost/asio.hpp>
#include "DISABLE_ANALYSIS_END"

namespace wolf::system::socket
{
    class w_tcp_client
    {
    public:
        // default constructor
        W_API w_tcp_client(
            boost::asio::io_context& p_io_context,
            const w_socket_options p_socket_options) noexcept;

        // move constructor.
        W_API w_tcp_client(w_tcp_client&& p_other) = default;
        // move assignment operator.
        W_API w_tcp_client& operator=(w_tcp_client&& p_other) = default;

        // destructor
        W_API virtual ~w_tcp_client() noexcept;

        /*
         * resolve an endpoint asynchronously
         * @param p_endpoint, the endpoint
         * @returns a coroutine contains the results of reolver
         */
        W_API
            boost::asio::awaitable<boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp>>
            async_resolve(_In_ const boost::asio::ip::tcp::endpoint& p_endpoint);

        /*
        * resolve an address with port asynchronously
        * @param p_address, the address
        * @param p_port, the port
        * @returns a coroutine contains the results of reolver
        */
        W_API
            boost::asio::awaitable<boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp>>
            async_resolve(
                _In_ const std::string& p_address,
                _In_ const std::string& p_port);

        /*
         * open a socket and connect to the endpoint asynchronously
         * @param p_endpoint, the endpoint of the server
         * @param p_socket_options, the socket options
         * @returns a coroutine
         */
        W_API
            boost::asio::awaitable<void, boost::asio::any_io_executor>
            async_connect(
                _In_ const boost::asio::ip::tcp::endpoint& p_endpoint,
                _In_ const w_socket_options& p_socket_options);

        /*
         * write buffer data on the socket
         * @param p_buffer, the source buffer which should be written
         * @returns a coroutine with number of the written bytes
         */
        W_API
            boost::asio::awaitable<size_t> async_write(_In_ std::string_view p_buffer);
        /*
         * read from the socket into the buffer
         * @param p_buffer, the destination buffer which will contain bytes
         * @returns a coroutine with number of read bytes
         */
        W_API
            boost::asio::awaitable<size_t> async_read(_Inout_ std::string& p_buffer);

    private:
        // copy constructor
        w_tcp_client(const w_tcp_client&) = delete;
        // copy operator
        w_tcp_client& operator=(const w_tcp_client&) = delete;

        w_socket_options _options;
        std::unique_ptr<boost::asio::ip::tcp::socket> _socket;
        std::unique_ptr<boost::asio::ip::tcp::resolver> _resolver;
    };
} // namespace wolf::system

#endif