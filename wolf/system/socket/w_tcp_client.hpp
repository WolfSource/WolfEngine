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
            std::shared_ptr<boost::asio::io_context> p_io_context, 
            const w_socket_options p_socket_options) noexcept;

        // move constructor.
        W_API w_tcp_client(w_tcp_client&& p_other) = default;
        // move assignment operator.
        W_API w_tcp_client& operator=(w_tcp_client&& p_other) = default;

        // destructor
        W_API virtual ~w_tcp_client() noexcept;

       /*
        * resolve an endpoint
        * @param p_endpoint, the endpoint
        * @returns a coroutine contains the results of reolver
        */
        W_API
            boost::asio::awaitable<boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp>>
                resolve(_In_ const boost::asio::ip::tcp::endpoint& p_endpoint);

        /*
         * open a socket and connect to the endpoint
         * @param p_endpoint, the endpoint of the server
         * @param p_socket_options, the socket options
         * @returns a coroutine with result
         */
        //W_API boost::asio::awaitable<w_result> connect(
        //    _In_ const boost::asio::ip::tcp::endpoint& p_endpoint,
        //    _In_ const w_socket_options& p_socket_options);

        /*
         * write to the socket
         * @param p_src_buffer, the source buffer which should be written used as the source buffer
         * @returns a coroutine with number of written bytes
         */
        W_API boost::asio::awaitable<std::variant<std::monostate, size_t>> write(std::string_view p_src_buffer);

       /*
        * read from the socket
        * @param p_dst_buffer, the destination buffer which contains read bytes
        * @returns a coroutine with number of read bytes
        */
        W_API boost::asio::awaitable<std::variant<std::monostate, size_t>> read(std::string& p_dst_buffer);

        // callbacks
        //boost::signals2::signal<void(const boost::system::system_error& /*p_error_code*/)> on_conn_callback;
        //boost::signals2::signal<void(const boost::system::system_error& /*p_error_code*/)> on_error_callback;

    private:
        // copy constructor
        w_tcp_client(const w_tcp_client&) = delete;
        // copy operator
        w_tcp_client& operator=(const w_tcp_client&) = delete;

        w_socket_options _options;
        std::shared_ptr<boost::asio::io_context> _context;
        std::unique_ptr<boost::asio::ip::tcp::socket> _socket;
        std::unique_ptr<boost::asio::ip::tcp::resolver> _resolver;
    };
} // namespace wolf::system

#endif