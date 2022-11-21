/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#ifdef WOLF_MEDIA_FFMPEG

#pragma once

#include <wolf.hpp>

//
//    struct AVFrame;
//    typedef struct AVFrame* w_av_frame;
//
//    /**
//     * initialize the ffmpeg AVFrame
//     * @param p_avframe, the ffmpeg AVFrame
//     * @param p_pixel_format, the pixel format of ffmpeg AVFrame
//     * @param p_width, the width of ffmpeg AVFrame
//     * @param p_height, the height of ffmpeg AVFrame
//     * @param p_alignment, the alignment
//     * @param p_data, the initial data of ffmpeg AVFrame
//     * @param p_error, the error buffer
//     * @returns zero on success
//     */
//    W_API
//        int w_av_frame_init(
//            _Inout_ w_av_frame* p_avframe,
//            _In_ uint32_t p_pixel_format,
//            _In_ uint32_t p_width,
//            _In_ uint32_t p_height,
//            _In_ uint32_t p_alignment,
//            _In_ const uint8_t* p_data,
//            _Inout_ char* p_error);
//
//    /**
//     * initialize the ffmpeg AVFrame
//     * @param p_avframe, the ffmpeg AVFrame
//     * @param p_data, the initial data of ffmpeg AVFrame
//     * @param p_alignment, the alignment
//     * @param p_error, the error buffer
//     * @returns zero on success
//     */
//    W_API
//        int w_av_set_data(
//            _In_ w_av_frame p_avframe,
//            _In_ const uint8_t* p_data,
//            _In_ uint32_t p_alignment,
//            _Inout_ char* p_error);
//
//    /**
//     * get data of ffmpeg AVFrame
//     * @param p_avframe, the avframe
//     * @param p_index, the index of data pointer (0 - 7)
//     * @param p_data, the pointer to the data
//     * @param p_error, the error buffer
//     * @returns zero on success
//    */
//    W_API
//        int w_av_get_data(
//            _In_ w_av_frame p_avframe,
//            _In_ size_t p_index,
//            _Out_ uint8_t* p_data,
//            _Inout_ char* p_error);
//
//
//    /**
//     * get data of ffmpeg AVFrame
//     * @param p_avframe, the avframe
//     * @param p_index, the index of data pointer (0 - 7)
//     * @returns the required linesize
//    */
//    W_API
//        int w_av_get_data_linesize(
//            _In_ w_av_frame p_avframe,
//            _In_ size_t p_index,
//            _Inout_ char* p_error);
//
//    /**
//     * initialize the ffmpeg AVFrame
//     * @param p_pixel_format, the pixel format of ffmpeg AVFrame
//     * @param p_width, the width of ffmpeg AVFrame
//     * @param p_height, the height of ffmpeg AVFrame
//     * @param p_alignment, the aligmnet which is usually 1
//     * @returns the size of buffer
//     */
//    W_API
//        size_t w_av_get_required_buffer_size(
//            _In_ uint32_t p_pixel_format,
//            _In_ uint32_t p_width,
//            _In_ uint32_t p_height,
//            _In_ uint32_t p_alignment);
//
//    /**
//      * convert the ffmpeg AVFrame
//      * @param p_src_avframe, the source AVFrame
//      * @param p_dst_avframe, the destination AVFrame
//      * @param p_error, the error buffer
//      * @returns zero on success
//      */
//    W_API
//        int w_av_frame_convert(
//            _In_ w_av_frame p_src_avframe,
//            _Inout_ w_av_frame* p_dst_avframe,
//            _Inout_ char* p_error);
//
//    /**
//     * release resources of avframe
//     * @param p_av_frame, the ffmpeg AVFrame
//     */
//    W_API
//        void w_av_frame_fini(_Inout_ w_av_frame* p_avframe);
//
//#ifdef __cplusplus
//}

#endif // WOLF_MEDIA_FFMPEG
