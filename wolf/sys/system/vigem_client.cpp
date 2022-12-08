#ifdef WIN32

#include "vigem_client.h"
#include <wolf.hpp>

#include <vector>
#include <Xinput.h>

#include <DISABLE_ANALYSIS_BEGIN>
#include "ViGEm/Client.h"
#include <DISABLE_ANALYSIS_END>

struct w_vigem_client_t
{
    PVIGEM_CLIENT				driver_handler = nullptr;
	std::vector<PVIGEM_TARGET>  gamepads;
};

W_RESULT w_vigem_client_init(_Inout_ w_vigem_client* p_vigem)
{
    W_RESULT _ret = 0;

    auto _ptr = new (std::nothrow) w_vigem_client_t();
    const auto _vigem = gsl::not_null<w_vigem_client_t*>(_ptr);

    defer _(nullptr, [&](...)
        {
            *p_vigem = _vigem.get();
            if (_ret != 0)
            {
                w_vigem_client_fini(p_vigem);
            }
        });

    // allocate memory for bus driver
    _vigem->driver_handler = vigem_alloc();
    if (!_vigem->driver_handler)
    {
        _ret = -1;
    }

    const auto _res = vigem_connect(_vigem->driver_handler);
    if (VIGEM_SUCCESS(_res))
    {
        return W_SUCCESS;
    }

    return gsl::narrow_cast<W_RESULT>(_res);
}

W_RESULT w_vigem_clear_gamepad_state(_In_ w_vigem_client p_vigem, _In_ size_t p_index)
{
    const auto _vigem_nn = gsl::not_null<w_vigem_client>(p_vigem);

    if (p_index < _vigem_nn->gamepads.size())
    {
        XINPUT_STATE _state = { 0 };

        // update gamepad
        const auto _res = vigem_target_x360_update(
            _vigem_nn->driver_handler,
            gsl::at(_vigem_nn->gamepads, p_index),
            *reinterpret_cast<XUSB_REPORT*>(&_state));
        if (VIGEM_SUCCESS(_res))
        {
            return W_SUCCESS;
        }
        return gsl::narrow_cast<W_RESULT>(_res);
    }

    return W_FAILURE;
}

W_RESULT w_vigem_add_gamepad(_In_ w_vigem_client p_vigem, _Inout_ size_t* p_index)
{
    const auto _vigem_nn = gsl::not_null<w_vigem_client>(p_vigem);
    const auto _index = gsl::not_null<size_t*>(p_index);
    *_index = 0;

    if (!_vigem_nn->driver_handler)
    {
        return gsl::narrow<W_RESULT>(VIGEM_ERROR_BUS_NOT_FOUND);
    }

    // allocate handle to identify new pad
    auto _handle = vigem_target_x360_alloc();
    if (_handle == nullptr)
    {
        return gsl::narrow<W_RESULT>(VIGEM_ERROR_TARGET_UNINITIALIZED);
    }

    // add client to the bus, this equals a plug-in event
    const auto _vigem_res = vigem_target_add(
        _vigem_nn->driver_handler,
        _handle);
    if (VIGEM_SUCCESS(_vigem_res))
    {
        _vigem_nn->gamepads.push_back(_handle);
        *_index = gsl::narrow_cast<int>(_vigem_nn->gamepads.size() - 1);
        return W_SUCCESS;
    }

    // we're done with this pad, free resources (this disconnects the virtual device)
    std::ignore = vigem_target_remove(_vigem_nn->driver_handler, _handle);
    vigem_target_free(_handle);
    return gsl::narrow_cast<W_RESULT>(_vigem_res);
}

W_RESULT w_vigem_remove_gamepad(_In_ w_vigem_client p_vigem, _In_ size_t p_index)
{
    const auto _vigem_nn = gsl::not_null<w_vigem_client>(p_vigem);

    auto _iter = _vigem_nn->gamepads.begin() + p_index;
    if (_iter != _vigem_nn->gamepads.end())
    {
        const auto _handle = gsl::at(_vigem_nn->gamepads, p_index);
        if (_handle != nullptr)
        {
            vigem_target_remove(
                _vigem_nn->driver_handler,
                _handle);
            vigem_target_free(_handle);
        }
        _vigem_nn->gamepads.erase(_iter);

        return W_SUCCESS;
    }

    return W_FAILURE;

}

W_RESULT w_vigem_send_input(
    _In_ w_vigem_client p_vigem,
    _In_ size_t p_index,
    _In_ xinput_state p_xinput)
{
    const auto _vigem_nn = gsl::not_null<w_vigem_client>(p_vigem);
    const auto _xinput_nn = gsl::not_null<const xinput_state>(p_xinput);
    
    if (p_index < _vigem_nn->gamepads.size())
    {
        XINPUT_STATE _xinput_state;

        std::memcpy(
            &(_xinput_state),
            _xinput_nn,
            sizeof(XINPUT_STATE));

        // update x360 gamepad
        const auto _res = vigem_target_x360_update(
            _vigem_nn->driver_handler,
            gsl::at(_vigem_nn->gamepads, p_index),
            *reinterpret_cast<XUSB_REPORT*>(&_xinput_state.Gamepad));
        if (VIGEM_SUCCESS(_res))
        {
            return W_SUCCESS;
        }
        return gsl::narrow_cast<W_RESULT>(_res);
    }

    return W_FAILURE;
}

size_t w_vigem_get_number_of_gamepads(_In_ w_vigem_client p_vigem)
{
    const auto _vigem_nn = gsl::not_null<w_vigem_client>(p_vigem);
    return _vigem_nn->gamepads.size();
}

void w_vigem_client_fini(_Inout_ w_vigem_client* p_vigem)
{
    const auto _vigem_ptr_nn = gsl::not_null<w_vigem_client*>(p_vigem);
    const auto _vigem_nn = gsl::not_null<w_vigem_client>(*_vigem_ptr_nn);

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
    _vigem_nn->gamepads.clear();

    // release all
    if (_vigem_nn->driver_handler)
    {
        vigem_disconnect(_vigem_nn->driver_handler);
        vigem_free(_vigem_nn->driver_handler);
        _vigem_nn->driver_handler = nullptr;
    }

    delete _vigem_nn.get();
    *p_vigem = nullptr;
}

#endif //WIN32