#if defined(WOLF_SYSTEM_GAMEPAD_VIRTUAL) && !defined(EMSCRIPTEN)

#include "w_gamepad_virtual_pool.hpp"

using w_gamepad_virtual = wolf::system::gamepad::w_gamepad_virtual;
using w_gamepad_virtual_pool = wolf::system::gamepad::w_gamepad_virtual_pool;

static PVIGEM_CLIENT s_driver_handle = nullptr;

boost::leaf::result<VIGEM_ERROR> w_gamepad_virtual_pool::init() noexcept {
  // check for driver handle memory
  if (s_driver_handle == nullptr) {
    // allocate memory for bus driver
    s_driver_handle = vigem_alloc();
    if (s_driver_handle == nullptr) {
      return W_FAILURE(std::errc::not_enough_memory,
                       "could not allocate memory for ViGem client");
    }
  }

  // connect to vigem
  const auto _ret = vigem_connect(s_driver_handle);
  if (_ret != VIGEM_ERROR_NONE) {
    // release it
    fini();
    return _ret;
  }

  return _ret;
}

boost::leaf::result<w_gamepad_virtual>
w_gamepad_virtual_pool::create() noexcept {
  if (s_driver_handle == nullptr) {
    return W_FAILURE(std::errc::host_unreachable,
                     "could not allocate memory for ViGem client");
  }

  // allocate handle to identify new pad
  auto _handle = vigem_target_x360_alloc();
  if (_handle == nullptr) {
    return W_FAILURE(std::errc::not_enough_memory,
                     "could not allocate memory for ViGem target client");
  }

  // add client to the bus, this equals a plug-in event
  const auto _ret = vigem_target_add(s_driver_handle, _handle);
  if (_ret != VIGEM_ERROR_NONE) {
    return W_FAILURE(
        std::errc::not_enough_memory,
        "could not create ViGEm target device to bus driver because: " +
            vigem_error_to_string(_ret));
  }

  return w_gamepad_virtual(s_driver_handle, _handle);
}

void w_gamepad_virtual_pool::fini() noexcept {
  if (s_driver_handle) {
    vigem_disconnect(s_driver_handle);
    vigem_free(s_driver_handle);
    s_driver_handle = nullptr;
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



