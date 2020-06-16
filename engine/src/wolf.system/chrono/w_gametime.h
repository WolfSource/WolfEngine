/*
    Project            : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source             : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website            : https://WolfEngine.App
    Name               : w_game_time.h
    Description        : Manage game time
    Comment            :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include "w_chrono.h"

//forward declaration
struct w_gametime_imp;
typedef struct w_gametime_imp* w_gametime;
typedef void w_gametime_tick_callback(w_gametime);

/**
 * Get elapsed ticks since the previous tick call.
 * @return pointer to object of w_gametime
*/
w_gametime w_gametime_init(void);

/**
 * reset game time
 */
void w_game_time_reset(_In_ w_gametime pGameTime);

/**
 * Get elapsed ticks since the previous tick call.
 * @return elapsed ticks
*/
uint64_t w_gametime_get_elapsed_ticks(_In_ w_gametime pGameTime);

/**
 * Get elapsed seconds since the previous tick call.
 * @return elapsed time
*/
double w_gametime_get_elapsvoided_seconds(_In_ w_gametime pGameTime);

/**
 * Get total ticks since the start.
 * @return elapsed total ticks
*/
uint64_t w_gametime_get_total_ticks(_In_ w_gametime pGameTime);

/**
 * Get total seconds since the start.
 * @return elapsed total seconds
*/
double w_gametime_get_total_seconds(_In_ w_gametime pGameTime);

/**
 * Get total frame count(s).
 * @return number of frames
*/
uint32_t w_gametime_get_frame_count(_In_ w_gametime pGameTime);

/**
 * Get the current number of framerate.
 * @return framerate
*/
uint32_t w_gametime_get_frames_per_second(_In_ w_gametime pGameTime);

/**
 * Get the fixed time step.
 * @return is fixed time step
*/
bool w_gametime_get_fixed_time_step(_In_ w_gametime pGameTime);

/**
 * Set fixed time step.
*/
void w_gametime_enable_fixed_time_step(_In_ w_gametime pGameTime);

/**
 * Set fixed time step.
*/
void w_gametime_disable_fixed_time_step(_In_ w_gametime pGameTime);

/**
 * Set how often to call tick callback during fixed timestep mode.
*/
void w_gametime_set_target_elapsed_ticks(_In_ w_gametime pGameTime, _In_ uint64_t pValue);

/**
 * Set how often to call tick callback during fixed timestep mode.
*/
void w_gametime_set_target_elapsed_seconds(_In_ w_gametime pGameTime, _In_ double pValue);

/**
 * tick function
 */
void w_gametime_tick(_In_ w_gametime pGameTime, _In_ w_gametime_tick_callback pOnTickCallBack);

#ifdef __cplusplus
}
#endif
