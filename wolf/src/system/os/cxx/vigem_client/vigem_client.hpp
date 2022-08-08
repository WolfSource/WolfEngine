#pragma once

#ifndef _VIGEM_CLIENT_H_
#define _VIGEM_CLIENT_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "rust/cxx.h"
#include <memory>
#include <windows.h>
#include <Xinput.h>
#include "ViGEm/Client.h"

//forward declaration
struct XINPUT_GAMEPAD_SHARED;
struct XINPUT_STATE_SHARED;
enum class GamepadType : std::uint8_t;

class vigem_client
{
public:
	/**
     * initialize bus driver handler
     */
	vigem_client() noexcept;

    /**
     * initialize bus driver handler
     */
    ~vigem_client();

    /**
     * reset bus driver handler 
     * @return returns true on success
     */
    bool reset_driver() const;

    /**
     * add gamepad controller 
     * @param pGameType, a gamepad type
     * @return returns -1 on error otherwise returns the index of gamepad  
     */
    int32_t add_gamepad_controller(const GamepadType& pGameType) const;

    /**
     * send xbox360 input to gamepad
     * @param pGamePadIndex the index of gamepad
     * @param pXInput the X360 input states
     * @param returns false on error
     */
    bool send_xbox360_input(
        const uint32_t& pGamePadIndex,
        const XINPUT_STATE_SHARED& pXInput) const;

    /**
     * clear the gamepad state
     * @param pGamePadIndex the index of gamepad
     * @param returns false on error
     */
    bool clear_gamepad_state(int pGamePadIndex);

    /**
     * get number of gamepads
     * @return number of gamepads
     */
    uint32_t get_number_of_gamepads() const noexcept;

    /**
     * get last error
     * @return last error
     */
    rust::String get_last_error() const;

private:
	//private constructors for avoid copy constructor
	vigem_client(const vigem_client &) = delete;
	vigem_client &operator=(const vigem_client &) = delete;

    class impl;
    std::unique_ptr<impl>  _impl;
};

std::unique_ptr<vigem_client> New();

#endif //_VIGEM_CLIENT_H_