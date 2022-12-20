#ifdef WOLF_SYSTEM_GAMEPAD

#pragma once

#include "w_gamepad_types.hpp"

namespace wolf::system::gamepad {

static w_gamepad_button_type w_gamepad_button_map[] = {
  w_gamepad_button_type::A,
  w_gamepad_button_type::B,
  w_gamepad_button_type::X,
  w_gamepad_button_type::Y,
  w_gamepad_button_type::LEFTSHOULDER,
  w_gamepad_button_type::RIGHTSHOULDER,
  w_gamepad_button_type::PADDLE1,
  w_gamepad_button_type::PADDLE2,
  w_gamepad_button_type::BACK,
  w_gamepad_button_type::START,
  w_gamepad_button_type::LEFTSTICK,
  w_gamepad_button_type::RIGHTSTICK,
  w_gamepad_button_type::DPAD_UP,
  w_gamepad_button_type::DPAD_DOWN,
  w_gamepad_button_type::DPAD_LEFT,
  w_gamepad_button_type::DPAD_RIGHT,
  w_gamepad_button_type::GUIDE,
};

static w_gamepad_axis_type w_gamepad_axis_map[] = {
  w_gamepad_axis_type::LEFTX,
  w_gamepad_axis_type::LEFTY,
  w_gamepad_axis_type::RIGHTX,
  w_gamepad_axis_type::RIGHTY,
};

} // namespace wolf::system::gamepad

#endif
