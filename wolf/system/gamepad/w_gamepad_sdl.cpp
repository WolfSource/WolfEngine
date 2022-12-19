#if defined(WOLF_SYSTEM_GAMEPAD) && !defined(EMSCRIPTEN)

#include "w_gamepad.hpp"

#include <DISABLE_ANALYSIS_BEGIN>
#include <SDL.h>
#include <DISABLE_ANALYSIS_END>

#include <iostream>
#include <vector>

using w_gamepad = wolf::system::gamepad::w_gamepad;
using w_gamepad_event = wolf::system::gamepad::w_gamepad_event;

std::vector<SDL_GameController*> controllers;
std::vector<w_gamepad_event> w_gamepad::_events;

static std::string s_get_sdl_last_error_msg() {
  std::string _error_msg;
  _error_msg.reserve(W_MAX_PATH);
  SDL_GetErrorMsg(_error_msg.data(), W_MAX_PATH);
  return _error_msg;
}

w_gamepad::w_gamepad() noexcept {}

w_gamepad::~w_gamepad() noexcept {
  for (auto _iter : controllers) {
    SDL_GameControllerClose(_iter);
  }
  controllers.clear();
  SDL_Quit();
}

boost::leaf::result<int> w_gamepad::init() noexcept {

  const auto _ret = SDL_Init(SDL_INIT_GAMECONTROLLER);
  if (_ret != 0) {
    return W_FAILURE(_ret,
                     "could not initialize the SDL for gamepad. SDL error: " +
                         s_get_sdl_last_error_msg());
  }
  for (auto i = 0; i < SDL_NumJoysticks(); ++i) {
    if (SDL_IsGameController(i)) {
      controllers.push_back(SDL_GameControllerOpen(i));
    }
  }
  return S_OK;
}

void w_gamepad::update() {
  SDL_Event sdl_event;
  w_gamepad_button button{.type = w_gamepad_event_type::BUTTON};
  w_gamepad_axis axis{.type = w_gamepad_event_type::AXIS};
  w_gamepad_event event;
  while (SDL_PollEvent(&sdl_event)) {
    switch (sdl_event.type) {
    default:
      break;
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
      button.which = sdl_event.cbutton.which;
      button.button =
          gsl::narrow_cast<w_gamepad_button_type>(sdl_event.cbutton.button + 1);
      button.state = sdl_event.cbutton.state ? w_gamepad_state_type::PRESSED
                                             : w_gamepad_state_type::RELEASED;
      event.button = button;
      _events.emplace_back(event);
      break;

    case SDL_CONTROLLERAXISMOTION:
      axis.which = sdl_event.caxis.which;
      axis.axis =
          gsl::narrow_cast<w_gamepad_axis_type>(sdl_event.caxis.axis + 1);
      axis.value = sdl_event.caxis.value;
      event.axis = axis;
      _events.emplace_back(event);
      break;

    case SDL_CONTROLLERDEVICEADDED:
      controllers.push_back(SDL_GameControllerOpen(sdl_event.cdevice.which));
      break;

    case SDL_CONTROLLERDEVICEREMOVED:
      auto controller =
          SDL_GameControllerFromInstanceID(sdl_event.cdevice.which);
      auto it = std::find(controllers.cbegin(), controllers.cend(), controller);
      SDL_GameControllerClose(*it);
      controllers.erase(it);
      break;
    }
  }
}

#endif // defined(WOLF_SYSTEM_GAMEPAD) && !defined(EMSCRIPTEN)