#include "vigem_client.h"
#include <wolf.hpp>

#include <vector>
#include <Xinput.h>

#include <DISABLE_ANALYSIS_BEGIN>
#include "ViGEm/Client.h"
#include <DISABLE_ANALYSIS_END>

struct w_vigem_client_t
{
    PVIGEM_CLIENT				driver_handler;
	std::vector<PVIGEM_TARGET>  gamepads;
};

constexpr auto s_vigem_get_last_error(_In_ VIGEM_ERROR p_error_code)
{
    switch (p_error_code)
    {
    default:
        return "VIGEM_UNKNOWN_ERROR";
    case VIGEM_ERROR_NONE:
        return "VIGEM_ERROR_NONE";
    case VIGEM_ERROR_BUS_NOT_FOUND:
        return "VIGEM_ERROR_BUS_NOT_FOUND";
    case VIGEM_ERROR_NO_FREE_SLOT:
        return "VIGEM_ERROR_NO_FREE_SLOT";
    case VIGEM_ERROR_INVALID_TARGET:
        return "VIGEM_ERROR_INVALID_TARGET";
    case VIGEM_ERROR_REMOVAL_FAILED:
        return "VIGEM_ERROR_REMOVAL_FAILED";
    case VIGEM_ERROR_ALREADY_CONNECTED:
        return "VIGEM_ERROR_ALREADY_CONNECTED";
    case VIGEM_ERROR_TARGET_UNINITIALIZED:
        return "VIGEM_ERROR_TARGET_UNINITIALIZED";
    case VIGEM_ERROR_TARGET_NOT_PLUGGED_IN:
        return "VIGEM_ERROR_TARGET_NOT_PLUGGED_IN";
    case VIGEM_ERROR_BUS_VERSION_MISMATCH:
        return "VIGEM_ERROR_BUS_VERSION_MISMATCH";
    case VIGEM_ERROR_BUS_ACCESS_FAILED:
        return "VIGEM_ERROR_BUS_ACCESS_FAILED";
    case VIGEM_ERROR_CALLBACK_ALREADY_REGISTERED:
        return "VIGEM_ERROR_CALLBACK_ALREADY_REGISTERED";
    case VIGEM_ERROR_CALLBACK_NOT_FOUND:
        return "VIGEM_ERROR_CALLBACK_NOT_FOUND";
    case VIGEM_ERROR_BUS_ALREADY_CONNECTED:
        return "VIGEM_ERROR_BUS_ALREADY_CONNECTED";
    case VIGEM_ERROR_BUS_INVALID_HANDLE:
        return "VIGEM_ERROR_BUS_INVALID_HANDLE";
    case VIGEM_ERROR_XUSB_USERINDEX_OUT_OF_RANGE:
        return "VIGEM_ERROR_XUSB_USERINDEX_OUT_OF_RANGE";
    case VIGEM_ERROR_INVALID_PARAMETER:
        return "VIGEM_ERROR_INVALID_PARAMETER";
    case VIGEM_ERROR_NOT_SUPPORTED:
        return "VIGEM_ERROR_NOT_SUPPORTED";
    case VIGEM_ERROR_WINAPI:
        return "VIGEM_ERROR_WINAPI";
    case VIGEM_ERROR_TIMED_OUT:
        return "VIGEM_ERROR_TIMED_OUT";
    }
}

int w_vigem_client_init(_Inout_ w_vigem_client* p_vigem, _Inout_ char* p_error)
{
    constexpr auto TRACE = "w_vigem_client_init";
    const auto _error_nn = gsl::not_null<char*>(p_error);
    auto _ptr = (w_vigem_client_t*)calloc(1, sizeof(w_vigem_client_t));
    const auto _vigem = gsl::not_null<w_vigem_client_t*>(_ptr);
    auto _ret = 0;

    defer _(nullptr, [&](...)
        {
            *p_vigem = _vigem.get();
            if (_ret != 0)
            {
                w_vigem_client_fini(p_vigem, p_error);
            }
        });


    // allocate memory for bus driver
    _vigem->driver_handler = vigem_alloc();
    if (!_vigem->driver_handler)
    {
        std::ignore = snprintf(
            _error_nn,
            W_MAX_PATH,
            "could not allocate vigem for bus driver handle. trace info: %s",
            TRACE);
        return _ret = -1;
    }

    //connect to the bus of ViGem
    const auto _vigem_res = vigem_connect(_vigem->driver_handler);
    if (!VIGEM_SUCCESS(_vigem_res))
    {
        auto error = s_vigem_get_last_error(_vigem_res);
        std::ignore = snprintf(
            _error_nn,
            W_MAX_PATH,
            "could not connect to bus driver handle because: %s. trace info: %s",
            error,
            TRACE);
        return _ret = -1;
    }

    return _ret;
}

