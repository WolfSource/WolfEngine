/*
      Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
      https://github.com/WolfEngine/WolfEngine
*/

#include "ffmpeg.h"
#include "conversion.h"
#include <cstdint>
#include <gsl/gsl> //NO-LINT
#include <wolf.h>

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

#include <cstdarg>
#include <gsl/gsl> //NO-LINT
#include <gsl/span>
#include <vector>

#pragma region types

typedef AVFrame *w_av_frame;

#pragma endregion

#pragma region structs

typedef struct w_ffmpeg_ctx_t
{
   AVCodecID codec_id = AV_CODEC_ID_NONE;
   AVCodecContext *context = nullptr;
   AVPacket *packet = nullptr;
   AVFrame *rgb_frame = nullptr;
   AVFrame *yuv_frame = nullptr;
   const AVCodec *codec = nullptr;
   AVCodecParserContext *parser = nullptr;
}w_ffmpeg_ctx_t;

typedef w_ffmpeg_ctx_t *w_ffmpeg_ctx;

#pragma endregion

#pragma region enums

typedef enum w_ffmpeg_type
{
    W_NONE,
    W_PACKET,
    W_CONTEXT,
    W_FRAME,
    W_PARSER,
    W_DATA
}w_ffmpeg_type;

#pragma endregion

constexpr int MAX_ARRAY_SIZE = 6;

#pragma region functions

//w_ffmpeg_opt_t::w_ffmpeg_opt_t()
//{
//    ctx = nullptr;
//}

template<typename T>
bool check_null(T a)
{
    return a == nullptr;
}

template<typename T, typename... Args>
bool check_null(T a, Args... args)
{
    return a == nullptr || check_null(args...);
}

void make_error(char *p_error, const char *const &p_trace, const char *const p_error_msg, int p_error_code)
{
    std::array<char, _MAX_PATH> error_log = {0};
    av_make_error_string(error_log.data(), _MAX_PATH, p_error_code);

    std::sprintf(
        p_error,
        "%s, because %s. trace info: %s",
        p_error_msg,
        error_log.data(),
        p_trace);
}

void cleanup(const std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE>* p_arg)
{
    bool flag = true;
    bool check = false;
    std::pair<int, void **> j;
    std::vector<std::pair<int, void **>> i;

#pragma unroll
    for (int t = 0; t < gsl::narrow_cast<int>(p_arg->size()); t++)
    {
        i = p_arg->at(t);
        flag = true;
        check = false;

#pragma unroll 10
        for (int k = 0; k < gsl::narrow_cast<int>(i.size()); k++)
        {
            check = true;
            j = i.at(k);
            flag &= !check_null(*j.second);
            if (!(flag))
            {
                break;
            }
        }

        if (flag && check)
        {
            switch (j.first)
            {
                case W_PACKET:
                {
                    auto *packet = static_cast<AVPacket *>(*(j.second));
                    av_packet_free(&packet);
                    *(j.second) = nullptr;
                    continue;
                }

                case W_CONTEXT:
                {
                    auto *context = static_cast<AVCodecContext *>(*(j.second));
                    avcodec_close(context);
                    avcodec_free_context(&context);
                    *(j.second) = nullptr;
                    continue;
                }

                case W_FRAME:
                {
                    auto *frame = static_cast<AVFrame *>(*(j.second));
                    av_frame_free(&frame);
                    *(j.second) = nullptr;
                    continue;
                }

                case W_PARSER:
                {
                    auto *parser = static_cast<AVCodecParserContext *>(*(j.second));
                    av_parser_close(parser);
                    *(j.second) = nullptr;
                    continue;
                }

                case W_DATA:
                {
                    free(*(j.second));
                    *(j.second) = nullptr;
                    continue;
                }

                default:
                break;
            }
        }
    }
}

struct final_action
{
    explicit final_action(std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE> *a, int *ret): arg{a}, ret{ret}
    {
    }

    ~final_action()
    {
        if (*ret == -1)
        {
            cleanup(arg);
        }
    }

private:
    std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE>* arg;
    int* ret = nullptr;
};

