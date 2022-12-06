/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    struct w_openal_t;
    typedef struct w_openal_t* w_openal;

    typedef int (*w_openal_callback)(void* /*userptr*/, void* /*p_data*/, int /*p_size*/);

    /**
     * returns all devices
     * @param p_output_devices, the ffmpeg AVFrame
     * @param p_input_devices, the pixel format of ffmpeg AVFrame
     */
    W_API
        void w_openal_list_all_devices(
            _Inout_ char* p_output_devices,
            _Inout_ char* p_input_devices);

    /**
     * initialize the openal object
     * @param p_openal, an object
     * @param p_format, the format of sound
     * @param p_sample_rate, the sample rate
     * @param p_refresh, the refresh rate
     * @param p_number_of_channels, the number of channels
     * @param p_error, the error buffer
     * returns zero on success
     */
    W_API
        int w_openal_init(
            _Inout_ w_openal* p_openal,
            _In_ int p_format,
            _In_ int p_sample_rate,
            _In_ int p_refresh,
            _In_ int p_number_of_channels,
            _Inout_ char* p_error);

   /**
    * open the openal
    * @param p_openal, an openal object
    * @param p_error, the error buffer
    * @returns zero on success
    */
    W_API
        int w_openal_open(
            _In_ w_openal p_openal,
            _Inout_ char* p_error);

   /**
    * update the openal
    * @param p_openal, an openal object
    * @param p_audio_frame_buffer, the audio frame buffer
    * @param p_audio_frame_buffer_len, the length of audio frame buffer
    * @param p_error, the error buffer
    * @returns zero on success
    */
    W_API
        int w_openal_update_i16(
            _In_ w_openal p_openal,
            _In_ const int16_t* p_audio_frame_buffer,
            _In_ size_t p_audio_frame_buffer_len,
            _Inout_ char* p_error);

   /**
    * reset the openal
    * @param p_openal, an openal object
    * @returns void
    */
    W_API
        void w_openal_reset(_In_ w_openal p_openal);

    /**
     * close the openal
     * @param p_openal, an openal object
     * @param p_force_close, force to close
     * @returns void
     */
    W_API
        void w_openal_close(_In_ w_openal p_openal, bool p_force_close);

    /**
     * release resources of openal
     * @param p_openal, the openal object
     */
    W_API
        void w_openal_fini(_Inout_ w_openal* p_openal);

#ifdef __cplusplus
}
#endif
