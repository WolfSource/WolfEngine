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
    _In_ uint32_t p_pixel_format,
    _In_ uint32_t p_width,
    _In_ uint32_t p_height,
    _In_ uint32_t p_alignment,
    _In_ const uint8_t* p_data,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_av_frame_init";
    int _ret = 0;

    if (p_width == 0 || p_height == 0)
    {
        return -1;
    }

    const auto _error = gsl::not_null<char*>(p_error);
    const auto _w = gsl::narrow_cast<int>(p_width);
    const auto _h = gsl::narrow_cast<int>(p_height);
    const auto _pixel_format = gsl::narrow_cast<AVPixelFormat>(p_pixel_format);

    defer _(nullptr, [&](...)
        {
            if (_ret == -1)
            {
                w_av_frame_fini(p_frame);
            }
        });

    auto _av_frame = av_frame_alloc();
    if (_av_frame == nullptr)
    {
        snprintf(
            _error,
            W_MAX_PATH,
            "could not allocate av frame. trace info: %s",
            TRACE);
    }
    _av_frame->width = _w;
    _av_frame->height = _h;
    _av_frame->format = _pixel_format;

    _ret = w_av_set_data(_av_frame, p_data, p_alignment, _error);
    if (_ret == 0)
    {
        *p_frame = _av_frame;
    }

    return _ret;
}

int w_av_set_data(
    _In_ w_av_frame p_frame,
    _In_ const uint8_t* p_data,
    _In_ uint32_t p_alignment,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_av_set_data";
    const auto _error = gsl::not_null<char*>(p_error);

    const auto _size = av_image_fill_arrays(
        static_cast<uint8_t**>(p_frame->data),
        static_cast<int*>(p_frame->linesize),
        p_data,
        gsl::narrow_cast<AVPixelFormat>(p_frame->format),
        p_frame->width,
        p_frame->height,
        p_alignment);

    if (_size < 0)
    {
        snprintf(
            _error,
            W_MAX_PATH,
            "failed to fill image buffer. trace info: %s",
            TRACE);

        return -1;
    }

    return 0;
}

int w_av_get_data(
    _In_ w_av_frame p_frame,
    _Inout_ uint8_t* p_frame_data,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_av_get_data";
    const auto _error = gsl::not_null<char*>(p_error);
    
    if (p_frame == nullptr ||
        p_frame->linesize == nullptr ||
        p_frame->linesize[0] <= 0)
    {
        snprintf(
            _error,
            W_MAX_PATH,
            "bad argumans. trace info: %s",
            TRACE);
        return -1;
    }
    p_frame_data = p_frame->data[0];

    return 0;
}

size_t w_av_get_required_buffer_size(
    _In_ uint32_t p_pixel_format,
    _In_ uint32_t p_width,
    _In_ uint32_t p_height,
    _In_ uint32_t p_alignment)
{
    const auto _pixel_format = gsl::narrow_cast<AVPixelFormat>(p_pixel_format);
    const auto _w = gsl::narrow_cast<int>(p_width);
    const auto _h = gsl::narrow_cast<int>(p_height);
    const auto _align = gsl::narrow_cast<int>(p_alignment);

    auto _size = av_image_get_buffer_size(_pixel_format, _w, _h, _align);
    return _size > 0 ? gsl::narrow_cast<size_t>(_size) : 0;
}

int w_av_frame_convert(
    _In_ w_av_frame p_src_frame,
    _Inout_ w_av_frame* p_dst_frame,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_av_frame_convert";

    const auto _dst_frame_ptr = gsl::not_null<w_av_frame*>(p_dst_frame);
    const auto _dst_frame = gsl::not_null<w_av_frame>(*_dst_frame_ptr);
    const auto _src_frame = gsl::not_null<w_av_frame>(p_src_frame);
    const auto _error = gsl::not_null<char*>(p_error);

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

//#ifdef DEBUG
//    av_log_set_level(AV_LOG_DEBUG);
//#endif

    const auto _src_w = gsl::narrow_cast<int>(_src_frame->width);
    const auto _src_h = gsl::narrow_cast<int>(_src_frame->height);
    const auto _src_pixel_fmt = gsl::narrow_cast<AVPixelFormat>(_src_frame->format);

    const auto _dst_w = gsl::narrow_cast<int>(_dst_frame->width);
    const auto _dst_h = gsl::narrow_cast<int>(_dst_frame->height);
    const auto _dst_pixel_fmt = gsl::narrow_cast<AVPixelFormat>(_dst_frame->format);

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
        static_cast<const uint8_t* const*>(_src_frame->data),
        gsl::narrow_cast<const int*>(_src_frame->linesize),
        0,
        _src_h,
        gsl::narrow_cast<uint8_t* const*>(_dst_frame->data),
        gsl::narrow_cast<const int*>(_dst_frame->linesize));

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
    if (p_ffmpeg != nullptr)
    {
        av_frame_free(p_ffmpeg);
        *p_ffmpeg = nullptr;
    }
}