final_action finally(std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE>* arg, int* ret)
{
    return final_action{arg, ret};
}

void to_cleanup(w_ffmpeg_opt* p_ffmpeg_opt, std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE>& to_cleanup)
{
    std::vector<std::pair<int, void **>> arg1;
    arg1.emplace_back(std::make_pair(W_DATA, (void **)p_ffmpeg_opt));
    arg1.emplace_back(std::make_pair(W_DATA, (void **)&(*p_ffmpeg_opt)->ctx));
    arg1.emplace_back(std::make_pair(W_PACKET, (void **)&(*p_ffmpeg_opt)->ctx->packet));

    std::vector<std::pair<int, void **>> arg2;
    arg2.emplace_back(std::make_pair(W_DATA, (void **)p_ffmpeg_opt));
    arg2.emplace_back(std::make_pair(W_DATA, (void **)&(*p_ffmpeg_opt)->ctx));
    arg2.emplace_back(std::make_pair(W_CONTEXT, (void **)&(*p_ffmpeg_opt)->ctx->context));

    std::vector<std::pair<int, void **>> arg3;
    arg3.emplace_back(std::make_pair(W_DATA, (void **)p_ffmpeg_opt));
    arg3.emplace_back(std::make_pair(W_DATA, (void **)&(*p_ffmpeg_opt)->ctx));
    arg3.emplace_back(std::make_pair(W_FRAME, (void **)&(*p_ffmpeg_opt)->ctx->yuv_frame));

    std::vector<std::pair<int, void **>> arg4;
    arg4.emplace_back(std::make_pair(W_DATA, (void **)p_ffmpeg_opt));
    arg4.emplace_back(std::make_pair(W_DATA, (void **)&(*p_ffmpeg_opt)->ctx));
    arg4.emplace_back(std::make_pair(W_FRAME, (void **)&(*p_ffmpeg_opt)->ctx->rgb_frame));

    to_cleanup[0] = arg1;
    to_cleanup[1] = arg2;
    to_cleanup[2] = arg3;
    to_cleanup[3] = arg4;
}

int w_ffmpeg_init_encoder(w_ffmpeg_opt p_ffmpeg_opt, char *p_error) 
{
 constexpr auto _trace = "ffmpeg::w_ffmpeg_init_encoder";
 int ret = 0;

 // finally will be raised at the end
 std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE> cleanup;

 auto _ = finally(&cleanup, &ret);  // establish exit action

   p_ffmpeg_opt->ctx->codec = avcodec_find_encoder(gsl::narrow_cast<AVCodecID>(p_ffmpeg_opt->codec_id));
   if (check_null(p_ffmpeg_opt->ctx->codec))
   {
       make_error(p_error, _trace, "failed to find the specified coded", 0);
       return ret = -1;
   }

   p_ffmpeg_opt->ctx->context = avcodec_alloc_context3(p_ffmpeg_opt->ctx->codec);
   std::vector<std::pair<int, void **>> arg1;
   arg1.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt))));
   arg1.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));
   arg1.emplace_back(std::make_pair(W_CONTEXT, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx->context))));

   cleanup[0] = arg1;
   if (check_null(p_ffmpeg_opt->ctx->context))
   {
       make_error(p_error, _trace, "failed to allocate video codec context", 0);
       return ret = -1;
   }

   (p_ffmpeg_opt->ctx->context)->bit_rate = p_ffmpeg_opt->bitrate;
   (p_ffmpeg_opt->ctx->context)->width = gsl::narrow_cast<int>(p_ffmpeg_opt->width);
   (p_ffmpeg_opt->ctx->context)->height = gsl::narrow_cast<int>(p_ffmpeg_opt->height);
   (p_ffmpeg_opt->ctx->context)->time_base.num = 1;
   (p_ffmpeg_opt->ctx->context)->time_base.den = p_ffmpeg_opt->fps;
   (p_ffmpeg_opt->ctx->context)->pix_fmt = AV_PIX_FMT_YUV420P;

   if (p_ffmpeg_opt->codec_id == AV_CODEC_ID_H264)
   {
       ret = av_opt_set(p_ffmpeg_opt->ctx->context->priv_data, "preset", "slow", 0);

       if (ret < 0)
       {
           make_error(p_error, _trace, "failed to set the options", ret);
           return ret = -1;
       }
    }

   ret = avcodec_open2(p_ffmpeg_opt->ctx->context, p_ffmpeg_opt->ctx->codec, nullptr);
	if (ret < 0) {
        make_error(p_error, _trace, "failed to open the codec", ret);
        return ret = -1;
	}

 return ret;
}

