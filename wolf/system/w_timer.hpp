/*
    Project: Wolf Engine. Copyright � 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf/wolf.hpp>
#ifndef EMSCRIPTEN
#include <boost/asio.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
using namespace boost::asio::experimental::awaitable_operators;
#endif

namespace wolf::system {
#ifdef EMSCRIPTEN
// a timer based on JS timeout
class w_timer {};
#else
class w_timer : public boost::asio::high_resolution_timer {
 public:
  explicit w_timer(boost::asio::io_context &p_io_context)
      : boost::asio::high_resolution_timer(p_io_context, 0) {}
};
#endif
}  // namespace wolf::system