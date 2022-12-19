#ifdef WOLF_SYSTEM_GAMEPAD
#ifndef EMSCRIPTEN
#include <SDL.h>

#include <iostream>
#include <vector>

#include "w_gamepad.hpp"

using w_gamepad = wolf::system::gamepad::w_gamepad;
using w_gamepad_event = wolf::system::gamepad::w_gamepad_event;

std::vector<SDL_GameController*> controllers;
std::vector<w_gamepad_event> w_gamepad::_events;

w_gamepad::w_gamepad() noexcept {
  // TODO: since the constructor is noexcept should we add an init function?
  if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
    std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
  }

  for (int i = 0; i < SDL_NumJoysticks(); i++) {
    if (SDL_IsGameController(i)) {
      controllers.push_back(SDL_GameControllerOpen(i));
    }
  }
}

w_gamepad::~w_gamepad() noexcept {
  for (auto controller : controllers) {
    SDL_GameControllerClose(controller);
  }
  controllers.clear();
  SDL_Quit();
}

void w_gamepad::update() {
  SDL_Event sdl_event;
  w_gamepad_button button{.type = w_gamepad_event_type::BUTTON};
  w_gamepad_axis axis{.type = w_gamepad_event_type::AXIS};
  w_gamepad_event event;
  while (SDL_PollEvent(&sdl_event)) {
    switch (sdl_event.type) {
      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
        button.which = sdl_event.cbutton.which;
        button.button = (w_gamepad_button_type)(sdl_event.cbutton.button + 1);
        button.state = sdl_event.cbutton.state ? w_gamepad_state_type::PRESSED : w_gamepad_state_type::RELEASED;
        event.button = button;
        _events.emplace_back(event);
        break;

      case SDL_CONTROLLERAXISMOTION:
        axis.which = sdl_event.caxis.which;
        axis.axis = (w_gamepad_axis_type)(sdl_event.caxis.axis + 1);
        axis.value = sdl_event.caxis.value;
        event.axis = axis;
        _events.emplace_back(event);
        break;

      case SDL_CONTROLLERDEVICEADDED:
        controllers.push_back(SDL_GameControllerOpen(sdl_event.cdevice.which));
        break;

      case SDL_CONTROLLERDEVICEREMOVED:
        auto controller = SDL_GameControllerFromInstanceID(sdl_event.cdevice.which);
        auto it = std::find(controllers.begin(), controllers.end(), controller);
        SDL_GameControllerClose(*it);
        controllers.erase(it);
        break;
    }
  }
}

#endif
#endif