/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#if defined(WIN32) && defined(DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif

#include <wolf/wolf.hpp>

namespace wolf::system {
class w_leak_detector {
public:
  // default constructor
  W_API w_leak_detector() noexcept;
  // destructor
  W_API virtual ~w_leak_detector() noexcept;

private:
  // move constructor
  w_leak_detector(w_leak_detector &&p_src) noexcept = delete;
  // move assignment operator.
  w_leak_detector &operator=(w_leak_detector &&p_src) noexcept = delete;
  // copy constructor.
  w_leak_detector(const w_leak_detector &) = delete;
  // copy assignment operator.
  w_leak_detector &operator=(const w_leak_detector &) = delete;

#if defined(WIN32) && defined(DEBUG)
  _CrtMemState _mem_state;
#endif
};
} // namespace wolf::system