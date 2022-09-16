#include "av_frame.h"
#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}

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
    const char* TRACE = "ffmpeg::w_av_frame_init";
    const auto _error_nn = gsl::not_null<char*>(p_error);
    
    // check width and height
    const int _width = gsl::narrow_cast<int>(p_width);
    const int _height = gsl::narrow_cast<int>(p_height);
    const auto _pixel_format = gsl::narrow_cast<AVPixelFormat>(p_pixel_format);

    if (_width == 0 || _height == 0)
    {
        (void)snprintf(
            _error_nn,
            W_MAX_PATH,
            "width or height is zero. trace info: %s",
            TRACE);
        return -1;
    }

    auto* _av_frame = av_frame_alloc();
    const auto _av_frame_nn = gsl::not_null<AVFrame*>(_av_frame);

    _av_frame_nn->width = _width;
    _av_frame_nn->height = _height;
    _av_frame_nn->format = _pixel_format;

    auto* _ptr = _av_frame_nn.get();
    if (w_av_set_data(_ptr, p_data, p_alignment, _error_nn.get()) != 0)
    {
        w_av_frame_fini(&_ptr);
    }

    *p_frame = _av_frame;
    return 0;
}

int w_av_set_data(
    _In_ w_av_frame p_frame,
    _In_ const uint8_t* p_data,
    _In_ uint32_t p_alignment,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_av_set_data";
    
    const auto _error = gsl::not_null<char*>(p_error);
    const auto _av_frame = gsl::not_null<w_av_frame>(p_frame);

    const auto _size = av_image_fill_arrays(
        static_cast<uint8_t**>(_av_frame->data),
        static_cast<int*>(_av_frame->linesize),
        p_data,
        gsl::narrow_cast<AVPixelFormat>(_av_frame->format),
        _av_frame->width,
        _av_frame->height,
        gsl::narrow_cast<int>(p_alignment));

    if (_size < 0)
    {
        (void)snprintf(
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
    _Out_ uint8_t* p_frame_data,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_av_get_data";

    const auto _error = gsl::not_null<char*>(p_error);
    const auto _av_frame = gsl::not_null<w_av_frame>(p_frame);

    if (_av_frame->linesize[0] <= 0)
    {
        (void)snprintf(
            _error,
            W_MAX_PATH,
            "bad argumans. trace info: %s",
            TRACE);
        return -1;
    }
    p_frame_data = _av_frame->data[0];

    return 0;
}

size_t w_av_get_required_buffer_size(
    _In_ uint32_t p_pixel_format,
    _In_ uint32_t p_width,
    _In_ uint32_t p_height,
    _In_ uint32_t p_alignment)
{
    const auto _pixel_format = gsl::narrow_cast<AVPixelFormat>(p_pixel_format);
    const auto _width = gsl::narrow_cast<int>(p_width);
    const auto _height = gsl::narrow_cast<int>(p_height);
    const auto _align = gsl::narrow_cast<int>(p_alignment);

    auto _size = av_image_get_buffer_size(_pixel_format, _width, _height, _align);
    return _size > 0 ? gsl::narrow_cast<size_t>(_size) : 0;
}

int w_av_frame_convert(
    _In_ w_av_frame p_src_frame,
    _Inout_ w_av_frame* p_dst_frame,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_av_frame_convert";

    const auto _error_nn = gsl::not_null<char*>(p_error);
    const auto _src_frame_nn = gsl::not_null<w_av_frame>(p_src_frame);
    const auto _dst_frame_ptr_nn = gsl::not_null<w_av_frame*>(p_dst_frame);
    const auto _dst_frame_nn = gsl::not_null<w_av_frame>(*_dst_frame_ptr_nn);

    const auto _src_w = gsl::narrow_cast<int>(_src_frame_nn->width);
    const auto _src_h = gsl::narrow_cast<int>(_src_frame_nn->height);
    const auto _src_pixel_fmt = gsl::narrow_cast<AVPixelFormat>(_src_frame_nn->format);

    const auto _dst_w = gsl::narrow_cast<int>(_dst_frame_nn->width);
    const auto _dst_h = gsl::narrow_cast<int>(_dst_frame_nn->height);
    const auto _dst_pixel_fmt = gsl::narrow_cast<AVPixelFormat>(_dst_frame_nn->format);

    auto* _context = sws_getContext(
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
    const auto _context_nn = gsl::not_null<SwsContext*>(_context);

    const auto _height = sws_scale(
        _context_nn,
        static_cast<const uint8_t* const*>(_src_frame_nn->data),
        gsl::narrow_cast<const int*>(_src_frame_nn->linesize),
        0,
        _src_h,
        gsl::narrow_cast<uint8_t* const*>(_dst_frame_nn->data),
        gsl::narrow_cast<const int*>(_dst_frame_nn->linesize));

    // free context
    sws_freeContext(_context_nn);

    if (_height < 0)
    {
        (void)snprintf(
            _error_nn,
            W_MAX_PATH,
            "sws_scale failed. trace info: %s",
            TRACE);
        return -1;
    }

    return 0;
}

void w_av_frame_fini(_Inout_ w_av_frame* p_ffmpeg)
{
    const auto _ptr = gsl::not_null<w_av_frame*>(p_ffmpeg);
    av_frame_free(_ptr);
    *p_ffmpeg = nullptr;
}