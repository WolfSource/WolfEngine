/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/
#if defined(WOLF_SYSTEM_VIRTUAL_GAMEPAD) && !defined(EMSCRIPTEN)

#pragma once

#include <wolf.hpp>
#include <vector>

#include <DISABLE_ANALYSIS_BEGIN>
#include <Xinput.h>
#include <ViGEm/Client.h>
#include <DISABLE_ANALYSIS_END>

namespace wolf::system::gamepad {
class w_virtual_gamepad {
public:
  // default constructor
  W_API w_virtual_gamepad() noexcept = default;
  // destructor
  W_API virtual ~w_virtual_gamepad() noexcept = default;
  // move constructor
  W_API w_virtual_gamepad(w_virtual_gamepad &&p_src) noexcept = default;
  // move assignment operator.
  W_API w_virtual_gamepad &
  operator=(w_virtual_gamepad &&p_src) noexcept = default;

  /**
   * initialize gamepad
   * @returns VIGEM_ERROR
   */
  boost::leaf::result<VIGEM_ERROR> init() noexcept;

  /**
   * clear state
   * @returns VIGEM_ERROR
   */
  boost::leaf::result<VIGEM_ERROR> clear_state(_In_ size_t p_index) noexcept;

  /**
   * add a gamepad to the bus
   * @returns a tuple which contains index of gamepad and VIGEM_ERROR
   */
  boost::leaf::result<std::tuple<int, VIGEM_ERROR>> add() noexcept;

  /**
   * remove a gamepad from the bus
   * @returns VIGEM_ERROR
   */
  boost::leaf::result<VIGEM_ERROR> remove(_In_ size_t p_index) noexcept;

  /**
   * send an input to the gamepad
   * @param p_index, the index of gamepad
   * @param p_xinput an x input of gamepad
   * @returns VIGEM_ERROR
   */
  boost::leaf::result<VIGEM_ERROR>
  send_input(_In_ size_t p_index, _In_ XINPUT_STATE &p_xinput) noexcept;

  /**
   * get number of gamepads
   * @returns total number of gamepads
   */
  size_t get_number_of_gamepads() const noexcept;

  private:
  // copy constructor.
  w_virtual_gamepad(const w_virtual_gamepad &) = delete;
  // copy assignment operator.
  w_virtual_gamepad &operator=(const w_virtual_gamepad &) = delete;

  /**
   * release gamepad resources
   */
  void _release() noexcept;

  PVIGEM_CLIENT _driver_handler = nullptr;
  std::vector<PVIGEM_TARGET> _gamepads;
};
} // namespace wolf::system::gamepad

#endif // defined(WOLF_SYSTEM_VIRTUAL_GAMEPAD) && !defined(EMSCRIPTEN)
