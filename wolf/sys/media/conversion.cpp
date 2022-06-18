/*
       Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
       https://github.com/WolfEngine/WolfEngine
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h> //NO-LINT
#include <libavformat/avformat.h> //NO-LINT
#include <libavformat/avio.h> //NO-LINT
#include <libavutil/imgutils.h> //NO-LINT
#include <libavutil/time.h> //NO-LINT
#include <libavutil/opt.h>
#include <libswscale/swscale.h> //NO-LINT

#ifdef __cplusplus
}
#endif

#include <gsl/gsl> //NO-LINT

#define SWS_FLAGS SWS_BICUBIC

auto rgb2yuv(uint8_t *p_rgb, int p_width, int p_height, AVFrame **p_frame,
            char *p_error) -> int {

    constexpr auto _trace = "ffmpeg::rgb2yuv";

    int ret = 0;

    if (p_rgb == nullptr || p_width == 0 || p_height == 0 ||
        p_error == nullptr || p_frame == nullptr || *p_frame == nullptr) {
        std::printf(
            p_error,
            "bad input parameters. trace info: %s",
            _trace);
            return ret = -1;
    }

    // declare local variables here
    uint8_t *rgb[3] = {p_rgb, nullptr, nullptr};
    int rgb_stride[3] = {p_width * 3, 0, 0};
    SwsContext *context = nullptr;
    uint8_t *frame_buffer_in = nullptr;

    // finally will be rised at the end
    auto _ = gsl::finally([&] {
        if (context != nullptr) {
            sws_freeContext(context);
        }
        // the following vars should be released on failure
        if (ret != 0) {
            if (frame_buffer_in != nullptr) {
                av_free(frame_buffer_in);
            }
        }
    });

    av_log_set_level(AV_LOG_DEBUG);

    context =
        sws_getContext(p_width, p_height, AV_PIX_FMT_RGB24, p_width, p_height,
                        AV_PIX_FMT_YUV420P, SWS_FLAGS, nullptr, nullptr, nullptr);

    if (context == nullptr) {
        std::printf(
            p_error,
            "failed to allocate RGB to I420 conversion context. trace info: %s",
            _trace);
        return ret = -1;
    }

    frame_buffer_in = gsl::narrow_cast<uint8_t *>(av_malloc(
        av_image_get_buffer_size(AV_PIX_FMT_YUV420P, p_width, p_height, 1)));

    if (frame_buffer_in == nullptr) {
        std::printf(
            p_error, 
            "failed to allocate av buffer. trace info: %s",
            _trace);
        return ret = -1;
    }

    auto size = av_image_fill_arrays((*p_frame)->data, (*p_frame)->linesize, frame_buffer_in,
                        AV_PIX_FMT_YUV420P, p_width, p_height, 1);

    if (size < 0) {
        char error_log[_MAX_PATH] = {0};
        av_make_error_string(error_log, _MAX_PATH, ret);

        std::printf(
            p_error,
            "failed fill image array, because %s. trace info: %s",
            error_log, _trace);
        return ret = -1;
    }

    auto height = sws_scale(context, (const uint8_t *const *)&rgb, rgb_stride, 0,
                p_height, (*p_frame)->data, (*p_frame)->linesize);

    if (height < 0) {
        char error_log[_MAX_PATH] = {0};
        av_make_error_string(error_log, _MAX_PATH, ret);

        std::printf(
            p_error,
            "failed convert from RGB to I420, because %s. trace info: %s",
            error_log, _trace);
        return ret = -1;
    }

    (*p_frame)->width = p_width;
    (*p_frame)->height = p_height;
    (*p_frame)->format = AV_PIX_FMT_YUV420P;

    return ret;
}

auto yuv2rgb(AVFrame* p_frame_in, int p_width, int p_height,
            AVFrame* p_frame_out, char *p_error) -> int {
	
    constexpr auto _trace = "ffmpeg::yuv2rgb";

    int ret = 0;

    if (p_width == 0 || p_height == 0 ||
        p_error == nullptr || p_frame_in == nullptr || p_frame_out == nullptr) {
        std::printf(
            p_error,
            "bad input parameters. trace info: %s",
            _trace);
        return ret = -1;
    }

	SwsContext* context = nullptr;

    // finally will be rised at the end
    auto _ = gsl::finally([&] {
        if (context != nullptr) {
            sws_freeContext(context);
        }
    });

	context = sws_getContext(p_width, p_height, AV_PIX_FMT_YUV420P, p_width, p_height, AV_PIX_FMT_RGB24, SWS_FLAGS, NULL, NULL, NULL);
	if (context == nullptr) {
        std::printf(
            p_error,
            "failed to allocate I420 to RGB conversion context. trace info: %s",
            _trace);
        return ret = -1;
	}

	// AVFrame* _frame_rgb = av_frame_alloc();
	// auto frame_buffer_out = (unsigned char*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24, p_width, p_height, 1));
	// av_image_fill_arrays(_frame_rgb->data, _frame_rgb->linesize, frame_buffer_out, AV_PIX_FMT_RGB24, p_width, p_height, 1);

	ret = sws_scale(context, (const uint8_t * const*)& p_frame_in->data, p_frame_in->linesize, 0, p_height, p_frame_out->data, p_frame_out->linesize);
	if (ret < 0) {
        char error_log[_MAX_PATH] = {0};
        av_make_error_string(error_log, _MAX_PATH, ret);

        std::printf(
            p_error,
            "failed convert from RGB to I420, because %s. trace info: %s",
            error_log,
            _trace);
        return ret = -1;
	}

    p_frame_out->width = p_width;
    p_frame_out->height = p_height;
    p_frame_out->format = AV_PIX_FMT_YUV420P;

	return ret;
}