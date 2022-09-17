/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "av_frame.h"

    typedef struct AVCodeOptions
    {
        uint32_t fps;
        uint32_t gop;
        uint32_t refs;
        uint32_t max_b_frames;
        uint32_t thread_count;
        uint64_t bitrate;
        uint64_t level;
    } AVCodeOptions;

    typedef struct w_av_opt_set_str
    { 
        // name of option 
        char* name; 
        // value of option
        char* value;
    } w_av_opt_set_str W_ALIGNMENT_16;

    typedef struct w_av_opt_set_int
    {
        // name of option 
        char* name;
        // value of option
        int value;
    } w_av_opt_set_int W_ALIGNMENT_16;

    typedef struct w_av_opt_set_double
    {
        // name of option 
        char* name;
        // value of option
        double value;
    } w_av_opt_set_double W_ALIGNMENT_16;

    struct w_ffmpeg_t;
    typedef struct w_ffmpeg_t* w_ffmpeg;

    /**
     * initialize the ffmpeg context
     * @param w_av_frame p_frame,
     * @param p_avcodec_id, the avcodec id
     * @param p_mode, zero for encoder and non-zero for decoder
     * @param p_avcodec_id, av codec id
     * @param p_codec_opt, the av codec options
     * @param p_preset_strings, the string presets
     * @param p_preset_strings_size, the size of string presets
     * @param p_preset_ints, the integer presets
     * @param p_preset_ints_size, the size of integer presets
     * @param p_preset_doubles, the double presets
     * @param p_preset_doubles_size, the size of double presets
     * @param p_error, the error buffer
     * @return int the result of encoding the frame
     */
    W_API
        int w_ffmpeg_init(
            _Inout_ w_ffmpeg* p_ffmpeg,
            _In_ w_av_frame p_frame,
            _In_ uint32_t p_mode,
            _In_ uint32_t p_avcodec_id,
            _In_ AVCodeOptions* p_codec_opt,
            _In_ w_av_opt_set_str* p_preset_strings,
            _In_ uint32_t p_preset_strings_size,
            _In_ w_av_opt_set_int* p_preset_ints,
            _In_ uint32_t p_preset_ints_size,
            _In_ w_av_opt_set_double* p_preset_doubles,
            _In_ uint32_t p_preset_doubles_size,
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