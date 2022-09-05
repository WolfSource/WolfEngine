#include "ffmpeg.h"
#include <wolf.hpp>

#include <cstdint>
#include <cstdarg>
#include <vector>

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

// types
typedef AVFrame* w_av_frame;

typedef struct w_ffmpeg_ctx_t
{
    AVCodecID codec_id = AV_CODEC_ID_NONE;
    AVCodecContext* context = nullptr;
    AVPacket* packet = nullptr;
    AVFrame* rgb_frame = nullptr;
    AVFrame* yuv_frame = nullptr;
    const AVCodec* codec = nullptr;
    AVCodecParserContext* parser = nullptr;
}w_ffmpeg_ctx_t;

typedef enum w_ffmpeg_type
{
    W_NONE,
    W_PACKET,
    W_CONTEXT,
    W_FRAME,
    W_PARSER,
    W_DATA
}w_ffmpeg_type;

static void s_make_error(
    _Inout_z_ char* p_error,
    _In_z_ const char* const p_trace,
    _In_z_ const char* const p_error_msg,
    _In_ int p_error_code) noexcept
{
    std::array<char, W_MAX_PATH> _error_log = { 0 };
    av_make_error_string(_error_log.data(), W_MAX_PATH, p_error_code);

    snprintf(
        p_error,
        W_MAX_PATH,
        "%s, because %s. trace info: %s",
        p_error_msg,
        _error_log.data(),
        p_trace);
}

static int s_init_encoder(_Inout_ w_ffmpeg p_ffmpeg, _Inout_z_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::s_init_encoder";
    int _ret = 0;

    const auto _codec_id = gsl::narrow_cast<AVCodecID>(p_ffmpeg->codec_id);
    p_ffmpeg->ctx->codec = avcodec_find_encoder(_codec_id);

    if (w_is_null(p_ffmpeg->ctx->codec))
    {
        s_make_error(p_error, TRACE, "failed to find the specified coded for encoder", 0);
        return _ret = -1;
    }

    p_ffmpeg->ctx->context = avcodec_alloc_context3(p_ffmpeg->ctx->codec);
    if (w_is_null(p_ffmpeg->ctx->context))
    {
        s_make_error(p_error, TRACE, "failed to allocate memory for ffmpeg context", 0);
        return _ret = -1;
    }

    p_ffmpeg->ctx->context->bit_rate = p_ffmpeg->bitrate;
    p_ffmpeg->ctx->context->width = gsl::narrow_cast<int>(p_ffmpeg->width);
    p_ffmpeg->ctx->context->height = gsl::narrow_cast<int>(p_ffmpeg->height);
    p_ffmpeg->ctx->context->time_base.num = 1;
    p_ffmpeg->ctx->context->time_base.den = p_ffmpeg->fps;
    p_ffmpeg->ctx->context->pix_fmt = gsl::narrow_cast<AVPixelFormat>(p_ffmpeg->pix_fmt);

    //if (p_ffmpeg->codec_id == AV_CODEC_ID_H264)
    //{
    //    _ret = av_opt_set(p_ffmpeg->ctx->context->priv_data, "preset", "slow", 0);

    //    if (_ret < 0)
    //    {
    //        make_error(p_error, TRACE, "failed to set the options", ret);
    //        return ret = -1;
    //    }
    //}

    _ret = avcodec_open2(p_ffmpeg->ctx->context, p_ffmpeg->ctx->codec, nullptr);
    if (_ret < 0) 
    {
        s_make_error(p_error, TRACE, "failed to open codec", 0);
    }

    return _ret;
}

