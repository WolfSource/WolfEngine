/*
       Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
       https://github.com/WolfEngine/WolfEngine
*/

#include <gsl/gsl> //NO-LINT
#include <wolf.h>
#include "ffmpeg.h"
#include "conversion.h"
//#include "utility.h"

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

#pragma region types

typedef AVFrame *w_av_frame;

#pragma endregion

#pragma region structs

typedef struct w_ffmpeg_ctx_t
{
    AVCodecID codec_id;
    AVPixelFormat format;
    AVCodecContext *context = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *rgb_frame = nullptr;
    AVFrame *yuv_frame = nullptr;
    const AVCodec *codec = nullptr;
    AVCodecParserContext *parser = nullptr;
}w_ffmpeg_ctx_t;

typedef w_ffmpeg_ctx_t *w_ffmpeg_ctx;

#pragma endregion

#pragma region callback

#pragma region functions

auto w_ffmpeg_init(w_ffmpeg_ctx p_ffmpeg_ctx, char *p_error) -> int {

  constexpr auto _trace = "ffmpeg::w_ffmpeg_init";

  int ret = 0;

  if(p_ffmpeg_ctx == nullptr || p_error == nullptr) {
     std::printf(
     p_error,
     "bad input parameters. trace info: %s",
     _trace);
     return ret = -1;
  }

  // finally will be raised at the end
  auto _ = gsl::finally([&] {
    if (ret != 0) {
      if (p_ffmpeg_ctx->packet != nullptr) {
          av_packet_free(&p_ffmpeg_ctx->packet);
      }

      if (p_ffmpeg_ctx->context != nullptr) {
          avcodec_close(p_ffmpeg_ctx->context);
      }
    }
 });

  p_ffmpeg_ctx->packet = av_packet_alloc();
	if (p_ffmpeg_ctx->packet == nullptr) {
       std::printf(
       p_error,
       "failed to allocate the packet. trace info: %s",
       _trace);
     return ret = -1;
	}

  p_ffmpeg_ctx->yuv_frame = av_frame_alloc();
  if (p_ffmpeg_ctx->yuv_frame == nullptr) {
      std::printf(
        p_error, 
        "failed to allocate the frame. trace info: %s",
        _trace);
     return ret = -1;
  }

   return ret;
}

auto w_ffmpeg_init_encoder(w_ffmpeg_opt p_ffmpeg_opt, char *p_error) -> int {

  constexpr auto _trace = "ffmpeg::w_ffmpeg_init_encoder";
  int ret = 0;

    p_ffmpeg_opt->ctx->codec = avcodec_find_encoder((AVCodecID)p_ffmpeg_opt->codec_id);
    if (p_ffmpeg_opt->ctx->codec == nullptr)
    {
        std::printf(
            p_error,
            "failed to find the specified coded. trace info: %s",
            _trace);
        return ret = -1;
    }

    (p_ffmpeg_opt->ctx->context) = avcodec_alloc_context3(p_ffmpeg_opt->ctx->codec);
    if ((p_ffmpeg_opt->ctx->context) == nullptr)
    {
        std::printf(
            p_error,
            "failed to allocate video codec context. trace info: %s",
            _trace);
        return ret = -1;
    }

    (p_ffmpeg_opt->ctx->context)->bit_rate = p_ffmpeg_opt->bitrate;
    (p_ffmpeg_opt->ctx->context)->width = p_ffmpeg_opt->width;
    (p_ffmpeg_opt->ctx->context)->height = p_ffmpeg_opt->height;
    (p_ffmpeg_opt->ctx->context)->time_base.num = 1;
    (p_ffmpeg_opt->ctx->context)->time_base.den = p_ffmpeg_opt->fps;
    (p_ffmpeg_opt->ctx->context)->pix_fmt = p_ffmpeg_opt->ctx->format;

	if (p_ffmpeg_opt->codec_id == AV_CODEC_ID_H264)
		av_opt_set(p_ffmpeg_opt->ctx->context->priv_data, "preset", "slow", 0);

	if (avcodec_open2(p_ffmpeg_opt->ctx->context, p_ffmpeg_opt->ctx->codec, nullptr) < 0) {
       std::printf(
       p_error,
       "failed to open the codec. trace info: %s",
       _trace);

     return ret = -1;
	}

  return ret;
}

