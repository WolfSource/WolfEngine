/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    struct AVPacket;
    typedef struct AVPacket* w_av_packet;

    /**
     * initialize the ffmpeg AVFrame
     * @param p_av_packet, the AVPacket object
     * @returns zero on success
     */
    W_API
        int w_av_packet_init(
            _Inout_ w_av_packet* p_av_packet,
            _Inout_ char* p_error);

    /**
     * get packet data
     * @param p_av_packet, the AVPacket object
     * @param p_data, the data
     * @returns size of packet in bytes
     */
    W_API
        int w_av_packet_get(
            _In_ w_av_packet p_av_packet,
            _Inout_ uint8_t* p_data);

    /**
     * release resources of avpacket
     * @param p_av_frame, the ffmpeg AVFrame
     */
    W_API
        void w_av_packet_fini(_Inout_ w_av_packet* p_av_packet);

#ifdef __cplusplus
}
#endif