int w_ffmpeg_init_decoder(w_ffmpeg_opt p_ffmpeg_opt, char *p_error) {

 constexpr auto _trace = "ffmpeg::w_ffmpeg_init_decoder";
 int ret = 0;

 if (check_null(p_ffmpeg_opt, p_error))
 {
    return ret = -1;
 }

 // finally will be raised at the end
 std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE> cleanup;

 auto _ = finally(&cleanup, &ret);  // establish exit action

 p_ffmpeg_opt->ctx->codec = avcodec_find_decoder(gsl::narrow_cast<AVCodecID>(p_ffmpeg_opt->codec_id));
 if (check_null(p_ffmpeg_opt->ctx->codec))
 {
     make_error(p_error, _trace, "failed to find the specified coded", 0);
     return ret = -1;
 }

 p_ffmpeg_opt->ctx->context = avcodec_alloc_context3(p_ffmpeg_opt->ctx->codec);
 std::vector<std::pair<int, void **>> arg1;
 arg1.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt))));
 arg1.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));
 arg1.emplace_back(std::make_pair(W_CONTEXT, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx->context))));
 cleanup[0] = arg1;

 if (check_null(p_ffmpeg_opt->ctx->context))
 {
     make_error(p_error, _trace, "failed to allocate video codec context", 0);
     return ret = -1;
 }

 p_ffmpeg_opt->ctx->rgb_frame = av_frame_alloc();
 std::vector<std::pair<int, void **>> arg2;
 arg2.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt))));
 arg2.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));
 arg2.emplace_back(std::make_pair(W_FRAME, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx->rgb_frame))));
 cleanup[1] = arg2;

 if (check_null(p_ffmpeg_opt->ctx->rgb_frame))
 {
     make_error(p_error, _trace, "failed to allocate the frame", 0);
    return ret = -1;
 }

 const auto * const frame_buffer_out = gsl::narrow_cast<const uint8_t *>(av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24, gsl::narrow_cast<int>(p_ffmpeg_opt->width), gsl::narrow_cast<int>(p_ffmpeg_opt->height), 1)));
 std::vector<std::pair<int, void **>> arg3;
 arg3.emplace_back(std::make_pair(W_DATA, (void **)(&(frame_buffer_out))));
 cleanup[2] = arg3;

 if (check_null(frame_buffer_out))
 {
     make_error(p_error, _trace, "failed to allocate the buffer for frame", 0);
     return ret = -1;
 }

 auto result = av_image_fill_arrays(static_cast<uint8_t **>(p_ffmpeg_opt->ctx->rgb_frame->data), static_cast<int *>(p_ffmpeg_opt->ctx->rgb_frame->linesize), frame_buffer_out, AV_PIX_FMT_RGB24, gsl::narrow_cast<int>(p_ffmpeg_opt->width), gsl::narrow_cast<int>(p_ffmpeg_opt->height), 1);
    if (result < 0)
    {
        make_error(p_error, _trace, "failed to find fill the image", ret);
        return ret = -1;
    }

   p_ffmpeg_opt->ctx->parser = av_parser_init(p_ffmpeg_opt->ctx->codec->id);
   std::vector<std::pair<int, void **>> arg4;
   arg4.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt))));
   arg4.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));
   arg4.emplace_back(std::make_pair(W_PARSER, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx->parser))));
   cleanup[3] = arg4;

   if (check_null(p_ffmpeg_opt->ctx->parser))
   {
        make_error(p_error, _trace, "failed to find the parser", 0);
        return ret = -1;
	}

 ret = avcodec_open2(p_ffmpeg_opt->ctx->context, p_ffmpeg_opt->ctx->codec, nullptr);
	if (ret < 0) {
        make_error(p_error, _trace, "failed to open the codec", ret);
    return ret = -1;
	}

 return ret;
}

