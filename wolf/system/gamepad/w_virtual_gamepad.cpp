#include "w_virtual_gamepad.hpp"

using w_virtual_gamepad = wolf::system::gamepad::w_virtual_gamepad;

std::string s_vigem_error_to_string(_In_ const VIGEM_ERROR p_error) {
  switch (p_error) {
  default:
    // API succeeded
    return "VIGEM_ERROR_NONE";
  case VIGEM_ERROR::VIGEM_ERROR_BUS_NOT_FOUND:
    return "VIGEM_ERROR_BUS_NOT_FOUND: a compatible bus driver wasn't found";
  case VIGEM_ERROR::VIGEM_ERROR_NO_FREE_SLOT:
    return "VIGEM_ERROR_NO_FREE_SLOT: all device slots are occupied, no new "
           "device can be spawned";
  case VIGEM_ERROR::VIGEM_ERROR_INVALID_TARGET:
    return "VIGEM_ERROR_INVALID_TARGET: invalid target";
  case VIGEM_ERROR::VIGEM_ERROR_REMOVAL_FAILED:
    return "VIGEM_ERROR_REMOVAL_FAILED: could not remove";
  case VIGEM_ERROR::VIGEM_ERROR_ALREADY_CONNECTED:
    return "VIGEM_ERROR_ALREADY_CONNECTED: an attempt has been made to plug in "
           "an already connected device";
  case VIGEM_ERROR::VIGEM_ERROR_TARGET_UNINITIALIZED:
    return "VIGEM_ERROR_TARGET_UNINITIALIZED: the target device is not "
           "initialized";
  case VIGEM_ERROR::VIGEM_ERROR_TARGET_NOT_PLUGGED_IN:
    return "VIGEM_ERROR_TARGET_NOT_PLUGGED_IN: the target device is not "
           "plugged in";
  case VIGEM_ERROR::VIGEM_ERROR_BUS_VERSION_MISMATCH:
    return "VIGEM_ERROR_TARGET_NOT_PLUGGED_IN: it's been attempted to "
           "communicate with an incompatible driver version";
  case VIGEM_ERROR::VIGEM_ERROR_BUS_ACCESS_FAILED:
    return "VIGEM_ERROR_BUS_ACCESS_FAILED: bus driver found but failed "
           "to open "
           "a handle.";
  case VIGEM_ERROR::VIGEM_ERROR_CALLBACK_ALREADY_REGISTERED:
    return "VIGEM_ERROR_CALLBACK_ALREADY_REGISTERED: bus driver found but "
           "failed to open a handle.";
  case VIGEM_ERROR::VIGEM_ERROR_CALLBACK_NOT_FOUND:
    return "VIGEM_ERROR_CALLBACK_NOT_FOUND: bus driver found but failed to "
           "open a handle.";
  case VIGEM_ERROR::VIGEM_ERROR_BUS_ALREADY_CONNECTED:
    return "VIGEM_ERROR_BUS_ALREADY_CONNECTED: bus driver found but failed to "
           "open a handle.";
  case VIGEM_ERROR::VIGEM_ERROR_BUS_INVALID_HANDLE:
    return "VIGEM_ERROR_BUS_INVALID_HANDLE: bus driver found but "
           "failed to open "
           "a handle.";
  case VIGEM_ERROR::VIGEM_ERROR_XUSB_USERINDEX_OUT_OF_RANGE:
    return "VIGEM_ERROR_XUSB_USERINDEX_OUT_OF_RANGE: bus driver found but "
           "failed to open a handle.";
  case VIGEM_ERROR::VIGEM_ERROR_INVALID_PARAMETER:
    return "VIGEM_ERROR_INVALID_PARAMETER: bus driver found but failed to open "
           "a handle.";
  case VIGEM_ERROR::VIGEM_ERROR_NOT_SUPPORTED:
    return "VIGEM_ERROR_NOT_SUPPORTED: the API is not supported by the driver";
  case VIGEM_ERROR::VIGEM_ERROR_WINAPI:
    return "VIGEM_ERROR_WINAPI: an unexpected Win32 API error occurred. Call "
           "GetLastError() for details";
  case VIGEM_ERROR::VIGEM_ERROR_TIMED_OUT:
    return "VIGEM_ERROR_TIMED_OUT: the specified timeout has been reached";
  };
}

