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

typedef struct w_ffmpeg_t
{
    AVCodecID codec_id = AVCodecID::AV_CODEC_ID_NONE;
    AVCodecContext* context = nullptr;
    AVPacket* packet = nullptr;
    const AVCodec* codec = nullptr;
    AVCodecParserContext* parser = nullptr;
}w_ffmpeg_t W_ALIGNMENT_64;

//static int s_init_decoder(_Inout_ w_ffmpeg p_ffmpeg, _Inout_z_ char* p_error)
//{
//    constexpr auto TRACE = "ffmpeg::s_init_decoder";
//    int _ret = 0;
//
//    if (w_is_null(p_ffmpeg, p_error))
//    {
//        return _ret = -1;
//    }
//
//    p_ffmpeg->ctx->codec = avcodec_find_decoder(gsl::narrow_cast<AVCodecID>(p_ffmpeg->codec_id));
//    if (w_is_null(p_ffmpeg->ctx->codec))
//    {
//        s_make_error(p_error, TRACE, "failed to find the specified coded for decoder", 0);
//        return _ret = -1;
//    }
//
//    p_ffmpeg->ctx->context = avcodec_alloc_context3(p_ffmpeg->ctx->codec);
//    if (w_is_null(p_ffmpeg->ctx->context))
//    {
//        s_make_error(p_error, TRACE, "failed to allocate video codec context", 0);
//        return _ret = -1;
//    }
//
//    p_ffmpeg->ctx->rgb_frame = av_frame_alloc();
//    if (w_is_null(p_ffmpeg->ctx->rgb_frame))
//    {
//        s_make_error(p_error, TRACE, "failed to allocate the frame", 0);
//        return _ret = -1;
//    }
//
//    const auto _w = gsl::narrow_cast<int>(p_ffmpeg->width);
//    const auto _h = gsl::narrow_cast<int>(p_ffmpeg->height);
//    const auto _pixel_fmt = gsl::narrow_cast<AVPixelFormat>(p_ffmpeg->pix_fmt);
//    const auto _buffer_size = av_image_get_buffer_size(_pixel_fmt, _w, _h, 1);
//
//    auto* _frame_buffer = gsl::narrow_cast<const uint8_t*>(av_malloc(_buffer_size));
//    if (w_is_null(_frame_buffer))
//    {
//        s_make_error(p_error, TRACE, "failed to allocate the buffer for frame", 0);
//        return _ret = -1;
//    }
//
//    _ret = av_image_fill_arrays(
//        gsl::narrow_cast<uint8_t**>(p_ffmpeg->ctx->rgb_frame->data),
//        gsl::narrow_cast<int*>(p_ffmpeg->ctx->rgb_frame->linesize),
//        _frame_buffer,
//        _pixel_fmt,
//        _w,
//        _h,
//        1);
//    if (_ret <= 0)
//    {
//        s_make_error(p_error, TRACE, "could not fill image arrays", 0);
//        return _ret;
//    }
//
//    p_ffmpeg->ctx->parser = av_parser_init(p_ffmpeg->ctx->codec->id);
//
//    if (w_is_null(p_ffmpeg->ctx->parser))
//    {
//        s_make_error(p_error, TRACE, "could not initialize parser", 0);
//        return _ret = -1;
//    }
//
//    return avcodec_open2(p_ffmpeg->ctx->context, p_ffmpeg->ctx->codec, nullptr);
//}

