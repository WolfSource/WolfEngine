/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_SOCKET

#include <wolf.hpp>
#include <chrono>
#include <functional>

namespace wolf::system::socket
{
    struct w_socket_options
    {
        bool keep_alive = true;
        bool no_delay = true;
        bool reuse_address = true;
        std::chrono::system_clock::duration timeout = std::chrono::milliseconds(10000);
    };
    typedef std::function<
        boost::leaf::result<void>(gsl::span<char> p_data, size_t p_read_bytes)> w_socket_on_data_callback;
} // namespace wolf::system

#endif