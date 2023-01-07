/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
#if defined(WOLF_SYSTEM_GAMEPAD_VIRTUAL) && !defined(EMSCRIPTEN)

#pragma once

#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
#include <ViGEm/Client.h>
#include <Xinput.h>
#include <DISABLE_ANALYSIS_END>

namespace wolf::system::gamepad {
struct w_gamepad_virtual {
public:
  // default constructor
  W_API w_gamepad_virtual(_In_ PVIGEM_CLIENT p_vigem,
                          _In_ PVIGEM_TARGET p_target) noexcept;

  // destructor
  W_API virtual ~w_gamepad_virtual() noexcept;

  // move constructor
  W_API w_gamepad_virtual(_Inout_ w_gamepad_virtual &&p_other) noexcept;

  // move assignment operator.
  W_API w_gamepad_virtual &
  operator=(_Inout_ w_gamepad_virtual &&p_other) noexcept;

  /**
   * clear the state of this gamepad
   * @returns VIGEM_ERROR
   */
  W_API boost::leaf::result<VIGEM_ERROR> clear_state() noexcept;

  /**
   * send an input to this gamepad
   * @param p_xinput an x input of gamepad
   * @returns VIGEM_ERROR
   */
  W_API boost::leaf::result<VIGEM_ERROR>
  send_input(_In_ XINPUT_STATE &p_xinput) noexcept;

private:
  // copy constructor.
  w_gamepad_virtual(const w_gamepad_virtual &) = delete;
  // copy assignment operator.
  w_gamepad_virtual &operator=(const w_gamepad_virtual &) = delete;

  void _move(_Inout_ w_gamepad_virtual &&p_other) noexcept;
  void _release() noexcept;

  VIGEM_ERROR _last_error = VIGEM_ERROR::VIGEM_ERROR_NONE;
  PVIGEM_CLIENT _vigem = nullptr;
  PVIGEM_TARGET _target = nullptr;
};
} // namespace wolf::system::gamepad

#endif // defined(WOLF_SYSTEM_VIRTUAL_GAMEPAD) && !defined(EMSCRIPTEN)