int w_vigem_client_reset(_Inout_ w_vigem_client p_vigem, _Inout_ char* p_error)
{
    constexpr auto TRACE = "w_vigem_client_reset";
    const auto _vigem_ptr_nn = gsl::not_null<w_vigem_client>(p_vigem);
    const auto _error_nn = gsl::not_null<char*>(p_error);

    //reset driver
    if (_vigem_ptr_nn->driver_handler)
    {
        vigem_disconnect(_vigem_ptr_nn->driver_handler);
        return 0;
    }

    std::ignore = snprintf(
        _error_nn,
        W_MAX_PATH,
        "could not disconnect vigem. trace info: %s",
        TRACE);

    return -1;
}

size_t w_vigem_get_number_of_gamepads(_In_ w_vigem_client p_vigem)
{
    const auto _vigem_nn = gsl::not_null<w_vigem_client>(p_vigem);
    return _vigem_nn->gamepads.size();
}

int w_vigem_clear_gamepad_state(_In_ w_vigem_client p_vigem, _In_ size_t p_index)
{
    const auto _vigem_nn = gsl::not_null<w_vigem_client>(p_vigem);

    if (p_index < _vigem_nn->gamepads.size())
    {
        XINPUT_STATE _state = { 0 };

        // update gamepad
        vigem_target_x360_update(
            _vigem_nn->driver_handler,
            gsl::at(_vigem_nn->gamepads, p_index),
            *reinterpret_cast<XUSB_REPORT*>(&_state));

        return 0;
    }

    return -1;
}

int w_vigem_add_gamepad(_In_ w_vigem_client p_vigem, _Inout_ char* p_error)
{
    constexpr auto TRACE = "w_vigem_add_gamepad";

    const auto _vigem_nn = gsl::not_null<w_vigem_client>(p_vigem);
    const auto _error_nn = gsl::not_null<char*>(p_error);

    if (!_vigem_nn->driver_handler)
    {
        std::ignore = snprintf(
            _error_nn,
            W_MAX_PATH,
            "ViGem driver handler is null. trace info: %s",
            TRACE);
        return -1;
    }

    // allocate handle to identify new pad
    auto _handler = vigem_target_x360_alloc();
    if (_handler == nullptr)
    {
        std::ignore = snprintf(
            _error_nn,
            W_MAX_PATH,
            "could not allocate memory for gamepad. trace info: %s",
            TRACE);

        return -1;
    }

    // add client to the bus, this equals a plug-in event
    const auto _vigem_res = vigem_target_add(
        _vigem_nn->driver_handler,
        _handler);
    if (!VIGEM_SUCCESS(_vigem_res))
    {
        const auto _error = s_vigem_get_last_error(_vigem_res);

        // we're done with this pad, free resources (this disconnects the virtual device)
        vigem_target_remove(_vigem_nn->driver_handler, _handler);

        std::ignore = snprintf(
            _error_nn,
            W_MAX_PATH,
            "could not pair with bus because %s. trace info: %s",
            _error,
            TRACE);

        return -1;
    }

    _vigem_nn->gamepads.push_back(_handler);
    return gsl::narrow_cast<int>(_vigem_nn->gamepads.size() - 1);
}

int w_vigem_send_input(
    _In_ w_vigem_client p_vigem,
    _In_ size_t p_index,
    _In_ XINPUT_GAMEPAD_SHARED_PTR p_xinput,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "w_vigem_send_input";

    const auto _vigem_nn = gsl::not_null<w_vigem_client>(p_vigem);
    const auto _xinput_nn = gsl::not_null<XINPUT_GAMEPAD_SHARED_PTR>(p_xinput);
    const auto _error_nn = gsl::not_null<char*>(p_error);

    if (p_index < _vigem_nn->gamepads.size())
    {
        XINPUT_STATE _xinput_state;
        std::memcpy(
            &(_xinput_state),
            _xinput_nn,
            sizeof(XINPUT_STATE));

        // update x360 gamepad
        vigem_target_x360_update(
            _vigem_nn->driver_handler,
            gsl::at(_vigem_nn->gamepads, p_index),
            *reinterpret_cast<XUSB_REPORT*>(&_xinput_state.Gamepad));

        return 0;
    }

    std::ignore = snprintf(
        _error_nn,
        W_MAX_PATH,
        "could not send an input to the gamepad(%lld). trace info: %s",
        p_index,
        TRACE);

    return -1;
}

int w_vigem_client_fini(_Inout_ w_vigem_client* p_vigem, _Inout_ char* p_error)
{
    const auto _vigem_ptr_nn = gsl::not_null<w_vigem_client*>(p_vigem);
    const auto _vigem_nn = gsl::not_null<w_vigem_client>(*_vigem_ptr_nn);

    const auto _ret = w_vigem_client_reset(_vigem_nn, p_error);
    
    //release all gamepads
    for (auto _handle : _vigem_nn->gamepads)
    {
        if (_handle != nullptr)
        {
            vigem_target_remove(
                _vigem_nn->driver_handler,
                _handle);
            vigem_target_free(_handle);
        }
    }

    // now release it
    if (_vigem_nn->driver_handler)
    {
        vigem_free(_vigem_nn->driver_handler);
    }

    free(_vigem_nn);
    *p_vigem = nullptr;

    return _ret;
}