static int s_init_decoder(_Inout_ w_ffmpeg p_ffmpeg, _Inout_z_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::s_init_decoder";
    int _ret = 0;

    if (w_is_null(p_ffmpeg, p_error))
    {
        return _ret = -1;
    }

    p_ffmpeg->ctx->codec = avcodec_find_decoder(gsl::narrow_cast<AVCodecID>(p_ffmpeg->codec_id));
    if (w_is_null(p_ffmpeg->ctx->codec))
    {
        s_make_error(p_error, TRACE, "failed to find the specified coded for decoder", 0);
        return _ret = -1;
    }

    p_ffmpeg->ctx->context = avcodec_alloc_context3(p_ffmpeg->ctx->codec);
    if (w_is_null(p_ffmpeg->ctx->context))
    {
        s_make_error(p_error, TRACE, "failed to allocate video codec context", 0);
        return _ret = -1;
    }

    p_ffmpeg->ctx->rgb_frame = av_frame_alloc();
    if (w_is_null(p_ffmpeg->ctx->rgb_frame))
    {
        s_make_error(p_error, TRACE, "failed to allocate the frame", 0);
        return _ret = -1;
    }

    const auto _w = gsl::narrow_cast<int>(p_ffmpeg->width);
    const auto _h = gsl::narrow_cast<int>(p_ffmpeg->height);
    const auto _pixel_fmt = gsl::narrow_cast<AVPixelFormat>(p_ffmpeg->pix_fmt);
    const auto _buffer_size = av_image_get_buffer_size(_pixel_fmt, _w, _h, 1);

    auto* _frame_buffer = gsl::narrow_cast<const uint8_t*>(av_malloc(_buffer_size));
    if (w_is_null(_frame_buffer))
    {
        s_make_error(p_error, TRACE, "failed to allocate the buffer for frame", 0);
        return _ret = -1;
    }

    _ret = av_image_fill_arrays(
        gsl::narrow_cast<uint8_t**>(p_ffmpeg->ctx->rgb_frame->data),
        gsl::narrow_cast<int*>(p_ffmpeg->ctx->rgb_frame->linesize),
        _frame_buffer,
        _pixel_fmt,
        _w,
        _h,
        1);
    if (_ret <= 0)
    {
        s_make_error(p_error, TRACE, "could not fill image arrays", 0);
        return _ret;
    }

    p_ffmpeg->ctx->parser = av_parser_init(p_ffmpeg->ctx->codec->id);

    if (w_is_null(p_ffmpeg->ctx->parser))
    {
        s_make_error(p_error, TRACE, "could not initialize parser", 0);
        return _ret = -1;
    }

    return avcodec_open2(p_ffmpeg->ctx->context, p_ffmpeg->ctx->codec, nullptr);
}

int w_ffmpeg_init(_Inout_ w_ffmpeg* p_ffmpeg, _Inout_z_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_ffmpeg_init";

    auto _ret = 0;
    
    defer _(nullptr, [&](...)
    { 
        if (_ret != 0)
        {
            w_ffmpeg_fini(p_ffmpeg);
        }   
    });

    if (w_is_null(p_error))
    {
        return _ret = -1;
    }

    auto* _ffmpeg = gsl::owner<w_ffmpeg_t*>(calloc(1, sizeof(w_ffmpeg_t)));
    if (w_is_null(_ffmpeg))
    {
        return _ret = -1;
    }
    
    _ffmpeg->ctx = gsl::owner<w_ffmpeg_ctx>(calloc(1, sizeof(w_ffmpeg_ctx_t)));
    if (w_is_null(_ffmpeg->ctx))
    {
        return _ret = -1;
    }

    _ffmpeg->ctx->packet = av_packet_alloc();
    if (w_is_null(_ffmpeg->ctx->packet))
    {
        s_make_error(p_error, TRACE, "failed to allocate memory for the packet", 0);
        return _ret = -1;
    }

    _ffmpeg->ctx->yuv_frame = av_frame_alloc();
    if (w_is_null(_ffmpeg->ctx->yuv_frame))
    {
        s_make_error(p_error, TRACE, "failed to allocate memory for the frame", 0);
        return _ret = -1;
    }

    switch (_ffmpeg->type)
    {
    case W_ENCODE:
    {
        // initialize decoder
        _ret = s_init_encoder(_ffmpeg, p_error);
        break;
    }

    case W_DECODE:
    {
        // initialize decoder
        _ret = s_init_decoder(_ffmpeg, p_error);
        break;
    }
    default:
    {
        s_make_error(
            p_error,
            TRACE,
            "undefined ffmpeg action. please provide ffmpeg action (e.g W_ENCODE or W_DECODE)", 0);
        _ret = -1;
    }
    break;
    }

    *p_ffmpeg = _ffmpeg;
   
    return _ret;
}

