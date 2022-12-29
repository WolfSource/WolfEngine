#if defined(WOLF_SYSTEM_GAMEPAD) && !defined(EMSCRIPTEN)

#include "w_gamepad.hpp"

#include <DISABLE_ANALYSIS_BEGIN>
#include <SDL3/SDL.h>
#include <DISABLE_ANALYSIS_END>

using w_gamepad = wolf::system::gamepad::w_gamepad;
using w_gamepad_event = wolf::system::gamepad::w_gamepad_event;

std::vector<SDL_Gamepad*> s_controllers;
std::vector<w_gamepad_event> w_gamepad::_events;

static std::string s_get_sdl_last_error_msg() noexcept {
  std::string _error_msg;
  _error_msg.reserve(W_MAX_PATH);
  SDL_GetErrorMsg(_error_msg.data(), W_MAX_PATH);
  return _error_msg;
}

boost::leaf::result<int> w_gamepad::init() noexcept {
  const auto _ret = SDL_Init(SDL_INIT_GAMEPAD);
  if (_ret != 0) {
    return W_FAILURE(_ret,
                     "could not initialize the SDL for gamepad. SDL error: " +
                         s_get_sdl_last_error_msg());
  }
  for (auto i = 0; i < SDL_GetNumJoysticks(); ++i) {
    if (SDL_IsGamepad(i)) {
      s_controllers.push_back(SDL_OpenGamepad(i));
    }
  }
  return S_OK;
}

void w_gamepad::update() noexcept {
  SDL_Event sdl_event = {};
  w_gamepad_button button{.type = w_gamepad_event_type::BUTTON};
  w_gamepad_axis axis{.type = w_gamepad_event_type::AXIS};
  w_gamepad_event _event = {};
  while (SDL_PollEvent(&sdl_event)) {
    switch (sdl_event.type) {
    default:
      break;
    case SDL_GAMEPADBUTTONDOWN:
    case SDL_GAMEPADBUTTONUP: {
      button.which = sdl_event.cbutton.which;
      button.button =
          gsl::narrow_cast<w_gamepad_button_type>(sdl_event.cbutton.button + 1);
      button.state = sdl_event.cbutton.state ? w_gamepad_state_type::PRESSED
                                             : w_gamepad_state_type::RELEASED;
      _event.button = button;
      _events.emplace_back(_event);
      break;
    }
    case SDL_GAMEPADAXISMOTION: {
      axis.which = sdl_event.caxis.which;
      axis.axis =
          gsl::narrow_cast<w_gamepad_axis_type>(sdl_event.caxis.axis + 1);
      axis.value = sdl_event.caxis.value;
      _event.axis = axis;
      _events.emplace_back(_event);
      break;
    }
    case SDL_GAMEPADADDED: {
      s_controllers.push_back(SDL_OpenGamepad(sdl_event.cdevice.which));
      break;
    }
    case SDL_GAMEPADREMOVED: {
      auto controller = SDL_GetGamepadFromInstanceID(sdl_event.cdevice.which);
      auto _iter =
          std::find(s_controllers.cbegin(), s_controllers.cend(), controller);
      SDL_CloseGamepad(*_iter);
      s_controllers.erase(_iter);
      break;
    }
    }
  }
}

void w_gamepad::fini() noexcept {
  for (auto _iter : s_controllers) {
    if (_iter) {
      SDL_CloseGamepad(_iter);
    }
  }
  s_controllers.clear();
  SDL_Quit();
}

#endif // defined(WOLF_SYSTEM_GAMEPAD) && !defined(EMSCRIPTEN)