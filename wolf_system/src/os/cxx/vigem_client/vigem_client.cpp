#include "vigem_client.hpp"
#include "wolf_system/src/os/vigem_client.rs.h"

#include <mutex>
#include <format>

#include <gsl/gsl>

class vigem_client::impl
{
    friend vigem_client;
public:
	impl() noexcept
    {
        constexpr char _trace_info[] = "vigem_client::vigem_client";

        try
        {
            //allocate memory for bus
            this->_driver_handler = vigem_alloc();
            if (!this->_driver_handler)
            {
                this->_last_error = std::format(
                    "could not allocate memory for bus handler. trace info: {}",
                    _trace_info);
                return;
            }

            //connect to the bus of ViGem
            const auto _ret = vigem_connect(this->_driver_handler);
            if (!VIGEM_SUCCESS(_ret))
            {
                this->_last_error = std::format(
                    "gamepad bus connection failed with error code: {:x}. trace info:{}",
                    (int)_ret,
                    _trace_info);
            }
        }
        catch (...) {}
    }

	~impl()
    {
        try
        {
            reset_driver();

            //release all gamepads
            for (auto _handle : this->_gamepads)
            {
                if (_handle)
                {
                    vigem_target_remove(
                        this->_driver_handler,
                        _handle);
                    vigem_target_free(_handle);
                }
            }

            //release driver bus handler
            if (this->_driver_handler)
            {
                vigem_free(this->_driver_handler);
            }
        }
        catch(...)
        {
        }
    }

    bool reset_driver()
    {
         //reset driver
        if (this->_driver_handler)
        {
            vigem_disconnect(this->_driver_handler);
            return true;
        }
        this->_last_error = "could not disconnect ViGem driver bus";
        return false;
    }

    int32_t add_gamepad_controller(const GamepadType& pGameType)
    {
        constexpr char _trace_info[] = "vigem_client::add_gamepad_controller";

        if (!this->_driver_handler)
        {
            this->_last_error = std::format(
                "ViGem driver handler is null. trace info:{}",
                _trace_info);
            return -1;
        }

        // allocate handle to identify new pad
        PVIGEM_TARGET _handler = nullptr;
        if (pGameType == GamepadType::XBox360)
        {
            _handler = vigem_target_x360_alloc();
        }
        else
        {
            _handler = vigem_target_ds4_alloc();
        }

        if (!_handler)
        {
            this->_last_error = std::format(
                "could not allocate memory for {} target. trace info:{}",
                pGameType == GamepadType::XBox360 ? "XBox360" : "PS4",
                _trace_info);
            return -1;
        }

        // add client to the bus, this equals a plug-in event
        const auto _pair = vigem_target_add(
            this->_driver_handler, 
            _handler);
        if (!VIGEM_SUCCESS(_pair))
        {
            // We're done with this pad, free resources (this disconnects the virtual device)
            vigem_target_remove(this->_driver_handler, _handler);
            this->_last_error = std::format(
                "pair with bus failed with error code: {:x}. trace info: {}",
                (int)_pair,
                _trace_info);
            return -1;  
        }

        this->_gamepads.push_back(_handler);

        return gsl::narrow_cast<int>(this->_gamepads.size() - 1);  
    }

    bool send_xbox360_input(
        const uint32_t& pGamePadIndex,
        const XINPUT_STATE_SHARED& pXInput)
    {
        if (pGamePadIndex < this->_gamepads.size())
        {
            XINPUT_STATE _xinput_state;
            _xinput_state.dwPacketNumber = pXInput.dwPacketNumber;
            std::memcpy(
                &(_xinput_state.Gamepad),
                &pXInput,
                sizeof(XINPUT_STATE));

            //update x360 gamepad
            vigem_target_x360_update(
                this->_driver_handler,
                gsl::at(this->_gamepads, pGamePadIndex),
                *reinterpret_cast<XUSB_REPORT*>(&_xinput_state.Gamepad));

            return true;
        }
        this->_last_error = "gamepad index is invalid";
        return false;
    }

    bool clear_gamepad_state(const uint32_t& pGamePadIndex)
    {
        if (pGamePadIndex < this->_gamepads.size())
        {
            XINPUT_STATE _state = {0};
            
            //update gamepad
            vigem_target_x360_update(
                this->_driver_handler,
                gsl::at(this->_gamepads, pGamePadIndex),
                *reinterpret_cast<XUSB_REPORT *>(&_state));
        }
        
        this->_last_error = "gamepad index is invalid";
        return false;
    }

    uint32_t get_number_of_gamepads() const noexcept
    {
        return this->_gamepads.size();
    }

    rust::String get_last_error() const noexcept
    {
        return this->_last_error;
    }

private:
    PVIGEM_CLIENT               _driver_handler;
    std::vector<PVIGEM_TARGET>  _gamepads;
    rust::String                _last_error;
};


vigem_client::vigem_client() noexcept :
    _impl(std::make_unique<vigem_client::impl>())
{
}

vigem_client::~vigem_client()
{
}

bool vigem_client::reset_driver() const
{
    if (!_impl) return false;

    return _impl->reset_driver();
}

int32_t vigem_client::add_gamepad_controller(const GamepadType& pGameType) const
{
    if (!_impl) return -1;

    return _impl->add_gamepad_controller(pGameType);
}

bool vigem_client::send_xbox360_input(
        const uint32_t& pGamePadIndex,
        const XINPUT_STATE_SHARED& pXInput) const
{
    if (!_impl) return false;

    return _impl->send_xbox360_input(pGamePadIndex, pXInput);    
}

bool vigem_client::clear_gamepad_state(int pGamePadIndex)
{
    if (!_impl) return false;

    return _impl->clear_gamepad_state(pGamePadIndex);
}

uint32_t vigem_client::get_number_of_gamepads() const noexcept
{
    if (!_impl) return 0;

    return _impl->get_number_of_gamepads();
}

rust::String vigem_client::get_last_error() const
{
    if (!_impl) return "";

    return _impl->get_last_error();
}

std::unique_ptr<vigem_client> New()
{
    return std::make_unique<vigem_client>();
}