//int w_ffmpeg_encode(
//    w_ffmpeg p_ffmpeg,
//    uint8_t* p_data_in,
//    uint8_t** p_data_out,
//    int* p_size_out,
//    char* p_error)
//{
//    constexpr auto TRACE = "ffmpeg::w_ffmpeg_encode";
//
//    int ret = 0;
//
//    // initialize the output buffer
//    if (w_is_null(p_ffmpeg, p_ffmpeg->ctx, p_error, p_data_in))
//    {
//        return -1;
//    }
//
//    *p_size_out = 0;
//    int write_index = 0;
//
//    p_ffmpeg->ctx->codec_id = gsl::narrow_cast<AVCodecID>(p_ffmpeg->codec_id);
//
//    // convert rgb to yuv
//    if (rgb2yuv(p_data_in, p_ffmpeg->width, p_ffmpeg->height, &(p_ffmpeg->ctx)->yuv_frame, p_error) < 0)
//    {
//        return ret = -1;
//    }
//
//    // initialize the output buffer
//    *p_data_out = gsl::narrow_cast<uint8_t*>(calloc(1, p_ffmpeg->width * p_ffmpeg->height * 3));
//    if (w_is_null(*p_data_out))
//    {
//        make_error(p_error, TRACE, "failed to allocate the output buffer", 0);
//        return ret = -1;
//    }
//
//    // start encoding
//    if (w_ffmpeg_encode_frame(p_ffmpeg->ctx, write_index, p_data_out, p_size_out, p_error) < 0)
//    {
//        return ret = -1;
//    }
//
//    // finish encoding
//    p_ffmpeg->ctx->yuv_frame = nullptr;
//    if (w_ffmpeg_encode_frame(p_ffmpeg->ctx, write_index, p_data_out, p_size_out, p_error) < 0)
//    {
//        return ret = -1;
//    }
//
//    return ret;
//}

//int w_ffmpeg_decode_frame(w_ffmpeg_ctx p_ffmpeg_ctx, uint8_t** p_out_data,int *p_size_out, char *p_error) {
//
// constexpr auto TRACE = "ffmpeg::w_ffmpeg_decode_frame";
// int ret = 0;
// int tmp_res = 0;
// const bool flag = true;
//
//	ret = avcodec_send_packet(p_ffmpeg_ctx->context, p_ffmpeg_ctx->packet);
//    if (ret < 0)
//    {
//        make_error(p_error, TRACE, "failed to send the packet", 0);
//        return -1;
//    }
//
//#pragma unroll
//	while (flag)
//	{
//        tmp_res = avcodec_receive_frame(p_ffmpeg_ctx->context, p_ffmpeg_ctx->yuv_frame);
//		if (tmp_res == AVERROR(EAGAIN) || tmp_res == AVERROR_EOF) {
//            break;
//   }
//		if (tmp_res < 0)
//		{
//            make_error(p_error, TRACE, "error during decoding", tmp_res);
//       return -1;
//		}
//
//		tmp_res = yuv2rgb(p_ffmpeg_ctx->yuv_frame, p_ffmpeg_ctx->yuv_frame->width, 
//               p_ffmpeg_ctx->yuv_frame->height, p_ffmpeg_ctx->rgb_frame, p_error);
//   if (tmp_res < 0) {
//       make_error(p_error, TRACE, "failed in converting yuv to rgb", tmp_res);
//       return -1;
//   }
//
//   (*p_out_data) = p_ffmpeg_ctx->rgb_frame->data[0];
//   tmp_res  = av_image_get_buffer_size(
//       gsl::narrow_cast<AVPixelFormat>(p_ffmpeg_ctx->rgb_frame->format),
//       p_ffmpeg_ctx->rgb_frame->width,
//       p_ffmpeg_ctx->rgb_frame->height,
//       3
//   );
//   if (tmp_res < 0)
//   {
//       make_error(p_error, TRACE, "failed in get the size of the frame buffer", 0);
//       return -1;
//   }
//
//   (*p_size_out) = tmp_res;
//	}
//
// return ret;
//}
//

