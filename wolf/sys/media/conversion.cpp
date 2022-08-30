//#include "conversion.h"
//#include <wolf.hpp>
//
//#include <DISABLE_ANALYSIS_BEGIN>
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavformat/avio.h>
//#include <libavutil/imgutils.h>
//#include <libavutil/opt.h>
//#include <libavutil/time.h>
//#include <libswscale/swscale.h>
//
//#ifdef __cplusplus
//}
//#endif
//
//#include <DISABLE_ANALYSIS_END>
//
//int yuv2rgb(
//    _In_ AVFrame* p_frame_in,
//    _In_ int64_t p_width,
//    _In_ int64_t p_height,
//    _Inout_ AVFrame* p_frame_out,
//    _Inout_ char* p_error)
//{
//
//    constexpr auto TRACE = "ffmpeg::yuv2rgb";
//
//    int ret = 0;
//
//    if (p_width == 0 || p_height == 0 ||
//        p_error == nullptr || p_frame_in == nullptr || p_frame_out == nullptr) 
//    {
//        return -1;
//    }
//
//    SwsContext* context = nullptr;
//
//    // finally will be rised at the end
//    defer _(nullptr, [&](...)
//        {
//            if (context != nullptr) {
//                sws_freeContext(context);
//            }
//        });
//
//    context = sws_getContext(gsl::narrow_cast<int>(p_width),
//        gsl::narrow_cast<int>(p_height), AV_PIX_FMT_YUV420P,
//        gsl::narrow_cast<int>(p_width), gsl::narrow_cast<int>(p_height),
//        AV_PIX_FMT_RGB24, SWS_FLAGS, nullptr, nullptr, nullptr);
//    if (context == nullptr) 
//    {
//        std::printf(
//            p_error,
//            "failed to allocate I420 to RGB conversion context. trace info: %s",
//            TRACE);
//        return -1;
//    }
//
//    ret = sws_scale(context, static_cast<const uint8_t* const*>(p_frame_in->data),
//        static_cast<int*>(p_frame_in->linesize), 0, gsl::narrow_cast<int>(p_height),
//        static_cast<uint8_t* const*>(p_frame_out->data), static_cast<const int*>(p_frame_out->linesize));
//    if (ret < 0)
//    {
//        std::array<char, _MAX_PATH> error_log = { 0 };
//        av_make_error_string(error_log.data(), _MAX_PATH, ret);
//
//        std::printf(
//            p_error,
//            "failed convert from RGB to I420, because %s. trace info: %s",
//            error_log,
//            TRACE);
//        return -1;
//    }
//
//    p_frame_out->width = gsl::narrow_cast<int>(p_width);
//    p_frame_out->height = gsl::narrow_cast<int>(p_height);
//    p_frame_out->format = AV_PIX_FMT_RGB24;
//
//    return ret;
//}