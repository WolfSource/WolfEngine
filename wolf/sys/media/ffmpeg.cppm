/*
        Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
module;

extern "C" {
#include <libavcodec/avcodec.h> //NO-LINT
#include <libavformat/avformat.h> //NO-LINT
#include <libavformat/avio.h> //NO-LINT
#include <libavutil/imgutils.h> //NO-LINT
#include <libavutil/time.h> //NO-LINT
#include <libswscale/swscale.h> //NO-LINT
}

#include <gsl/gsl> //NO-LINT
#include <wolf.hpp>

constexpr auto SWS_FLAGS = SWS_BICUBIC;

export module wolf.media.ffmpeg;

namespace wolf::media::ffmpeg {

#pragma region types

C_EXPORT typedef AVFrame *w_av_frame;

#pragma endregion

#pragma region callback

#pragma region functions

C_EXPORT
auto rgb2yuv(uint8_t *p_rgb_raw, int p_width, int p_height, w_av_frame *p_frame,
             char *p_error) -> int {
  constexpr auto _trace = "ffmpeg::rgb2yuv";

  if (p_rgb_raw == nullptr || p_width == 0 || p_height == 0 ||
      p_error == nullptr) {
    return -1;
  }

  // declare local variables here
  int ret = 0;
  uint8_t *rgb[3] = {p_rgb_raw, nullptr, nullptr};
  int rgb_stride[3] = {p_width * 3, 0, 0};
  SwsContext *rgb_to_I420_ctx = nullptr;
  AVFrame* _frame_yuv = nullptr;
  uint8_t *frame_buffer_in = nullptr;

  auto _ = gsl::finally([&] {
    if (rgb_to_I420_ctx != nullptr) {
      sws_freeContext(rgb_to_I420_ctx);
    }
    // the following vars should be released on failure
    if (ret != 0)
    {
      if (_frame_yuv != nullptr) {
        av_frame_free(&_frame_yuv);
      }
      if (frame_buffer_in != nullptr) {
        av_free(frame_buffer_in);
      }
    }
  });

  av_log_set_level(AV_LOG_DEBUG);

  rgb_to_I420_ctx =
      sws_getContext(p_width, p_height, AV_PIX_FMT_RGB24, p_width, p_height,
                     AV_PIX_FMT_YUV420P, SWS_FLAGS, nullptr, nullptr, nullptr);

  if (rgb_to_I420_ctx == nullptr) {
    std::printf(
        p_error,
        "failed to allocate RGB to I420 conversion context. trace info: %s",
        _trace);
  }

  _frame_yuv = av_frame_alloc();
  if (_frame_yuv == nullptr) {
    std::printf(p_error, "failed to allocate av frame context. trace info: %s",
                _trace);
    return -1;
  }

  frame_buffer_in = gsl::narrow_cast<uint8_t *>(av_malloc(
      av_image_get_buffer_size(AV_PIX_FMT_YUV420P, p_width, p_height, 1)));
  if (frame_buffer_in == nullptr) {
    av_frame_free(&_frame_yuv);
    std::printf(p_error, "failed to allocate av buffer. trace info: %s",
                _trace);
    return -1;
  }

  av_image_fill_arrays(_frame_yuv->data, _frame_yuv->linesize, frame_buffer_in,
                       AV_PIX_FMT_YUV420P, p_width, p_height, 1);

  auto to_I420_res =
      sws_scale(rgb_to_I420_ctx, (const uint8_t *const *)&rgb, rgb_stride, 0,
                p_height, _frame_yuv->data, _frame_yuv->linesize);

  if (to_I420_res < 0) {
    ret = -1;

    char _error_log[_MAX_PATH] = {0};
    av_make_error_string( _error_log, _MAX_PATH, to_I420_res);
    std::printf(
        p_error,
        "conversion from RGB to I420 failed, because %s. trace info: %s",
        _error_log, _trace);
    return ret;
  }

  _frame_yuv->width = p_width;
  _frame_yuv->height = p_height;
  _frame_yuv->format = AV_PIX_FMT_YUV420P;

  *p_frame = _frame_yuv;

  return 0;
}

#pragma endregion

} // namespace wolf::media::ffmpeg