int w_ffmpeg_init(w_ffmpeg_opt p_ffmpeg_opt, char *p_error)
{
    constexpr auto _trace = "ffmpeg::w_ffmpeg_init";

    int ret = 0;

    if (check_null(p_ffmpeg_opt, p_error))
    {
        return -1;
    }

    // finally will be raised at the end
    std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE> cleanup;

    auto _ = finally(&cleanup, &ret);  // establish exit action

    p_ffmpeg_opt->ctx = gsl::narrow_cast<w_ffmpeg_ctx_t *>(calloc(1, sizeof(w_ffmpeg_ctx_t)));
    std::vector<std::pair<int, void **>> arg1;
    arg1.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&p_ffmpeg_opt)));
    arg1.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));
    cleanup[0] = arg1;

    if (check_null(p_ffmpeg_opt->ctx))
    {
        make_error(p_error, _trace, "failed to allocate the context", 0);
        return ret = -1;
    }

    p_ffmpeg_opt->ctx->packet = av_packet_alloc();
    std::vector<std::pair<int, void **>> arg2;
    arg2.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));
    arg2.emplace_back(std::make_pair(W_PACKET, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx->packet))));

    cleanup[1] = arg2;

    if (check_null(p_ffmpeg_opt->ctx->packet))
    {
        make_error(p_error, _trace, "failed to allocate the packet", 0);
        return ret = -1;
    }

    p_ffmpeg_opt->ctx->yuv_frame = av_frame_alloc();
    std::vector<std::pair<int, void **>> arg3;
    arg3.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));
    arg3.emplace_back(std::make_pair(W_FRAME, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx->yuv_frame))));

    cleanup[2] = arg3;

    if (check_null(p_ffmpeg_opt->ctx->yuv_frame))
    {
        make_error(p_error, _trace, "failed to allocate the frame", 0);
        return ret = -1;
    }

    switch (p_ffmpeg_opt->type)
    {
        case W_ENCODE:
        {
            // initialize decoder
            if (w_ffmpeg_init_encoder(p_ffmpeg_opt, p_error) < 0)
            {
                ret = -1;
            }
            break;
        }

        case W_DECODE:
        {
            // initialize decoder
            if (w_ffmpeg_init_decoder(p_ffmpeg_opt, p_error) < 0)
            {
                ret = -1;
            }
            break;
        }
        default:
        {
            make_error(p_error, _trace, "ffmpeg action not found. please provide ffmpeg action (e.g W_ENCODE or W_DECODE)", 0);
            ret = -1;
        }
        break;
    }

    return ret;
}

int w_ffmpeg_encode_frame(w_ffmpeg_ctx p_ffmpeg_ctx, int& p_index, uint8_t** p_data_out, int* p_size_out, char *p_error){

 constexpr auto _trace = "ffmpeg::w_ffmpeg_encode_frame";

 int ret = 0;
 int tmp_res = 0;
 const bool flag = true;

 if (check_null(p_ffmpeg_ctx, p_error))
 {
      return -1;
 }

	av_packet_unref(p_ffmpeg_ctx->packet);
	p_ffmpeg_ctx->packet->data = nullptr;
	p_ffmpeg_ctx->packet->size = 0;

   ret = avcodec_send_frame(p_ffmpeg_ctx->context, p_ffmpeg_ctx->yuv_frame);
	if (ret < 0) {
        make_error(p_error, _trace, "failed to send the frame for encoding", ret);
      return -1;
	}

#pragma unroll
	while (flag) {
        tmp_res = avcodec_receive_packet(p_ffmpeg_ctx->context, p_ffmpeg_ctx->packet);
		if (tmp_res == AVERROR(EAGAIN) || tmp_res == AVERROR_EOF) {
            break;
   }
		if (tmp_res < 0) {
            make_error(p_error, _trace, "error during the encoding", ret);
      return -1;
		}

   memcpy(*p_data_out + p_index, p_ffmpeg_ctx->packet->data, p_ffmpeg_ctx->packet->size);
   p_index += p_ffmpeg_ctx->packet->size;
   *p_size_out += p_ffmpeg_ctx->packet->size;

		av_packet_unref(p_ffmpeg_ctx->packet);
	}

  return ret;
}

