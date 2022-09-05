/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    typedef enum w_ffmpeg_action
    {
        W_ENCODE,
        W_DECODE
    }w_ffmpeg_action;

    typedef enum w_ffmpeg_pixel_format
    {
        W_PIX_FMT_YUV420P = 0, ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
        W_PIX_FMT_RGB24 = 2,   ///< packed RGB 8:8:8, 24bpp, RGBRGB
        W_PIX_FMT_BGR24 = 3,   ///< packed RGB 8:8:8, 24bpp, BGRBGR
        W_PIX_FMT_YUV422P = 4, ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
        W_PIX_FMT_YUV444P = 5, ///< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    }w_ffmpeg_pixel_format;

    struct w_ffmpeg_ctx_t;

    typedef struct w_ffmpeg_ctx_t* w_ffmpeg_ctx;

    //struct AVFrame;
    //typedef struct AVFrame* w_ffmpeg_frame;

    typedef struct w_ffmpeg_t {
        int fps;
        uint32_t width;
        uint32_t height;
        uint32_t bitrate;
        uint32_t codec_id;
        w_ffmpeg_pixel_format pix_fmt;
        w_ffmpeg_action type;
        w_ffmpeg_ctx ctx;
    }w_ffmpeg_t;

    typedef w_ffmpeg_t* w_ffmpeg;

    /**
     * initialize the ffmpeg context
     * @param p_ffmpeg, the ffmpeg object
     * @param p_error, the error buffer
     * @return int the result of encoding the frame
     */
    W_API
        int w_ffmpeg_init(
            _Inout_ w_ffmpeg* p_ffmpeg,
            _Inout_z_ char* p_error);

    /**
    * encode the frame
    * @param p_ffmpeg, the ffmpeg object
    * @param p_src, the input rgb buffer
    * @param p_dst_len, the size of encoded buffer
    * @param p_dst, output encoded buffer
    * @param p_error the error buffer
    * @return int the result of encoding the frame
    */
    //W_API
    //    int w_ffmpeg_encode(
    //        _In_ w_ffmpeg p_ffmpeg,
    //        _In_ uint8_t* p_src,
    //        _In_ int* p_dst_len,
    //        _Inout_ uint8_t** p_dst, 
    //        _Inout_z_ char* p_error);

    ///**
    //* decode the frame
    //* @param w_ffmpeg the ffmpeg object
    //* @param p_src_len, the size of input decode data
    //* @param p_src, the input decode data
    //* @param p_dst_len, the size of decoded buffer
    //* @param p_dst, the output buffer containing the decoded data
    //* @param p_error the error buffer
    //* @return int the result of encoding the frame
    //*/
    //W_API
    //    int w_ffmpeg_decode(
    //        _In_ w_ffmpeg p_ffmpeg_opt,
    //        _In_ int p_src_len,
    //        _In_ uint8_t* p_src,
    //        _Inout_ int* p_dst_len,
    //        _Inout_ uint8_t** p_dst,
    //        _Inout_z_ char* p_error);

   /**
    * release all ffmpeg resources
    */
    W_API
        void w_ffmpeg_fini(_Inout_ w_ffmpeg* p_ffmpeg);

#ifdef __cplusplus
}
#endif