#ifdef WOLF_SYSTEM_GAMEPAD

#pragma once

#include <cstdint>

namespace wolf::system::gamepad {

enum class w_gamepad_event_type { BUTTON, AXIS, MAX };

enum class w_gamepad_state_type { RELEASED, PRESSED, MAX };

enum class w_gamepad_button_type {
  A,
  B,
  X,
  Y,
  BACK,
  GUIDE,
  START,
  LEFTSTICK,
  RIGHTSTICK,
  LEFTSHOULDER,
  RIGHTSHOULDER,
  DPAD_UP,
  DPAD_DOWN,
  DPAD_LEFT,
  DPAD_RIGHT,
  MISC1,   /* Xbox Series X share button, PS5 microphone button, Nintendo Switch
              Pro capture button, Amazon Luna microphone button */
  PADDLE1, /* Xbox Elite paddle P1 */
  PADDLE2, /* Xbox Elite paddle P3 */
  PADDLE3, /* Xbox Elite paddle P2 */
  PADDLE4, /* Xbox Elite paddle P4 */
  TOUCHPAD, /* PS4/PS5 touchpad button */
  MAX
};

enum class w_gamepad_axis_type {
  LEFTX,
  LEFTY,
  RIGHTX,
  RIGHTY,
  TRIGGERLEFT,
  TRIGGERRIGHT,
  MAX
};

struct w_gamepad_button {
  w_gamepad_event_type type;
  std::uint32_t which;
  w_gamepad_button_type button;
  w_gamepad_state_type state;
};

struct w_gamepad_axis {
  w_gamepad_event_type type;
  std::uint32_t which;
  w_gamepad_axis_type axis;
  std::int16_t value;
};

union w_gamepad_event {
  w_gamepad_event_type type;
  w_gamepad_button button;
  w_gamepad_axis axis;
};

} // namespace wolf::system::gamepad

#endif