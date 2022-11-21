/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
#ifndef EMSCRIPTEN

#pragma once

#include <wolf.hpp>

namespace wolf::system {
class w_process {
public:
  // default constructor
  W_API w_process() = default;
  // destructor
  W_API virtual ~w_process() = default;
  // move constructor
  W_API w_process(w_process &&p_src) noexcept = default;
  // move assignment operator.
  W_API w_process &operator=(w_process &&p_src) noexcept = default;


  /**
   * get current exe path
   * @returns current exe path
   */
  W_API static boost::leaf::result<std::filesystem::path> current_path();

  /**
   * get current exe path
   * @returns current exe path
   */
   W_API static boost::leaf::result<std::filesystem::path> current_exe_path();

private:
  // copy constructor.
  w_process(const w_process &) = delete;
  // copy assignment operator.
  w_process &operator=(const w_process &) = delete;
};
} // namespace wolf::system

#endif // EMSCRIPTEN
