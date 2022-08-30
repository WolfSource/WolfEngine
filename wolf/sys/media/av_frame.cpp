#include "av_frame.h"
#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif
#include <DISABLE_ANALYSIS_END>

int w_av_frame_init(
    _Inout_ w_av_frame* p_frame,
    _In_ uint32_t p_width,
    _In_ uint32_t p_height,
    _In_ uint32_t p_pixel_format,
    _Inout_z_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_av_frame_init";
    int _ret = 0;

    if (p_width == 0 || p_height == 0 || p_pixel_format == 0 || p_error == nullptr)
    {
        return -1;
    }

    defer _(nullptr, [&](...)
        {
            if (_ret == -1)
            {
                w_av_frame_fini(p_frame);
            }
        });

    auto* _av_frame = gsl::narrow_cast<AVFrame*>(av_malloc(sizeof(AVFrame)));
    if (w_is_null(_av_frame))
    {
        snprintf(
            p_error,
            W_MAX_PATH,
            "could not allocate memory for w_av_frame. trace info: %s",
            TRACE);

        return _ret = -1;
    }

    const auto _w = gsl::narrow_cast<int>(p_width);
    const auto _h = gsl::narrow_cast<int>(p_height);
    const auto _pixel_format = gsl::narrow_cast<AVPixelFormat>(p_pixel_format);

    const auto _buffer_size = av_image_get_buffer_size(_pixel_format, p_width, p_height, 1);
    auto* _buffer = gsl::narrow_cast<uint8_t*>(av_malloc(_buffer_size));

    if (w_is_null(_buffer))
    {
        snprintf(
            p_error,
            W_MAX_PATH,
            "could not allocate memory for av image. trace info: %s",
            TRACE);

        return _ret = -1;
    }

    const auto _size = av_image_fill_arrays(
        static_cast<uint8_t**>(_av_frame->data),
        static_cast<int*>(_av_frame->linesize),
        _buffer,
        _pixel_format,
        _w,
        _h,
        1);

    if (_size < 0)
    {
        snprintf(
            p_error,
            W_MAX_PATH,
            "failed to fill image array. trace info: %s",
            TRACE);

        return _ret = -1;
    }

    *p_frame = _av_frame;

    return _ret;
}

int w_av_frame_convert(
    _In_ w_av_frame p_src_frame,
    _Inout_ w_av_frame* p_dst_frame,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_av_frame_convert";

    if (p_src_frame == nullptr ||
        p_error == nullptr ||
        p_dst_frame == nullptr || *p_dst_frame == nullptr)
    {
        return -1;
    }

    int _ret = 0;
    SwsContext* _context = nullptr;

    // defer will be rised at the end
    defer _(nullptr, [&](...)
        {
            if (_context != nullptr)
            {
                sws_freeContext(_context);
            }
        });

#ifdef DEBUG
    av_log_set_level(AV_LOG_DEBUG);
#endif

    const auto _src_w = gsl::narrow_cast<int>(p_src_frame->width);
    const auto _src_h = gsl::narrow_cast<int>(p_src_frame->height);
    const auto _src_pixel_fmt = gsl::narrow_cast<AVPixelFormat>(p_src_frame->format);

    const auto _dst_w = gsl::narrow_cast<int>((*p_dst_frame)->width);
    const auto _dst_h = gsl::narrow_cast<int>((*p_dst_frame)->height);
    const auto _dst_pixel_fmt = gsl::narrow_cast<AVPixelFormat>((*p_dst_frame)->format);

    _context = sws_getContext(
        _src_w,
        _src_h,
        _src_pixel_fmt,
        _dst_w,
        _dst_h,
        _dst_pixel_fmt,
        SWS_BICUBIC,
        nullptr,
        nullptr,
        nullptr);

    if (w_is_null(_context))
    {
        snprintf(
            p_error,
            W_MAX_PATH,
            "failed to create a context for converting frames. trace info: %s",
            TRACE);

        return _ret = -1;
    }

    const auto _height = sws_scale(
        _context,
        static_cast<const uint8_t* const*>(p_src_frame->data),
        gsl::narrow_cast<const int*>(p_src_frame->linesize),
        0,
        _src_h,
        gsl::narrow_cast<uint8_t* const*>((*p_dst_frame)->data),
        gsl::narrow_cast<const int*>((*p_dst_frame)->linesize));

    if (_height < 0)
    {
        snprintf(
            p_error,
            W_MAX_PATH,
            "sws_scale failed. trace info: %s",
            TRACE);
        _ret = -1;
    }

    return _ret;
}

void w_av_frame_fini(_Inout_ w_av_frame* p_ffmpeg)
{
    if (p_ffmpeg)
    {
        av_frame_free(p_ffmpeg);
        *p_ffmpeg = nullptr;
    }
}