//void cleanup(const std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE>* p_arg)
//{
//    bool flag = true;
//    bool check = false;
//    std::pair<int, void **> j;
//    std::vector<std::pair<int, void **>> i;
//
//#pragma unroll
//    for (int t = 0; t < gsl::narrow_cast<int>(p_arg->size()); t++)
//    {
//        i = p_arg->at(t);
//        flag = true;
//        check = false;
//
//#pragma unroll 10
//        for (int k = 0; k < gsl::narrow_cast<int>(i.size()); k++)
//        {
//            check = true;
//            j = i.at(k);
//            flag &= !w_is_null(*j.second);
//            if (!(flag))
//            {
//                break;
//            }
//        }
//
//        if (flag && check)
//        {
//            switch (j.first)
//            {
//                case W_PACKET:
//                {
//                    auto *packet = static_cast<AVPacket *>(*(j.second));
//                    av_packet_free(&packet);
//                    *(j.second) = nullptr;
//                    continue;
//                }
//
//                case W_CONTEXT:
//                {
//                    auto *context = static_cast<AVCodecContext *>(*(j.second));
//                    avcodec_close(context);
//                    avcodec_free_context(&context);
//                    *(j.second) = nullptr;
//                    continue;
//                }
//
//                case W_FRAME:
//                {
//                    auto *frame = static_cast<AVFrame *>(*(j.second));
//                    av_frame_free(&frame);
//                    *(j.second) = nullptr;
//                    continue;
//                }
//
//                case W_PARSER:
//                {
//                    auto *parser = static_cast<AVCodecParserContext *>(*(j.second));
//                    av_parser_close(parser);
//                    *(j.second) = nullptr;
//                    continue;
//                }
//
//                case W_DATA:
//                {
//                    free(*(j.second));
//                    *(j.second) = nullptr;
//                    continue;
//                }
//
//                default:
//                break;
//            }
//        }
//    }
//}

//int w_ffmpeg_encode_frame(w_ffmpeg_ctx p_ffmpeg_ctx, int& p_index, uint8_t** p_data_out, int* p_size_out, char *p_error){
//
// constexpr auto TRACE = "ffmpeg::w_ffmpeg_encode_frame";
//
// int ret = 0;
// int tmp_res = 0;
// const bool flag = true;
//
// if (w_is_null(p_ffmpeg_ctx, p_error))
// {
//      return -1;
// }
//
//	av_packet_unref(p_ffmpeg_ctx->packet);
//	p_ffmpeg_ctx->packet->data = nullptr;
//	p_ffmpeg_ctx->packet->size = 0;
//
//   ret = avcodec_send_frame(p_ffmpeg_ctx->context, p_ffmpeg_ctx->yuv_frame);
//	if (ret < 0) {
//        make_error(p_error, TRACE, "failed to send the frame for encoding", ret);
//      return -1;
//	}
//
//#pragma unroll
//	while (flag) {
//        tmp_res = avcodec_receive_packet(p_ffmpeg_ctx->context, p_ffmpeg_ctx->packet);
//		if (tmp_res == AVERROR(EAGAIN) || tmp_res == AVERROR_EOF) {
//            break;
//   }
//		if (tmp_res < 0) {
//            make_error(p_error, TRACE, "error during the encoding", ret);
//      return -1;
//		}
//
//   memcpy(*p_data_out + p_index, p_ffmpeg_ctx->packet->data, p_ffmpeg_ctx->packet->size);
//   p_index += p_ffmpeg_ctx->packet->size;
//   *p_size_out += p_ffmpeg_ctx->packet->size;
//
//		av_packet_unref(p_ffmpeg_ctx->packet);
//	}
//
//  return ret;
//}
//