auto w_ffmpeg_init_decoder(w_ffmpeg_opt p_ffmpeg_opt, char *p_error) -> int {

  constexpr auto _trace = "ffmpeg::w_ffmpeg_init_decoder";
  int ret = 0;

  if(p_ffmpeg_opt == nullptr || p_error == nullptr) {
     std::printf(
     p_error,
     "bad input parameters. trace info: %s",
     _trace);
     return ret = -1;
  }

  p_ffmpeg_opt->ctx->codec = avcodec_find_decoder((AVCodecID)p_ffmpeg_opt->codec_id);
  if (p_ffmpeg_opt->ctx->codec == nullptr)
  {
      std::printf(
          p_error,
          "failed to find the specified coded. trace info: %s",
          _trace);
      return ret = -1;
  }

  (p_ffmpeg_opt->ctx->context) = avcodec_alloc_context3(p_ffmpeg_opt->ctx->codec);
  if ((p_ffmpeg_opt->ctx->context) == nullptr)
  {
      std::printf(
          p_error,
          "failed to allocate video codec context. trace info: %s",
          _trace);
      return ret = -1;
  }

  p_ffmpeg_opt->ctx->rgb_frame = av_frame_alloc();
  if (p_ffmpeg_opt->ctx->rgb_frame == nullptr) {
      std::printf(
        p_error, 
        "failed to allocate the frame. trace info: %s",
        _trace);
     return ret = -1;
  }

  auto frame_buffer_out = (const uint8_t*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24, p_ffmpeg_opt->width, p_ffmpeg_opt->height, 1));
	av_image_fill_arrays(p_ffmpeg_opt->ctx->rgb_frame->data, p_ffmpeg_opt->ctx->rgb_frame->linesize, frame_buffer_out, AV_PIX_FMT_RGB24, p_ffmpeg_opt->width, p_ffmpeg_opt->height, 1);

    p_ffmpeg_opt->ctx->parser = av_parser_init(p_ffmpeg_opt->ctx->codec->id);
	if (p_ffmpeg_opt->ctx->parser == nullptr) {
     std::printf(
     p_error,
     "failed to find the parser. trace info: %s",
     _trace);
     return ret = -1;
	}

  ret = avcodec_open2(p_ffmpeg_opt->ctx->context, p_ffmpeg_opt->ctx->codec, nullptr);
	if (ret < 0) {
     std::printf(
     p_error,
     "failed to open the codec. trace info: %s",
     _trace);
     return ret = -1;
	}

  return ret;
}

auto w_ffmpeg_encode_frame(w_ffmpeg_ctx p_ffmpeg_ctx, int& p_index, uint8_t** p_data_out, int* p_size_out, char *p_error) -> int {

  constexpr auto _trace = "ffmpeg::w_ffmpeg_encode_frame";

  int ret = 0;

  if(p_ffmpeg_ctx == nullptr || p_error == nullptr) {
       std::printf(
       p_error,
       "bad input parameters. trace info: %s",
       _trace);
       return ret = -1;
  }

	av_packet_unref(p_ffmpeg_ctx->packet);
	p_ffmpeg_ctx->packet->data = nullptr;
	p_ffmpeg_ctx->packet->size = 0;

    ret = avcodec_send_frame(p_ffmpeg_ctx->context, p_ffmpeg_ctx->yuv_frame);
	if (ret < 0) {
		char error[AV_ERROR_MAX_STRING_SIZE] = { 0 };

		av_make_error_string(error, sizeof(error), ret);

    std::printf(
       p_error,
       "failed to send the frame for encoding, because %s. trace info: %s",
       error,
       _trace);
       return ret = -1;
	}

	while (ret >= 0) {
		ret = avcodec_receive_packet(p_ffmpeg_ctx->context, p_ffmpeg_ctx->packet);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
      return ret;
    }
		else if (ret < 0) {
		  char error[AV_ERROR_MAX_STRING_SIZE] = { 0 };

		  av_make_error_string(error, sizeof(error), ret);

      std::printf(
       p_error,
       "error during the encoding, because %s. trace info: %s",
       error,
       _trace);
       return ret = -1;
		}

    memcpy(*p_data_out + p_index, p_ffmpeg_ctx->packet->data, p_ffmpeg_ctx->packet->size);
    p_index += p_ffmpeg_ctx->packet->size;
    *p_size_out += p_ffmpeg_ctx->packet->size;

		av_packet_unref(p_ffmpeg_ctx->packet);
	}

   return ret;
}

