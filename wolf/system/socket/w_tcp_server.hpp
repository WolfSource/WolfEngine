/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_SOCKET

#include <wolf.hpp>
#include "w_socket_options.hpp"

#include "DISABLE_ANALYSIS_BEGIN"
#include <boost/asio.hpp>
#include "DISABLE_ANALYSIS_END"

namespace wolf::system::socket
{
    class w_tcp_server
    {
    public:
        /*
         * @param p_io_context, the boost io context
         * @param p_endpoint, the endpoint of the server
         * @param p_socket_options, the socket options
         * @param w_session_on_data_callback, on data callback for session
         * @param w_session_on_data_callback, on timeout callback for session
         * @returns void
         */
        W_API static void init(
            boost::asio::io_context& p_io_context,
            boost::asio::ip::tcp::endpoint&& p_endpoint,
            w_socket_options&& p_socket_options,
            w_session_on_data_callback p_on_data_callback,
            w_session_on_timeout_callback p_on_timeout_callback) noexcept;
    };
} // namespace wolf::system::io

#endif