//int w_ffmpeg_decode(w_ffmpeg p_ffmpeg, uint8_t* p_data_in, int p_data_size, uint8_t** p_data_out,int *p_size_out, char *p_error) {
//
// constexpr auto TRACE = "ffmpeg::w_ffmpeg_decode";
//
// int ret = 0;
//
// // we, ourself initialize the output buffer
// if (w_is_null(p_ffmpeg, p_ffmpeg->ctx, p_error, p_data_in))
// {
//      return -1;
// }
//
// int tmp_res = 0;
// int size = p_data_size;
// uint8_t *data = p_data_in;
// const bool flag = true;
//
//  // finally will be rised at the end
// std::array<std::vector<std::pair<int, void **>>, MAX_ARRAY_SIZE> cleanup;
// to_cleanup(&p_ffmpeg, cleanup);
//
// std::vector<std::pair<int, void **>> arg5;
// arg5.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(p_data_out)));
//
// std::vector<std::pair<int, void **>> arg6;
// arg6.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&p_ffmpeg)));
// arg6.emplace_back(std::make_pair(W_DATA, reinterpret_cast<void **>(&(p_ffmpeg->ctx))));
//
// cleanup[4] = arg5;
// cleanup[5] = arg6;
//
//  auto _ = finally(&cleanup, &ret);  // establish exit action
// 
// p_ffmpeg->ctx->codec_id = gsl::narrow_cast<AVCodecID>(p_ffmpeg->codec_id);
//
// // initialize the output buffer
// *p_data_out = gsl::narrow_cast<uint8_t *>(calloc(1, p_ffmpeg->width * p_ffmpeg->height * 3));
// if (w_is_null(*p_data_out))
// {
//     make_error(p_error, TRACE, "failed to allocate the output buffer", 0);
//     return ret = -1;
// }
//
//#pragma unroll
// while (flag) {
//     tmp_res = av_parser_parse2(p_ffmpeg->ctx->parser, p_ffmpeg->ctx->context, &(p_ffmpeg->ctx->packet)->data,
//   &(p_ffmpeg->ctx->packet)->size, data, size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
//
//		if (tmp_res < 0) {
//            make_error(p_error, TRACE, "failed in parsing during decoding", 0);
//    return ret = -1;
//		}
//		
//        data += tmp_res;
//		size -= tmp_res;
//
//		if (p_ffmpeg->ctx->packet->size > 0) {
//			ret = w_ffmpeg_decode_frame(p_ffmpeg->ctx, p_data_out, p_size_out, p_error);
//   }
//
//        if (size <= 0)
//        {
//            break;
//        }
//	}
//
//	/* flush the decoder */
// p_ffmpeg->ctx->packet = nullptr;
//	ret = w_ffmpeg_decode_frame(p_ffmpeg->ctx, p_data_out, p_size_out, p_error);
//
// return ret;
//}

void w_ffmpeg_fini(_Inout_ w_ffmpeg* p_ffmpeg)
{
    if (p_ffmpeg == nullptr || *p_ffmpeg == nullptr)
    {
        return;
    }

    auto _ptr = *p_ffmpeg;

    if (_ptr->ctx->packet)
    {
        av_free(_ptr->ctx->packet);
        _ptr->ctx->packet = nullptr;
    }
    if (_ptr->ctx->yuv_frame)
    {
        av_frame_free(&_ptr->ctx->yuv_frame);
        _ptr->ctx->yuv_frame = nullptr;
    }
    if (_ptr->ctx->rgb_frame)
    {
        av_frame_free(&_ptr->ctx->rgb_frame);
        _ptr->ctx->rgb_frame = nullptr;
    }
    if (_ptr->ctx->parser)
    {
        av_parser_close(_ptr->ctx->parser);
        _ptr->ctx->parser = nullptr;
    }
    if (_ptr->ctx->context)
    {
        avcodec_close(_ptr->ctx->context);
        avcodec_free_context(&_ptr->ctx->context);
        _ptr->ctx->context = nullptr;
    }

    free(_ptr);
    *p_ffmpeg = nullptr;
}