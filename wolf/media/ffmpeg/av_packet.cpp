//#include "av_packet.h"
//#include <wolf.hpp>
//
//#include <DISABLE_ANALYSIS_BEGIN>
//extern "C"
//{
//#include <libavcodec/avcodec.h>
//}
//#include <DISABLE_ANALYSIS_END>
//
//int w_av_packet_init(
//    _Inout_ w_av_packet* p_av_packets,
//    _Inout_ char* p_error)
//{
//    constexpr auto TRACE = "ffmpeg::w_av_frame_init";
//    const auto _error_nn = gsl::not_null<char*>(p_error);
//
//    auto _av_packet = av_packet_alloc();
//    if (_av_packet == nullptr)
//    {
//        *p_av_packets = nullptr;
//
//        std::ignore = snprintf(
//            _error_nn,
//            W_MAX_PATH,
//            "width or height is zero. trace info: %s",
//            TRACE);
//        return W_FAILURE;
//    }
//
//    *p_av_packets = _av_packet;
//    return W_SUCCESS;
//}
//
//int w_av_packet_get(
//    _In_ w_av_packet p_av_packet,
//    _Inout_ uint8_t* p_data)
//{
//    const auto _packet_nn = gsl::not_null<w_av_packet>(p_av_packet);
//    p_data = _packet_nn->data;
//    return _packet_nn->size;
//}
//
//void w_av_packet_fini(_Inout_ w_av_packet* p_avframe)
//{
//    const auto _ptr = gsl::not_null<w_av_packet*>(p_avframe);
//    av_packet_free(_ptr);
//    *p_avframe = nullptr;
//}