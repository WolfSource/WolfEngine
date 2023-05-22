#if defined(WOLF_SYSTEM_GAMEPAD_VIRTUAL) && !defined(EMSCRIPTEN)

#include "w_gamepad_virtual.hpp"

using w_gamepad_virtual = wolf::system::gamepad::w_gamepad_virtual;

w_gamepad_virtual::w_gamepad_virtual(
    _In_ std::shared_ptr<w_gamepad_virtual_bus> p_vigem_bus) noexcept
    : _bus(std::move(p_vigem_bus)) {}

w_gamepad_virtual::~w_gamepad_virtual() { _release(); }

w_gamepad_virtual::w_gamepad_virtual(_Inout_ w_gamepad_virtual &&p_other) noexcept {
  _move(std::move(p_other));
}

w_gamepad_virtual &w_gamepad_virtual::operator=(_Inout_ w_gamepad_virtual &&p_other) noexcept {
  _move(std::move(p_other));
  return *this;
}

void w_gamepad_virtual::_move(_Inout_ w_gamepad_virtual &&p_other) noexcept {
  if (this == &p_other)
    return;

  _release();

  this->_bus = std::move(p_other._bus);
  this->_target = std::exchange(p_other._target, nullptr);
  this->_last_error = p_other._last_error;
}

boost::leaf::result<VIGEM_ERROR> w_gamepad_virtual::init() noexcept {
  std::shared_lock _lock(this->_bus->mutex);

  if (this->_bus->driver_handle == nullptr) {
    return W_FAILURE(VIGEM_ERROR::VIGEM_ERROR_BUS_INVALID_HANDLE,
                     "invalid ViGem bus while creating ViGem target controller");
  }

  // allocate handle to identify new pad
  this->_target = vigem_target_x360_alloc();
  if (this->_target == nullptr) {
    return W_FAILURE(VIGEM_ERROR::VIGEM_ERROR_TARGET_NOT_PLUGGED_IN,
                     "could not allocate memory while creating ViGem target controller");
  }

  // add client to the bus, this equals a plug-in event
  auto _ret = vigem_target_add(this->_bus->driver_handle, this->_target);
  if (_ret == VIGEM_ERROR::VIGEM_ERROR_NONE) {
    return _ret;
  }
  return W_FAILURE(_ret, "could not add ViGem target controller to the bus");
}

boost::leaf::result<VIGEM_ERROR> w_gamepad_virtual::clear_state() noexcept {
  std::shared_lock _lock(this->_bus->mutex);

  // xinput state
  XUSB_REPORT _state = {0};
  // update x360 gamepad
  return vigem_target_x360_update(this->_bus->driver_handle, this->_target, _state);
}

boost::leaf::result<VIGEM_ERROR>
w_gamepad_virtual::send_input(_In_ XINPUT_STATE &p_xinput) noexcept {
  std::shared_lock _lock(this->_bus->mutex);

  // update x360 gamepad
  const auto _ret = vigem_target_x360_update(this->_bus->driver_handle, this->_target,
      *reinterpret_cast<XUSB_REPORT *>(&p_xinput.Gamepad));
  return _ret;
}

void w_gamepad_virtual::_release() noexcept {
  if (this->_bus == nullptr)
    return;

  std::shared_lock _lock(this->_bus->mutex);

  if (this->_target != nullptr) {
    if (this->_bus->driver_handle != nullptr) {
      vigem_target_remove(this->_bus->driver_handle, this->_target);
    }
    vigem_target_free(this->_target);
  }
}

#endif // #if defined(WOLF_SYSTEM_VIRTUAL_GAMEPAD) && !defined(EMSCRIPTEN)