auto w_ffmpeg_encode(w_ffmpeg_opt p_ffmpeg_opt, uint8_t *p_data_in, uint8_t** p_data_out, int* p_size_out, char *p_error) -> int{

  constexpr auto _trace = "ffmpeg::w_ffmpeg_encode";

  int ret = 0;
  int write_index = 0;
  *p_size_out = 0;

  // we, ourself initialize the output buffer
  if(p_ffmpeg_opt == nullptr || p_error == nullptr 
     || p_data_in == nullptr) {
       std::printf(
       p_error,
       "bad input parameters. trace info: %s",
       _trace);
       return ret = -1;
  }

  // allocate buffer to w_ffmpeg_ctx
  p_ffmpeg_opt->ctx = (w_ffmpeg_ctx_t*) malloc(sizeof(w_ffmpeg_ctx_t));
  p_ffmpeg_opt->ctx->codec_id = (AVCodecID)p_ffmpeg_opt->codec_id;
  p_ffmpeg_opt->ctx->format = AV_PIX_FMT_YUV420P;

  // finally will be rised at the end
  auto _ = gsl::finally([&] {

    if(ret != 0) {
      if (p_ffmpeg_opt->ctx->packet != nullptr) {
          av_packet_free(&p_ffmpeg_opt->ctx->packet);
      }

      if (p_ffmpeg_opt->ctx->context != nullptr) {
          avcodec_close(p_ffmpeg_opt->ctx->context);
      }

      if (p_ffmpeg_opt->ctx->yuv_frame != nullptr) {
          av_frame_free(&(p_ffmpeg_opt->ctx->yuv_frame));
      }

      if (p_ffmpeg_opt->ctx != nullptr)
      {
          free(p_ffmpeg_opt->ctx);
      }
    }
 });

  ret = w_ffmpeg_init(p_ffmpeg_opt->ctx, p_error);
  if (ret < 0)
  {
      return ret = -1;
  }

  // initialize decoder
  ret = w_ffmpeg_init_encoder(p_ffmpeg_opt, p_error);
  if (ret < 0)
  {
      return ret = -1;
  }

  // convert rgb to yuv
  ret = rgb2yuv(p_data_in, p_ffmpeg_opt->width, p_ffmpeg_opt->height, &(p_ffmpeg_opt->ctx)->yuv_frame, p_error);
  if(ret < 0) {
    return ret = -1;
  }

  // initialize the output buffer
  *p_data_out = (uint8_t*) malloc(p_ffmpeg_opt->width * p_ffmpeg_opt->height * 3);

  // start encoding
  auto result = w_ffmpeg_encode_frame(p_ffmpeg_opt->ctx, write_index, p_data_out, p_size_out, p_error);
  if (ret == -1)
  {
      return ret = -1;
  }

  // finish encoding
  p_ffmpeg_opt->ctx->yuv_frame = nullptr;
  result = w_ffmpeg_encode_frame(p_ffmpeg_opt->ctx, write_index, p_data_out, p_size_out, p_error);
  if(ret == -1) {
    return ret = -1;
  }

  return ret;
}

auto w_ffmpeg_decode_frame(w_ffmpeg_ctx p_ffmpeg_ctx, int& p_index, uint8_t** p_out_data, char *p_error) -> int {

  constexpr auto _trace = "ffmpeg::w_ffmpeg_decode_frame";
	int ret = 0;

	ret = avcodec_send_packet(p_ffmpeg_ctx->context, p_ffmpeg_ctx->packet);
	if (ret < 0) {
		char error[AV_ERROR_MAX_STRING_SIZE] = { 0 };

		av_make_error_string(error, sizeof(error), ret);

    std::printf(
       p_error,
       "failed to send the packet for decoding, because %s. trace info: %s",
       error,
       _trace);
       return ret = -1;
	}

	while (ret >= 0)
	{
		ret = avcodec_receive_frame(p_ffmpeg_ctx->context, p_ffmpeg_ctx->yuv_frame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
			return ret;
    }
		else if (ret < 0)
		{
      char error[AV_ERROR_MAX_STRING_SIZE] = { 0 };

      av_make_error_string(error, sizeof(error), ret);

      std::printf(
        p_error,
        "error during decoding, because %s. trace info: %s",
        error,
        _trace);
        return ret = -1;
		}

		ret = yuv2rgb(p_ffmpeg_ctx->yuv_frame, p_ffmpeg_ctx->yuv_frame->width, 
                p_ffmpeg_ctx->yuv_frame->height, p_ffmpeg_ctx->rgb_frame, p_error);

    if (ret < 0) {
      std::printf(
        p_error,
        "failed in converting yuv to rgb. trace info: %s",
        _trace);
        return ret = -1;
    }

    //write_JPEG_file(p_ffmpeg_ctx->rgb_frame->data, (char*)"output.jpg", 95, p_ffmpeg_ctx->rgb_frame->width, p_ffmpeg_ctx->rgb_frame->height);
    
    //for (int i = 0; i < p_ffmpeg_ctx->rgb_frame->height; i++)
    //{
    //    memcpy(*p_out_data + p_index + i * p_ffmpeg_ctx->rgb_frame->linesize[0], p_ffmpeg_ctx->rgb_frame->data + i * p_ffmpeg_ctx->rgb_frame->linesize[0], 64);
    //    p_index += p_ffmpeg_ctx->rgb_frame->width;
    //}
	}

  return ret;
}