int w_ffmpeg_init_encoder(
    _Inout_ w_ffmpeg* p_ffmpeg, 
    _In_ w_av_frame p_frame,
    _In_ uint32_t p_avcodec_id,
    _In_ uint32_t p_fps,
    _In_ uint64_t p_bitrate,
    _In_ w_av_opt_set_str* p_preset_strings,
    _In_ uint32_t p_preset_strings_size,
    _In_ w_av_opt_set_int* p_preset_ints,
    _In_ uint32_t p_preset_ints_size,
    _In_ w_av_opt_set_double* p_preset_doubles,
    _In_ uint32_t p_preset_doubles_size,
    _Inout_z_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_ffmpeg_init_encoder";

    auto _ret = 0;
    if (w_is_null(p_error))
    {
        return _ret = -1;
    }
    
    auto* _ffmpeg = gsl::owner<w_ffmpeg_t*>(calloc(1, sizeof(w_ffmpeg_t)));
    if (w_is_null(_ffmpeg))
    {
        return _ret = -1;
    }

    defer _(nullptr, [&](...)
        {
            *p_ffmpeg = _ffmpeg;
            if (_ret != 0)
            {
                w_ffmpeg_fini(p_ffmpeg);
            }
        });

    _ffmpeg->packet = av_packet_alloc();
    if (w_is_null(_ffmpeg->packet))
    {
        (void)snprintf(
            p_error,
            W_MAX_PATH,
            "could nit allocate memory for ffmpeg packet. trace info: %s",
            TRACE);
        return _ret = -1;
    }

    _ffmpeg->codec_id = gsl::narrow_cast<AVCodecID>(p_avcodec_id);
    _ffmpeg->codec = avcodec_find_encoder(_ffmpeg->codec_id);
    if (w_is_null(_ffmpeg->codec))
    {
        (void)snprintf(
            p_error,
            W_MAX_PATH,
            "failed to find the specified coded for encoder. trace info: %s",
            TRACE);
        return _ret = -1;
    }

    _ffmpeg->context = avcodec_alloc_context3(_ffmpeg->codec);
    if (w_is_null(_ffmpeg->context))
    {
        (void)snprintf(
            p_error,
            W_MAX_PATH,
            "failed to allocate memory for ffmpeg context. trace info: %s",
            TRACE);
        return _ret = -1;
    }

    _ffmpeg->context->bit_rate = gsl::narrow_cast<int64_t>(p_bitrate);
    _ffmpeg->context->time_base.num = 1;
    _ffmpeg->context->time_base.den = gsl::narrow_cast<int>(p_fps);
    _ffmpeg->context->pix_fmt = gsl::narrow_cast<AVPixelFormat>(p_frame->format);
    _ffmpeg->context->width = p_frame->width;
    _ffmpeg->context->height = p_frame->height;

#ifdef __clang__
#pragma unroll
#endif
    for (uint32_t i = 0; i < p_preset_strings_size; ++i)
    {
        auto* const _name = p_preset_strings[i].name;
        auto* const _value = p_preset_strings[i].value;

        _ret = av_opt_set(_ffmpeg->context->priv_data, _name, _value, 0);
        if (_ret < 0)
        {
            (void)snprintf(
                p_error,
                W_MAX_PATH,
                "could not set %s : %s. trace info: %s",
                _name,
                _value,
                TRACE);
            return _ret = -1;
        }
    }

#ifdef __clang__
#pragma unroll
#endif
    for (uint32_t i = 0; i < p_preset_ints_size; ++i)
    {
        auto* const _name = p_preset_ints[i].name;
        const auto _value = p_preset_ints[i].value;

        _ret = av_opt_set_int(_ffmpeg->context->priv_data, _name, _value, 0);
        if (_ret < 0)
        {
            (void)snprintf(
                p_error,
                W_MAX_PATH,
                "could not set %s : %d. trace info: %s",
                _name,
                _value,
                TRACE);
            return _ret = -1;
        }
    }

#ifdef __clang__
#pragma unroll
#endif
    for (uint32_t i = 0; i < p_preset_doubles_size; ++i)
    {
        auto* const _name = p_preset_doubles[i].name;
        const auto _value = p_preset_doubles[i].value;

        _ret = av_opt_set_double(_ffmpeg->context->priv_data, _name, _value, 0);
        if (_ret < 0)
        {
            (void)snprintf(
                p_error,
                W_MAX_PATH,
                "could not set %s : %f. trace info: %s",
                _name,
                _value,
                TRACE);
            return _ret = -1;
        }
    }

    _ret = avcodec_open2(_ffmpeg->context, _ffmpeg->codec, nullptr);
    if (_ret < 0)
    {
        (void)snprintf(
            p_error,
            W_MAX_PATH,
            "could not open avcodec for codec id: %d. trace info: %s",
            _ffmpeg->codec_id,
            TRACE);
        return _ret = -1;
    }

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
    if (w_is_null(p_ffmpeg) || w_is_null(*p_ffmpeg))
    {
        return;
    }

    auto _ptr = *p_ffmpeg;
    if (_ptr)
    {
        if (_ptr->packet)
        {
            av_free(_ptr->packet);
        }
        if (_ptr->parser)
        {
            av_parser_close(_ptr->parser);
        }
        if (_ptr->context)
        {
            avcodec_close(_ptr->context);
            avcodec_free_context(&_ptr->context);
        }
    }

    free(*p_ffmpeg);
    *p_ffmpeg = nullptr;
}