int w_ffmpeg_encode(w_ffmpeg_opt p_ffmpeg_opt, uint8_t *p_data_in, uint8_t **p_data_out, int *p_size_out, char *p_error){

 constexpr auto _trace = "ffmpeg::w_ffmpeg_encode";

 int ret = 0;

 // we, ourself initialize the output buffer
 if (check_null(p_ffmpeg_opt, p_ffmpeg_opt->ctx, p_error, p_data_in))
 {
      return -1;
 }

 *p_size_out = 0;
 int write_index = 0;

  // finally will be rised at the end
 std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE> cleanup;

  auto _ = finally(&cleanup, &ret);  // establish exit action

    // allocate buffer to w_ffmpeg_ctx

  to_cleanup(&p_ffmpeg_opt, cleanup);

  std::vector<std::pair<int, void **>> arg5;
  arg5.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(p_data_out)));

  std::vector<std::pair<int, void **>> arg6;
  arg6.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&p_ffmpeg_opt)));
  arg6.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));

  cleanup[4] = arg5;
  cleanup[5] = arg6;

  p_ffmpeg_opt->ctx->codec_id = gsl::narrow_cast<AVCodecID>(p_ffmpeg_opt->codec_id);

 // convert rgb to yuv
 if(rgb2yuv(p_data_in, p_ffmpeg_opt->width, p_ffmpeg_opt->height, &(p_ffmpeg_opt->ctx)->yuv_frame, p_error) < 0) 
 {
   return ret = -1;
 }

 // initialize the output buffer
 *p_data_out = gsl::narrow_cast<uint8_t *>(calloc(1, p_ffmpeg_opt->width * p_ffmpeg_opt->height * 3));
 if (check_null(*p_data_out))
 {
     make_error(p_error, _trace, "failed to allocate the output buffer", 0);
     return ret = -1;
 }

 // start encoding
 if (w_ffmpeg_encode_frame(p_ffmpeg_opt->ctx, write_index, p_data_out, p_size_out, p_error) < 0)
 {
     return ret = -1;
 }

 // finish encoding
 p_ffmpeg_opt->ctx->yuv_frame = nullptr;
 if(w_ffmpeg_encode_frame(p_ffmpeg_opt->ctx, write_index, p_data_out, p_size_out, p_error) < 0) 
 {
   return ret = -1;
 }

 return ret;
}

int w_ffmpeg_decode_frame(w_ffmpeg_ctx p_ffmpeg_ctx, uint8_t** p_out_data,int *p_size_out, char *p_error) {

 constexpr auto _trace = "ffmpeg::w_ffmpeg_decode_frame";
 int ret = 0;
 int tmp_res = 0;
 const bool flag = true;

	ret = avcodec_send_packet(p_ffmpeg_ctx->context, p_ffmpeg_ctx->packet);
    if (ret < 0)
    {
        make_error(p_error, _trace, "failed to send the packet", 0);
        return -1;
    }

#pragma unroll
	while (flag)
	{
        tmp_res = avcodec_receive_frame(p_ffmpeg_ctx->context, p_ffmpeg_ctx->yuv_frame);
		if (tmp_res == AVERROR(EAGAIN) || tmp_res == AVERROR_EOF) {
            break;
   }
		if (tmp_res < 0)
		{
            make_error(p_error, _trace, "error during decoding", tmp_res);
       return -1;
		}

		tmp_res = yuv2rgb(p_ffmpeg_ctx->yuv_frame, p_ffmpeg_ctx->yuv_frame->width, 
               p_ffmpeg_ctx->yuv_frame->height, p_ffmpeg_ctx->rgb_frame, p_error);
   if (tmp_res < 0) {
       make_error(p_error, _trace, "failed in converting yuv to rgb", tmp_res);
       return -1;
   }

   (*p_out_data) = p_ffmpeg_ctx->rgb_frame->data[0];
   tmp_res  = av_image_get_buffer_size(
       gsl::narrow_cast<AVPixelFormat>(p_ffmpeg_ctx->rgb_frame->format),
       p_ffmpeg_ctx->rgb_frame->width,
       p_ffmpeg_ctx->rgb_frame->height,
       3
   );
   if (tmp_res < 0)
   {
       make_error(p_error, _trace, "failed in get the size of the frame buffer", 0);
       return -1;
   }

   (*p_size_out) = tmp_res;
	}

 return ret;
}