boost::leaf::result<VIGEM_ERROR> w_virtual_gamepad::init() noexcept {
  // allocate memory for bus driver
  this->_driver_handler = vigem_alloc();
  if (this->_driver_handler != nullptr) {
    _release();
  }

  // connect to vigem
  const auto _ret = vigem_connect(this->_driver_handler);
  if (VIGEM_SUCCESS(_ret)) {
    return _ret;
  }

  // release it
  _release();

  return W_FAILURE(_ret, "could not initialize ViGem client because: " +
                             s_vigem_error_to_string(_ret));
}

boost::leaf::result<VIGEM_ERROR>
w_virtual_gamepad::clear_state(_In_ size_t p_index) noexcept {

  auto _ret = VIGEM_ERROR::VIGEM_ERROR_TARGET_NOT_PLUGGED_IN;

  if (p_index < this->_gamepads.size()) {
    // xinput state
    XINPUT_STATE _state = {0};

    // update gamepad
    _ret = vigem_target_x360_update(this->_driver_handler,
                                    gsl::at(this->_gamepads, p_index),
                                    *reinterpret_cast<XUSB_REPORT *>(&_state));

    if (VIGEM_SUCCESS(_ret)) {
      return _ret;
    }
  }

  return W_FAILURE(_ret, "could not clear state of gamepad # " +
                             std::to_string(p_index) +
                             " because: " + s_vigem_error_to_string(_ret));
}

boost::leaf::result<std::tuple<int, VIGEM_ERROR>>
w_virtual_gamepad::add() noexcept {
  if (!this->_driver_handler) {
    return std::make_tuple(-1, VIGEM_ERROR_BUS_NOT_FOUND);
  }

  // allocate handle to identify new pad
  auto _handle = vigem_target_x360_alloc();
  if (_handle == nullptr) {
    return std::make_tuple(-1, VIGEM_ERROR_TARGET_UNINITIALIZED);
  }

  // add client to the bus, this equals a plug-in event
  int _index = -1;
  const auto _ret = vigem_target_add(this->_driver_handler, _handle);
  if (VIGEM_SUCCESS(_ret)) {
    this->_gamepads.push_back(_handle);
    _index = gsl::narrow_cast<int>(this->_gamepads.size() - 1);
  }

  return std::make_tuple(_index, _ret);
}

boost::leaf::result<VIGEM_ERROR>
w_virtual_gamepad::remove(_In_ size_t p_index) noexcept {
  // find the target and remove it
  auto _iter = this->_gamepads.begin() + p_index;
  if (_iter != this->_gamepads.end()) {
    const auto _handle = gsl::at(this->_gamepads, p_index);
    if (_handle != nullptr) {
      vigem_target_remove(this->_driver_handler, _handle);
      vigem_target_free(_handle);
    }
    this->_gamepads.erase(_iter);
    return VIGEM_ERROR::VIGEM_ERROR_NONE;
  }

  constexpr auto _ret = VIGEM_ERROR_TARGET_NOT_PLUGGED_IN;
  return W_FAILURE(_ret, "could not remove gamepad # " +
                             std::to_string(p_index) +
                             " because: " + s_vigem_error_to_string(_ret));
}

boost::leaf::result<VIGEM_ERROR>
w_virtual_gamepad::send_input(_In_ size_t p_index,
                              _In_ XINPUT_STATE &p_xinput) noexcept {
  // send an input to x360 controller
  auto _ret = VIGEM_ERROR::VIGEM_ERROR_TARGET_NOT_PLUGGED_IN;
  if (p_index < this->_gamepads.size()) {
    // update x360 gamepad
    _ret = vigem_target_x360_update(
        this->_driver_handler, gsl::at(this->_gamepads, p_index),
        *reinterpret_cast<XUSB_REPORT *>(&p_xinput.Gamepad));
    if (VIGEM_SUCCESS(_ret)) {
      return VIGEM_ERROR::VIGEM_ERROR_NONE;
    }
  }
  return W_FAILURE(_ret, "could not send an input to the gamepad # " +
                             std::to_string(p_index) +
                             " because: " + s_vigem_error_to_string(_ret));
}

size_t w_virtual_gamepad::get_number_of_gamepads() const noexcept {
  return this->_gamepads.size();
}

void w_virtual_gamepad::_release() noexcept {
  // release all gamepads
  for (auto _handle : this->_gamepads) {
    // remove and free target
    if (_handle != nullptr) {
      vigem_target_remove(this->_driver_handler, _handle);
      vigem_target_free(_handle);
    }
  }
  this->_gamepads.clear();

  // release all
  if (this->_driver_handler) {
    vigem_disconnect(this->_driver_handler);
    vigem_free(this->_driver_handler);
    this->_driver_handler = nullptr;
  }
}



