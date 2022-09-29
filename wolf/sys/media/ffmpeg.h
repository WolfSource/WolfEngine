/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "av_packet.h"
#include "av_frame.h"

    typedef struct AVCodeOptions
    {
        int gop;
        int level;
        int max_b_frames;
        int refs;
        int thread_count;
        uint32_t fps;
        uint64_t bitrate;
    } AVCodeOptions;

    typedef enum AVSetOptionValueType
    {
        STRING = 0,
        INTEGER,
        DOUBLE
    } AVSetOptionValueType;

    typedef struct AVSetOption
    {
        // name of option 
        const char* name;
        // value type
        AVSetOptionValueType value_type;
        // string value of option
        const char* value_str;
        // int value of option
        int value_int;
        // double value of option
        double value_double;
    } AVSetOption;

    struct w_ffmpeg_t;
    typedef struct w_ffmpeg_t* w_ffmpeg;

    /**
     * initialize the ffmpeg context
     * @param p_ffmpeg, ffmpeg object
     * @param p_frame, the av frame
     * @param p_mode, zero for encoder and non-zero for decoder
     * @param p_avcodec_id, av codec id
     * @param p_codec_opt, the av codec options
     * @param p_av_opt_sets, the av set options
     * @param p_av_opt_sets_size, the av set options size
     * @param p_error, the error buffer
     * @return int the result of encoding the frame
     */
    W_API
        int w_ffmpeg_init(
            _Inout_ w_ffmpeg* p_ffmpeg,
            _In_ w_av_frame p_frame,
            _In_ uint32_t p_mode,
            _In_ const char* p_avcodec_id,
            _In_ AVCodeOptions* p_codec_opt,
            _In_ const AVSetOption* p_av_opt_sets,
            _In_ uint32_t p_av_opt_sets_size,
            _Inout_z_ char* p_error);

    /**
     * encode the ffmpeg avframe
     * @param p_ffmpeg, ffmpeg object
     * @param p_av_frame, the av frame
     * @param p_packet, the output packet
     * @param p_error, the error buffer
     * @returns an int which is the result of encoding the frame
     */
    W_API
        int w_ffmpeg_encode(
            _In_ w_ffmpeg p_ffmpeg,
            _In_ const w_av_frame p_av_frame,
            _Inout_ w_av_packet* p_packet,
            _Inout_ char* p_error);

    /**
     * decode the ffmpeg avframe
     * @param p_ffmpeg, the ffmpeg object
     * @param p_packet, the av packet
     * @param p_av_frame, the avframe
     * @param p_error, the error buffer
     * @returns an int which is the result of encoding the frame
     */
    W_API
        int w_ffmpeg_decode(
            _In_ w_ffmpeg p_ffmpeg,
            _In_ const w_av_packet p_packet,
            _Inout_ w_av_frame* p_av_frame,
            _Inout_ char* p_error);

    /**
     * @param p_ffmpeg, ffmpeg object
     * release all ffmpeg resources
     */
    W_API
        void w_ffmpeg_fini(_Inout_ w_ffmpeg* p_ffmpeg);

#ifdef __cplusplus
}
#endif