int w_ffmpeg_decode(w_ffmpeg_opt p_ffmpeg_opt, uint8_t* p_data_in, int p_data_size, uint8_t** p_data_out,int *p_size_out, char *p_error) {

 constexpr auto _trace = "ffmpeg::w_ffmpeg_decode";

 int ret = 0;

 // we, ourself initialize the output buffer
 if (check_null(p_ffmpeg_opt, p_ffmpeg_opt->ctx, p_error, p_data_in))
 {
      return -1;
 }

 int tmp_res = 0;
 int size = p_data_size;
 uint8_t *data = p_data_in;
 const bool flag = true;

  // finally will be rised at the end
 std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE> cleanup;
 to_cleanup(&p_ffmpeg_opt, cleanup);

 std::vector<std::pair<int, void **>> arg5;
 arg5.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(p_data_out)));

 std::vector<std::pair<int, void **>> arg6;
 arg6.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&p_ffmpeg_opt)));
 arg6.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));

 cleanup[4] = arg5;
 cleanup[5] = arg6;

  auto _ = finally(&cleanup, &ret);  // establish exit action
 
 p_ffmpeg_opt->ctx->codec_id = gsl::narrow_cast<AVCodecID>(p_ffmpeg_opt->codec_id);

 // initialize the output buffer
 *p_data_out = gsl::narrow_cast<uint8_t *>(calloc(1, p_ffmpeg_opt->width * p_ffmpeg_opt->height * 3));
 if (check_null(*p_data_out))
 {
     make_error(p_error, _trace, "failed to allocate the output buffer", 0);
     return ret = -1;
 }

#pragma unroll
 while (flag) {
     tmp_res = av_parser_parse2(p_ffmpeg_opt->ctx->parser, p_ffmpeg_opt->ctx->context, &(p_ffmpeg_opt->ctx->packet)->data,
   &(p_ffmpeg_opt->ctx->packet)->size, data, size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

		if (tmp_res < 0) {
            make_error(p_error, _trace, "failed in parsing during decoding", 0);
    return ret = -1;
		}
		
        data += tmp_res;
		size -= tmp_res;

		if (p_ffmpeg_opt->ctx->packet->size > 0) {
			ret = w_ffmpeg_decode_frame(p_ffmpeg_opt->ctx, p_data_out, p_size_out, p_error);
   }

        if (size <= 0)
        {
            break;
        }
	}

	/* flush the decoder */
 p_ffmpeg_opt->ctx->packet = nullptr;
	ret = w_ffmpeg_decode_frame(p_ffmpeg_opt->ctx, p_data_out, p_size_out, p_error);

 return ret;
}

void w_ffmpeg_fini(w_ffmpeg_opt p_ffmpeg_opt) 
{
    int ret = -1;
    std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE> cleanup;

    auto _ = finally(&cleanup, &ret);  // establish exit action

    to_cleanup(&p_ffmpeg_opt, cleanup);

    std::vector<std::pair<int, void **>> arg5;
    arg5.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&p_ffmpeg_opt)));
    arg5.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));
    arg5.emplace_back(std::make_pair(W_PARSER, reinterpret_cast<void **>(& (p_ffmpeg_opt->ctx->parser))));

    std::vector<std::pair<int, void **>> arg6;
    arg6.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&p_ffmpeg_opt)));
    arg6.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg_opt->ctx))));

    cleanup[4] = arg5;
    cleanup[5] = arg6;
}

#pragma endregion