auto w_ffmpeg_decode(w_ffmpeg_opt p_ffmpeg_opt, uint8_t* p_data_in, int p_data_size, uint8_t** p_data_out, char *p_error) -> int{

  constexpr auto _trace = "ffmpeg::w_ffmpeg_decode";

  int ret = 0;
  int write_index = 0;

  int size = p_data_size;
  uint8_t* data = p_data_in;

  // we, ourself initialize the output buffer
  if(p_ffmpeg_opt == nullptr || p_error == nullptr
       || p_data_in == nullptr) {
       std::printf(
       p_error,
       "bad input parameters. trace info: %s",
       _trace);
       return ret = -1;
  }

  // finally will be rised at the end
  auto _ = gsl::finally([&] {

    if(ret != 0) {
      if (p_ffmpeg_opt->ctx->packet != nullptr) {
          av_packet_free(&p_ffmpeg_opt->ctx->packet);
      }

      if (p_ffmpeg_opt->ctx->context != nullptr) {
          avcodec_close(p_ffmpeg_opt->ctx->context);
      }

      if (p_ffmpeg_opt->ctx->yuv_frame != nullptr) {
          av_frame_free(&(p_ffmpeg_opt->ctx->yuv_frame));
      }

      if (p_ffmpeg_opt->ctx->rgb_frame != nullptr) {
          av_frame_free(&(p_ffmpeg_opt->ctx->rgb_frame));
      }

      if (p_ffmpeg_opt->ctx != nullptr)
      {
          free(p_ffmpeg_opt->ctx);
      }
    }
  });

  // allocate buffer to w_ffmpeg_ctx
  p_ffmpeg_opt->ctx = (w_ffmpeg_ctx_t *)malloc(sizeof(w_ffmpeg_ctx_t));
  p_ffmpeg_opt->ctx->codec_id = (AVCodecID)p_ffmpeg_opt->codec_id;
  //p_ffmpeg_opt->ctx->format = AV_PIX_FMT_YUV420P;

  ret = w_ffmpeg_init(p_ffmpeg_opt->ctx, p_error);
  if (ret < 0) {
      return ret = -1;
  }

  // initialize decoder
  ret = w_ffmpeg_init_decoder(p_ffmpeg_opt, p_error);
  if(ret < 0) {
    return ret = -1;
  }

  // initialize the output buffer
  *p_data_out = (uint8_t*) malloc(p_ffmpeg_opt->width * p_ffmpeg_opt->height * 3);

  while (size > 0) {
		ret = av_parser_parse2(p_ffmpeg_opt->ctx->parser, p_ffmpeg_opt->ctx->context, &(p_ffmpeg_opt->ctx->packet)->data,
    &(p_ffmpeg_opt->ctx->packet)->size, data, size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

		if (ret < 0) {
     std::printf(
     p_error,
     "failed in parsing during decoding. trace info: %s",
     _trace);
     return ret = -1;
		}
		data += ret;
		size -= ret;

		if (&(p_ffmpeg_opt->ctx->packet)->size) {
			w_ffmpeg_decode_frame(p_ffmpeg_opt->ctx, write_index, p_data_out, p_error);
    }
	}

	/* flush the decoder */
  p_ffmpeg_opt->ctx->packet = nullptr;
	w_ffmpeg_decode_frame(p_ffmpeg_opt->ctx, write_index, p_data_out, p_error);

  return ret;
}

auto w_ffmpeg_fini(w_ffmpeg_opt p_ffmpeg_opt) -> void{

  constexpr auto _trace = "ffmpeg::w_ffmpeg_fini";

  if (p_ffmpeg_opt->ctx->packet != nullptr) {
      av_packet_free(&(p_ffmpeg_opt->ctx->packet));
  }

  if (p_ffmpeg_opt->ctx->context != nullptr) {
      avcodec_close(p_ffmpeg_opt->ctx->context);
  }

  if (p_ffmpeg_opt->ctx->yuv_frame != nullptr) {
      av_frame_free(&(p_ffmpeg_opt->ctx->yuv_frame));
  }

  if (p_ffmpeg_opt->ctx->rgb_frame != nullptr) {
      av_frame_free(&(p_ffmpeg_opt->ctx->rgb_frame));
  }

  if (p_ffmpeg_opt->ctx != nullptr)
  {
      free(p_ffmpeg_opt->ctx);
  }
}

#pragma endregion