#include "w_system_pch.h"
#include "w_inputs_manager.h"

using namespace wolf::system;

w_inputs_manager::w_inputs_manager()
{
    reset();
}

w_inputs_manager::~w_inputs_manager()
{
    reset();
}

void w_inputs_manager::reset()
{
    this->mouse.left_button_pressed = false;
    this->mouse.left_button_released = false;
    this->mouse.middle_button_pressed = false;
    this->mouse.middle_button_released = false;
    this->mouse.right_button_pressed = false;
    this->mouse.right_button_released = false;
    this->mouse.wheel = 0;
    this->mouse.pos_x = 0;
    this->mouse.pos_y = 0;
    this->mouse.last_pos_x = 0;
    this->mouse.last_pos_y = 0;
    this->keyboard.keys_pressed.clear();
    this->keyboard.keys_released.clear();
    this->keyboard.inputed_chars.clear();
}

#ifdef __WIN32

HRESULT w_inputs_manager::update(
    _In_ const HWND pHWND,
    _In_ const UINT pMessage,
    _In_ const WPARAM pWParam,
    _In_ const LPARAM pLParam)
{
    if (this->mouse.left_button_released) this->mouse.left_button_released = false;
    if (this->mouse.middle_button_released) this->mouse.middle_button_released = false;
    if (this->mouse.right_button_released) this->mouse.right_button_released = false;

    this->keyboard.inputed_chars.clear();
    this->keyboard.keys_released.clear();

    switch (pMessage)
    {
    case WM_LBUTTONDOWN:
        this->mouse.left_button_pressed = true;
        return true;
    case WM_LBUTTONUP:
        this->mouse.left_button_pressed = false;
        this->mouse.left_button_released = true;
        return true;
    case WM_RBUTTONDOWN:
        this->mouse.right_button_pressed = true;
        return true;
    case WM_RBUTTONUP:
        this->mouse.right_button_pressed = false;
        this->mouse.right_button_released = true;
        return true;
    case WM_MBUTTONDOWN:
        this->mouse.middle_button_pressed = true;
        return true;
    case WM_MBUTTONUP:
        this->mouse.middle_button_pressed = false;
        this->mouse.middle_button_released = true;
        return true;
    case WM_MOUSEWHEEL:
        this->mouse.wheel += GET_WHEEL_DELTA_WPARAM(pWParam) > 0 ? +1.0f : -1.0f;
        return true;
    case WM_MOUSEMOVE:
        this->mouse.last_pos_x = this->mouse.pos_x;
        this->mouse.last_pos_y = this->mouse.pos_y;
        this->mouse.pos_x = static_cast<signed short>(pLParam);
        this->mouse.pos_y = static_cast<signed short>(pLParam >> 16);
        return true;
    case WM_KEYDOWN:
        this->keyboard.keys_pressed.insert((int)pWParam);
        return true;
    case WM_KEYUP:
    {
        auto _value = (int)pWParam;
        this->keyboard.keys_pressed.erase(_value);
        this->keyboard.keys_released.insert(_value);
        return true;
    }
    case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (pWParam > 0 && pWParam < 0x10000)
        {
            this->keyboard.inputed_chars.push_back((unsigned short)pWParam);
        }
        return true;
    }
    return 0;
}

#endif

std::vector<bool> w_inputs_manager::is_keys_pressed(_In_ std::initializer_list<int> pKeys)
{
    std::vector<bool> _results(pKeys.size());

    size_t i = 0;
    for (auto& _key : pKeys)
    {
        bool _yay = false;
        for (auto& _pressed : this->keyboard.keys_pressed)
        {
            if (_key == _pressed)
            {
                _yay = true;
                break;
            }
        }
        _results[i++] = _yay;
    }

    return _results;
}

std::vector<bool> w_inputs_manager::is_keys_released(_In_ std::initializer_list<int> pKeys)
{
    std::vector<bool> _results(pKeys.size());

    size_t i = 0;
    for (auto& _key : pKeys)
    {
        bool _yay = false;
        for (auto& _released : this->keyboard.keys_released)
        {
            if (_key == _released)
            {
                _yay = true;
                break;
            }
        }
        _results[i++] = _yay;
    }

    return _results;
}
