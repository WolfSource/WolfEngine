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

    struct XINPUT_GAMEPAD_SHARED;
    typedef struct XINPUT_GAMEPAD_SHARED* XINPUT_GAMEPAD_SHARED_PTR;

    struct XINPUT_STATE_SHARED;
    typedef struct XINPUT_STATE_SHARED* XINPUT_STATE_SHARED_PTR;

   /**
    * initialize the ViGEm client
    * @param p_vigem, the vigem client object
    * @param p_error, the error buffer
    * @returns zero on success
    */
    W_API
        int w_vigem_client_init(_Inout_ w_vigem_client* p_vigem, _Inout_ char* p_error);
        
   /**
    * disconnects from the vigem bus device and resets the driver object state. 
    * The driver object may be reused again after calling this function
    * @param p_vigem, the vigem client object
    * @param p_error, the error buffer
    * @returns zero on success
    */
    W_API
        int w_vigem_client_reset(_Inout_ w_vigem_client p_vigem, _Inout_ char* p_error);

   /**
    * get number of gamepads.
    * @param p_vigem, the vigem client object
    * @returns number of gamepads
    */
    W_API
        size_t w_vigem_get_number_of_gamepads(_In_ w_vigem_client p_vigem);

    /**
     * clear gamepad state
     * @param p_vigem, the vigem client object
     * @param p_index, the gamepad state
     * @returns zero on success
     */
    W_API
        int w_vigem_clear_gamepad_state(_In_ w_vigem_client p_vigem, _In_ size_t p_index);

   /**
    * add a gamepad 
    * @param p_vigem, the vigem client object
    * @param p_error, the error buffer
    * @returns gamepad index on success and -1 on failure 
    */
    W_API
        int w_vigem_add_gamepad(_In_ w_vigem_client p_vigem, _Inout_ char* p_error);

   /**
    * add a gamepad
    * @param p_vigem, the vigem client object
    * @param p_index, the index of gamepad
    * @param p_xinput, the xinput data
    * @param p_error, the error buffer
    * @returns zero on success
    */
    W_API
        int w_vigem_send_input(
            _In_ w_vigem_client p_vigem,
            _In_ size_t p_index,
            _In_ XINPUT_GAMEPAD_SHARED_PTR p_xinput,
            _Inout_ char* p_error);

   /**
    * release the vigem resources.
    * @param p_vigem, the vigem client object
    * @param p_error, the error buffer
    * @returns zero on success
    */
    W_API
        int w_vigem_client_fini(_Inout_ w_vigem_client* p_vigem, _Inout_ char* p_error);

#ifdef __cplusplus
}
#endif