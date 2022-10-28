/*
        Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    struct w_vigem_client_t;
    typedef struct w_vigem_client_t* w_vigem_client;

    typedef struct xinput_gamepad_t
    {
        uint16_t buttons;
        uint8_t left_trigger;
        uint8_t right_trigger;
        int16_t thumb_lx;
        int16_t thumb_ly;
        int16_t thumb_rx;
        int16_t thumb_ry;
    } xinput_gamepad_t;
    typedef struct xinput_gamepad_t* xinput_gamepad;

    typedef struct xinput_state_t {
        uint16_t packet_number;
        xinput_gamepad_t gamepad;
    } xinput_state_t;
    typedef struct xinput_state_t* xinput_state;

   /**
    * initialize the ViGEm client and connect to the bus
    * @param p_vigem, the vigem client object
    * @returns zero on success
    */
    W_API
        WRESULT w_vigem_client_init(_Inout_ w_vigem_client* p_vigem);
        
    /**
     * clear gamepad state
     * @param p_vigem, the vigem client object
     * @param p_index, the gamepad state
     * @returns zero on success
     */
    W_API
        WRESULT w_vigem_clear_gamepad_state(_In_ w_vigem_client p_vigem, _In_ size_t p_index);

   /**
    * add a gamepad 
    * @param p_vigem, the vigem client object
    * @param p_index, the the index of new gamepad
    * @returns zero on success
    */
    W_API
        WRESULT w_vigem_add_gamepad(_In_ w_vigem_client p_vigem, _Inout_ size_t* p_index);

   /**
    * remove a gamepad
    * @param p_vigem, the vigem client object
    * @param p_index, index of gamepad
    * @returns zero on success
    */
    W_API
        WRESULT w_vigem_remove_gamepad(_In_ w_vigem_client p_vigem, _In_ size_t p_index);

   /**
    * add a gamepad
    * @param p_vigem, the vigem client object
    * @param p_index, the index of gamepad
    * @param p_xinput, the xinput data
    * @returns zero on success
    */
    W_API
        WRESULT w_vigem_send_input(
            _In_ w_vigem_client p_vigem,
            _In_ size_t p_index,
            _In_ xinput_state p_xinput);

   /**
    * get number of gamepads.
    * @param p_vigem, the vigem client object
    * @returns number of gamepads
    */
    W_API
        size_t w_vigem_get_number_of_gamepads(_In_ w_vigem_client p_vigem);

   /**
    * disconnects from the ViGEm bus device and resets the driver object state and release the ViGEm client resources.
    * @param p_vigem, the vigem client object
    * @returns void
    */
    W_API
        void w_vigem_client_fini(_Inout_ w_vigem_client* p_vigem);

#ifdef __cplusplus
}
#endif
