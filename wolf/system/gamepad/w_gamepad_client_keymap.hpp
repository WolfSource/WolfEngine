#ifdef WOLF_SYSTEM_GAMEPAD_CLIENT

#pragma once

#include "w_gamepad_client_types.hpp"

namespace wolf::system::gamepad {

static w_gamepad_client_button_type w_gamepad_button_map[] = {
    w_gamepad_client_button_type::A,
    w_gamepad_client_button_type::B,
    w_gamepad_client_button_type::X,
    w_gamepad_client_button_type::Y,
    w_gamepad_client_button_type::left_shoulder,
    w_gamepad_client_button_type::right_shoulder,
    w_gamepad_client_button_type::paddle_1,
    w_gamepad_client_button_type::paddle_2,
    w_gamepad_client_button_type::back,
    w_gamepad_client_button_type::start,
    w_gamepad_client_button_type::left_stick,
    w_gamepad_client_button_type::right_stick,
    w_gamepad_client_button_type::dpad_up,
    w_gamepad_client_button_type::dpad_down,
    w_gamepad_client_button_type::dpad_left,
    w_gamepad_client_button_type::dpad_right,
    w_gamepad_client_button_type::guide,
};

static w_gamepad_client_axis_type w_gamepad_axis_map[] = {
    w_gamepad_client_axis_type::left_x,
    w_gamepad_client_axis_type::left_y,
    w_gamepad_client_axis_type::right_x,
    w_gamepad_client_axis_type::right_y,
};

} // namespace wolf::system::gamepad

#endif
