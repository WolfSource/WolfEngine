/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    struct AVFrame;
    typedef struct AVFrame* w_av_frame;

    /**
     * initialize the ffmpeg AVFrame
     * @param p_frame, the ffmpeg AVFrame
     * @param p_pixel_format, the pixel format of ffmpeg AVFrame
     * @param p_width, the width of ffmpeg AVFrame
     * @param p_height, the height of ffmpeg AVFrame
     * @param p_alignment, the alignment 
     * @param p_data, the initial data of ffmpeg AVFrame
     * @param p_error, the error buffer
     * @returns zero on success
     */
    W_API
        int w_av_frame_init(
            _Inout_ w_av_frame* p_frame,
            _In_ uint32_t p_pixel_format,
            _In_ uint32_t p_width,
            _In_ uint32_t p_height,
            _In_ uint32_t p_alignment,
            _In_ uint8_t* p_data,
            _Inout_ char* p_error);

    /**
     * initialize the ffmpeg AVFrame
     * @param p_frame, the ffmpeg AVFrame
     * @param p_data, the initial data of ffmpeg AVFrame
     * @param p_alignment, the alignment
     * @param p_error, the error buffer
     * @returns zero on success
     */
    int w_av_set_data(
        _In_ w_av_frame p_frame,
        _In_ uint8_t* p_data,
        _In_ int p_alignment,
        _Inout_ char* p_error);

    /**
     * initialize the ffmpeg AVFrame
     * @param p_pixel_format, the pixel format of ffmpeg AVFrame
     * @param p_width, the width of ffmpeg AVFrame
     * @param p_height, the height of ffmpeg AVFrame
     * @param p_alignment, the aligmnet which is usually 1
     * @returns zero on success
     */
    W_API
        int w_av_get_required_buffer_size(
            _In_ uint32_t p_pixel_format,
            _In_ uint32_t p_width,
            _In_ uint32_t p_height,
            _In_ uint32_t p_alignment);

    /**
      * convert the ffmpeg AVFrame
      * @param p_frame, the ffmpeg AVFrame
      * @param p_error, the error buffer
      * @returns zero on success
      */
    W_API
        int w_av_frame_convert(
            _In_ w_av_frame p_src_frame,
            _Inout_ w_av_frame* p_dst_frame,
            _Inout_ char* p_error);

    /**
     * release all ffmpeg resources
     * @param p_ffmpeg, the ffmpeg AVFrame
     */
    W_API
        void w_av_frame_fini(_Inout_ w_av_frame* p_ffmpeg);

#ifdef __cplusplus
}
#endif