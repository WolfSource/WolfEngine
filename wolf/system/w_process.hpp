/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
#pragma once

#include <wolf.hpp>

namespace wolf::system {
struct w_process {
  // default constructor
  W_API w_process() noexcept = default;
  // destructor
  W_API virtual ~w_process() noexcept = default;
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
