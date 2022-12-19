#pragma once

#ifdef WOLF_SYSTEM_GAMEPAD

#include <wolf.hpp>

#include "w_gamepad_types.h"
#include <vector>

namespace wolf::system::gamepad {

// TODO: should be static class
class w_gamepad {
 public:
  W_API w_gamepad() noexcept;
  W_API ~w_gamepad() noexcept;
  W_API boost::leaf::result<int> init() noexcept;

  W_API bool get_events(w_gamepad_event &p_event) {
    if (_events.empty()) {
      return false;
    }

    p_event = _events.back();
    _events.pop_back();

    return true;
  }

  void update();

 private:
  //INFO: using std::vector makes a LIFO queue of events
  static std::vector<w_gamepad_event> _events;
};

} // namespace wolf::system::gamepad

#endif
