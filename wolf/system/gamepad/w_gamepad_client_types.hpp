#ifdef WOLF_SYSTEM_GAMEPAD_CLIENT

#pragma once

#include <cstdint>

namespace wolf::system::gamepad {

enum class w_gamepad_client_event_type { button, axis, max };

enum class w_gamepad_client_state_type { released, pressed, max };

enum class w_gamepad_client_button_type {
  A,
  B,
  X,
  Y,
  back,
  guide,
  start,
  left_stick,
  right_stick,
  left_shoulder,
  right_shoulder,
  dpad_up,
  dpad_down,
  dpad_left,
  dpad_right,
  // Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro
  // capture button, Amazon Luna microphone button
  misc_1,
  // Xbox Elite paddle P1
  paddle_1,
  // Xbox Elite paddle P3
  paddle_2,
  // Xbox Elite paddle P2
  paddle_3,
  // Xbox Elite paddle P4
  paddle_4,
  // PS4/PS5 touchpad button
  touch_pad,
  max
};

enum class w_gamepad_client_axis_type {
  left_x,
  left_y,
  right_x,
  right_y,
  trigger_left,
  trigger_right,
  max
};

struct w_gamepad_client_button {
  w_gamepad_client_event_type type;
  std::uint32_t which;
  w_gamepad_client_button_type button;
  w_gamepad_client_state_type state;
};

struct w_gamepad_client_axis {
  w_gamepad_client_event_type type;
  std::uint32_t which;
  w_gamepad_client_axis_type axis;
  std::int16_t value;
};

union w_gamepad_client_event {
  w_gamepad_client_event_type type;
  w_gamepad_client_button button;
  w_gamepad_client_axis axis;
};

} // namespace wolf::system::gamepad

#endif