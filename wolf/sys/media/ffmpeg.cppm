/*
        Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
module;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}

#include <gsl/gsl> //NO-LINT
#include <wolf.hpp>

#define SWS_FLAGS SWS_BICUBIC

export module wolf.media.ffmpeg;

namespace wolf::media::ffmpeg {

#pragma region types

C_EXPORT typedef AVFrame *w_av_frame;

#pragma endregion

#pragma region functions

C_EXPORT
auto rgb2yuv(uint8_t *p_rgb_raw, int p_width, int p_height) -> w_av_frame {
  av_log_set_level(AV_LOG_DEBUG);

  int w = p_width;
  int h = p_height;

  SwsContext *rgb_to_I420_ctx = nullptr;

  rgb_to_I420_ctx =
      sws_getContext(w, h, AV_PIX_FMT_RGB24, w, h, AV_PIX_FMT_YUV420P,
                     SWS_FLAGS, nullptr, nullptr, nullptr);

  if (rgb_to_I420_ctx == nullptr) {
    std::fprintf(stderr,
                 "Failed to allocate RGB to I420 conversion context.\n");
  }

  uint8_t *rgb[3] = {0};
  uint8_t *i420[3] = {0};
  int rgbStride[3] = {0};

  rgbStride[0] = w * 3;

  rgb[0] = p_rgb_raw;

  AVFrame *_frame_yuv;

  _frame_yuv = av_frame_alloc();
  unsigned char *frame_buffer_in = (unsigned char *)av_malloc(
      av_image_get_buffer_size(AV_PIX_FMT_YUV420P, w, h, 1));

  av_image_fill_arrays(_frame_yuv->data, _frame_yuv->linesize, frame_buffer_in,
                       AV_PIX_FMT_YUV420P, w, h, 1);

  int toI420Res =
      sws_scale(rgb_to_I420_ctx, (const uint8_t *const *)&rgb, rgbStride, 0, h,
                _frame_yuv->data, _frame_yuv->linesize);

  if (toI420Res < 0) {
    // TODO: use callback for error in order to catch them from rust
    char _errorLog[_MAX_PATH] = {0};
    std::fprintf(stderr, "Conversion from RGB to I420 failed, %s.\n",
                 av_make_error_string(_errorLog, _MAX_PATH, toI420Res));
  }

  sws_freeContext(rgb_to_I420_ctx);

  _frame_yuv->width = w;
  _frame_yuv->height = h;
  _frame_yuv->format = AV_PIX_FMT_YUV420P;

  return _frame_yuv;
}

#pragma endregion

} // namespace wolf::media::ffmpeg