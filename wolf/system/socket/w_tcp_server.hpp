/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_SOCKET

#include <wolf.hpp>
#include "w_socket_options.hpp"
#include <boost/asio.hpp>

namespace wolf::system::io
{
    class w_tcp_server
    {
    public:
        /*
         * @param p_io_context, the boost io context
         * @param p_endpoint, the endpoint of the server
         * @param p_socket_options, the socket options
         * @returns void 
         */
        W_API static void init(
            boost::asio::io_context& p_io_context,
            boost::asio::ip::tcp::endpoint&& p_endpoint,
            w_socket_options&& p_socket_options,
            std::function<boost::leaf::result<bool>(
                std::span<char, W_MAX_BUFFER_SIZE> /*p_data*/,
                size_t /*p_read_bytes*/)> p_on_io_callback) noexcept;
    };
} // namespace wolf::system::io

#endif