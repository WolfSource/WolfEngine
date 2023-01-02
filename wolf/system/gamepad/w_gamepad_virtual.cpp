#if defined(WOLF_SYSTEM_GAMEPAD_VIRTUAL) && !defined(EMSCRIPTEN)

#include "w_gamepad_virtual.hpp"

using w_gamepad_virtual = wolf::system::gamepad::w_gamepad_virtual;

w_gamepad_virtual::w_gamepad_virtual(_In_ PVIGEM_CLIENT p_vigem,
                                     _In_ PVIGEM_TARGET p_target) noexcept
    : _vigem(p_vigem), _target(p_target) {}

w_gamepad_virtual::~w_gamepad_virtual() { _release(); }

w_gamepad_virtual::w_gamepad_virtual(_Inout_ w_gamepad_virtual &&p_other) noexcept {
  _move(std::move(p_other));
}

w_gamepad_virtual &
w_gamepad_virtual::operator=(_Inout_ w_gamepad_virtual &&p_other) noexcept {
  _move(std::move(p_other));
  return *this;
}

void w_gamepad_virtual::_move(_Inout_ w_gamepad_virtual &&p_other) noexcept {
  if (this == &p_other)
    return;

  _release();

  this->_last_error = p_other._last_error;
  this->_target = std::exchange(p_other._target, nullptr);
  this->_vigem = std::exchange(p_other._vigem, nullptr);
}

boost::leaf::result<VIGEM_ERROR> w_gamepad_virtual::clear_state() noexcept {
  // xinput state
  XINPUT_STATE _state = {0};
  // update x360 gamepad
  const auto _ret = vigem_target_x360_update(
      this->_vigem, this->_target, *reinterpret_cast<XUSB_REPORT *>(&_state));
  return _ret;
}

boost::leaf::result<VIGEM_ERROR>
w_gamepad_virtual::send_input(_In_ XINPUT_STATE &p_xinput) noexcept {
  // update x360 gamepad
  const auto _ret = vigem_target_x360_update(
      this->_vigem, this->_target,
      *reinterpret_cast<XUSB_REPORT *>(&p_xinput.Gamepad));
  return _ret;
}

void w_gamepad_virtual::_release() noexcept {
  if (this->_target != nullptr) {
    if (this->_vigem != nullptr) {
      vigem_target_remove(this->_vigem, this->_target);
    }
    vigem_target_free(this->_target);
  }
}

#endif // #if defined(WOLF_SYSTEM_VIRTUAL_GAMEPAD) && !defined(EMSCRIPTEN)



