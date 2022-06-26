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
#include <libavutil/opt.h>
#include <libavutil/time.h> //NO-LINT
#include <libswscale/swscale.h> //NO-LINT

#ifdef __cplusplus
}
#endif

#include <gsl/gsl> //NO-LINT

constexpr int SWS_FLAGS = SWS_BICUBIC;

auto rgb2yuv(uint8_t *p_rgb, int64_t p_width, int64_t p_height, AVFrame **p_frame,
           char *p_error) -> int {

   constexpr auto _trace = "ffmpeg::rgb2yuv";

   int ret = 0;

   if (p_rgb == nullptr || p_width == 0 || p_height == 0 ||
       p_error == nullptr || p_frame == nullptr || *p_frame == nullptr) {
           return -1;
   }

   // declare local variables here
   std::array<uint8_t *, 3> rgb = {nullptr};
   rgb[0] = p_rgb;
   rgb[1] = nullptr;
   rgb[2] = nullptr;

   std::array<int, 3> rgb_stride = {0};
   rgb_stride[0] = gsl::narrow_cast<int>(p_width) * 3;
   rgb_stride[1] = 0;
   rgb_stride[2] = 0;

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
       sws_getContext(gsl::narrow_cast<int>(p_width), 
           gsl::narrow_cast<int>(p_height), AV_PIX_FMT_RGB24, 
           gsl::narrow_cast<int>(p_width), gsl::narrow_cast<int>(p_height),
           AV_PIX_FMT_YUV420P, SWS_FLAGS, nullptr, nullptr, nullptr);

   if (context == nullptr) {
       std::printf(
           p_error,
           "failed to allocate RGB to I420 conversion context. trace info: %s",
           _trace);
       return ret = -1;
   }

   frame_buffer_in = gsl::narrow_cast<uint8_t *>(av_malloc(
       av_image_get_buffer_size(AV_PIX_FMT_YUV420P, gsl::narrow_cast<int>(p_width), gsl::narrow_cast<int>(p_height), 1)));

   if (frame_buffer_in == nullptr) {
       std::printf(
           p_error, 
           "failed to allocate av buffer. trace info: %s",
           _trace);
       return ret = -1;
   }

   auto size = av_image_fill_arrays(static_cast<uint8_t **>((*p_frame)->data), static_cast<int *>((*p_frame)->linesize), frame_buffer_in,
                       AV_PIX_FMT_YUV420P, gsl::narrow_cast<int>(p_width), gsl::narrow_cast<int>(p_height), 1);

   if (size < 0) 
   {
       std::array<char, _MAX_PATH> error_log = {0};
       av_make_error_string(error_log.data(), _MAX_PATH, ret);

       std::printf(
           p_error,
           "failed fill image array, because %s. trace info: %s",
           error_log, _trace);
       return ret = -1;
   }

   auto height = sws_scale(context, static_cast<const uint8_t *const *>(rgb.data()), rgb_stride.data(), 0,
       gsl::narrow_cast<int>(p_height), static_cast<uint8_t *const*>((*p_frame)->data), static_cast<const int*>((*p_frame)->linesize));

   if (height < 0) {
       std::array<char, _MAX_PATH> error_log = {0};
       av_make_error_string(error_log.data(), _MAX_PATH, ret);

       std::printf(
           p_error,
           "failed convert from RGB to I420, because %s. trace info: %s",
           error_log, _trace);
       return ret = -1;
   }

   (*p_frame)->width = gsl::narrow_cast<int>(p_width);
   (*p_frame)->height = gsl::narrow_cast<int>(p_height);
   (*p_frame)->format = AV_PIX_FMT_YUV420P;

   return ret;
}

auto yuv2rgb(AVFrame* p_frame_in, int64_t p_width, int64_t p_height,
           AVFrame* p_frame_out, char *p_error) -> int {
	
   constexpr auto _trace = "ffmpeg::yuv2rgb";

   int ret = 0;

   if (p_width == 0 || p_height == 0 ||
       p_error == nullptr || p_frame_in == nullptr || p_frame_out == nullptr) {
       return -1;
   }

	SwsContext* context = nullptr;

   // finally will be rised at the end
   auto _ = gsl::finally([&] {
       if (context != nullptr) {
           sws_freeContext(context);
       }
   });

	context = sws_getContext(gsl::narrow_cast<int>(p_width), 
        gsl::narrow_cast<int>(p_height), AV_PIX_FMT_YUV420P, 
        gsl::narrow_cast<int>(p_width), gsl::narrow_cast<int>(p_height), 
        AV_PIX_FMT_RGB24, SWS_FLAGS, nullptr, nullptr, nullptr);
	if (context == nullptr) {
       std::printf(
           p_error,
           "failed to allocate I420 to RGB conversion context. trace info: %s",
           _trace);
       return -1;
	}

	ret = sws_scale(context, static_cast<const uint8_t *const *>(p_frame_in->data), 
        static_cast<int *>(p_frame_in->linesize), 0, gsl::narrow_cast<int>(p_height), 
        static_cast<uint8_t *const *>(p_frame_out->data), static_cast<const int *>(p_frame_out->linesize));
	if (ret < 0) 
    {
        std::array<char, _MAX_PATH> error_log = {0};
        av_make_error_string(error_log.data(), _MAX_PATH, ret);

       std::printf(
           p_error,
           "failed convert from RGB to I420, because %s. trace info: %s",
           error_log,
           _trace);
       return -1;
	}

   p_frame_out->width = gsl::narrow_cast<int>(p_width);
   p_frame_out->height = gsl::narrow_cast<int>(p_height);
   p_frame_out->format = AV_PIX_FMT_RGB24;

	return ret;
}