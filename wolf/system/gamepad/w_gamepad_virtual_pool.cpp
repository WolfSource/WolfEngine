#if defined(WOLF_SYSTEM_GAMEPAD_VIRTUAL) && !defined(EMSCRIPTEN)

#include "w_gamepad_virtual_pool.hpp"
#include <shared_mutex>

using w_gamepad_virtual = wolf::system::gamepad::w_gamepad_virtual;
using w_gamepad_virtual_pool = wolf::system::gamepad::w_gamepad_virtual_pool;
using w_gamepad_virtual_bus = wolf::system::gamepad::w_gamepad_virtual_bus;

static auto s_bus = std::make_shared<w_gamepad_virtual_bus>();

boost::leaf::result<VIGEM_ERROR> w_gamepad_virtual_pool::init() noexcept {
  std::unique_lock lock(s_bus->mutex);

  // check for driver handle memory
  if (s_bus->driver_handle == nullptr) {
    // allocate memory for bus driver
    s_bus->driver_handle = vigem_alloc();
    if (s_bus->driver_handle == nullptr) {
      return W_FAILURE(std::errc::not_enough_memory, "could not allocate memory for ViGem client");
    }
  }

  // connect to vigem
  const auto _ret = vigem_connect(s_bus->driver_handle);
  if (_ret != VIGEM_ERROR_NONE) {
    // release it
    fini();
    return _ret;
  }

  return _ret;
}

boost::leaf::result<w_gamepad_virtual> w_gamepad_virtual_pool::add() noexcept {
  auto _gamepad = w_gamepad_virtual(s_bus);
  BOOST_LEAF_AUTO(init_res, _gamepad.init());
  return _gamepad;
}

void w_gamepad_virtual_pool::fini() noexcept {
  std::unique_lock lock(s_bus->mutex);

  if (s_bus->driver_handle) {
    vigem_disconnect(s_bus->driver_handle);
    vigem_free(s_bus->driver_handle);
    s_bus->driver_handle = nullptr;
  }
}

std::string w_gamepad_virtual_pool::vigem_error_to_string(
    _In_ const VIGEM_ERROR p_error) noexcept {
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

#endif // #if defined(WOLF_SYSTEM_VIRTUAL_GAMEPAD) && !defined(EMSCRIPTEN)



