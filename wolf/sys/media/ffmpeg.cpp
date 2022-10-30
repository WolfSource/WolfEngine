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
    const AVCodec* codec = nullptr;
    AVCodecParserContext* parser = nullptr;
}w_ffmpeg_t W_ALIGNMENT_64;

int w_ffmpeg_init(
    _Inout_ w_ffmpeg* p_ffmpeg,
    _In_ w_av_frame p_frame,
    _In_ uint32_t p_mode,
    _In_ const char* p_avcodec_id,
    _In_ AVCodeOptions* p_codec_opt,
    _In_ const AVSetOption* p_av_opt_sets,
    _In_ uint32_t p_av_opt_sets_size,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_ffmpeg_init";
    const auto _error_nn = gsl::not_null<char*>(p_error);
    auto _ret = 0;

    auto _ptr = (w_ffmpeg_t*)calloc(1, sizeof(w_ffmpeg_t));
    const auto _codec_opt = gsl::not_null<AVCodeOptions*>(p_codec_opt);
    const auto _ffmpeg = gsl::not_null<w_ffmpeg_t*>(_ptr);
    const auto _codec_id_nn = gsl::not_null<const char*>(p_avcodec_id);

    defer _(nullptr, [&](...)
        {
            *p_ffmpeg = _ffmpeg.get();
            if (_ret != 0)
            {
                w_ffmpeg_fini(p_ffmpeg);
            }
        });

    if (p_mode == 0)
    {
        _ffmpeg->codec = avcodec_find_encoder_by_name(_codec_id_nn);
        if (_ffmpeg->codec == nullptr)
        {
            std::ignore = snprintf(
                p_error,
                W_MAX_PATH,
                "could not find encoder codec id %s. trace info: %s",
                _codec_id_nn.get(),
                TRACE);
            return _ret = -1;
        }
    }
    else
    {
        _ffmpeg->codec = avcodec_find_decoder_by_name(_codec_id_nn);
        if (_ffmpeg->codec == nullptr)
        {
            std::ignore = snprintf(
                p_error,
                W_MAX_PATH,
                "could not find encoder codec id %s. trace info: %s",
                _codec_id_nn.get(),
                TRACE);
            return _ret = -1;
        }
        _ffmpeg->parser = av_parser_init(_ffmpeg->codec->id);
    }

    _ffmpeg->context = gsl::not_null<AVCodecContext*>(avcodec_alloc_context3(_ffmpeg->codec));

    // note resolution must be a multiple of 2!!
    _ffmpeg->context->width = p_frame->width;
    // note resolution must be a multiple of 2!!
    _ffmpeg->context->height = p_frame->height;
    // bitrate of context
    _ffmpeg->context->bit_rate = gsl::narrow_cast<int64_t>(_codec_opt->bitrate);
    // time based number
    _ffmpeg->context->time_base = AVRational{ 1, gsl::narrow_cast<int>(_codec_opt->fps) };
    // frame rate
    _ffmpeg->context->framerate = AVRational{ gsl::narrow_cast<int>(_codec_opt->fps) , 1 };
    // pixel format
    _ffmpeg->context->pix_fmt = gsl::narrow_cast<AVPixelFormat>(p_frame->format);
    // set gop
    if (_codec_opt->gop >= 0)
    {
        _ffmpeg->context->gop_size = gsl::narrow_cast<int>(_codec_opt->gop);
    }
    // set refs
    if (_codec_opt->refs >= 0)
    {
        _ffmpeg->context->refs = gsl::narrow_cast<int>(_codec_opt->refs);
    }
    // set frames
    if (_codec_opt->max_b_frames >= 0)
    {
        _ffmpeg->context->max_b_frames = gsl::narrow_cast<int>(_codec_opt->max_b_frames);
    }
    // set thread numbers
    if (_codec_opt->thread_count >= 0)
    {
        _ffmpeg->context->thread_count = gsl::narrow_cast<int>(_codec_opt->thread_count);
    }
    // set level
    if (_codec_opt->level >= 0)
    {
        _ffmpeg->context->level = _codec_opt->level;
    }

    if (_ffmpeg->context->flags & AVFMT_GLOBALHEADER)
    {
        _ffmpeg->context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

#ifdef __clang__
#pragma unroll
#endif
    for (uint32_t i = 0; i < p_av_opt_sets_size; ++i)
    {
        auto* const _name = p_av_opt_sets[i].name;

        if (_name == nullptr)
        {
            continue;
        }

        switch (p_av_opt_sets[i].value_type)
        {
        default:
            break;
        case AVSetOptionValueType::STRING:
        {
            const auto _value = p_av_opt_sets[i].value_str;
            if (_value != nullptr)
            {
                _ret = av_opt_set(_ffmpeg->context->priv_data, _name, _value, 0);
                if (_ret < 0)
                {
                    std::array<char, W_MAX_PATH> _av_error = { 0 };
                    std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);

                    std::ignore = snprintf(
                        p_error,
                        W_MAX_PATH,
                        "could not set %s : %s because: %s. trace info: %s",
                        _name,
                        _value,
                        _av_error.data(),
                        TRACE);
                    return _ret = -1;
                }
            }
            break;
        }
        case AVSetOptionValueType::INTEGER:
        {
            const auto _value = p_av_opt_sets[i].value_int;
            _ret = av_opt_set_int(_ffmpeg->context->priv_data, _name, _value, 0);
            if (_ret < 0)
            {
                std::array<char, W_MAX_PATH> _av_error = { 0 };
                std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);

                std::ignore = snprintf(
                    p_error,
                    W_MAX_PATH,
                    "could not set %s : %d because: %s. trace info: %s",
                    _name,
                    _value,
                    _av_error.data(),
                    TRACE);
                return _ret = -1;
            }
            break;
        }
        case AVSetOptionValueType::DOUBLE:
        {
            const auto _value = p_av_opt_sets[i].value_double;
            _ret = av_opt_set_double(_ffmpeg->context->priv_data, _name, _value, 0);
            if (_ret < 0)
            {
                std::array<char, W_MAX_PATH> _av_error = { 0 };
                std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);

                std::ignore = snprintf(
                    p_error,
                    W_MAX_PATH,
                    "could not set %s : %f because: %s. trace info: %s",
                    _name,
                    _value,
                    _av_error.data(),
                    TRACE);
                return _ret = -1;
            }
            break;
        }
        }
    }

    _ret = avcodec_open2(_ffmpeg->context, _ffmpeg->codec, nullptr);
    if (_ret < 0)
    {
        std::array<char, W_MAX_PATH> _av_error = { 0 };
        std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);

        std::ignore = snprintf(
            p_error,
            W_MAX_PATH,
            "could not open avcodec for codec id: %d because: %s. trace info: %s",
            _ffmpeg->codec_id,
            _av_error.data(),
            TRACE);
        return _ret = -1;
    }

    return _ret;
}

int w_ffmpeg_encode(
    _In_ w_ffmpeg p_ffmpeg,
    _In_ const w_av_frame p_av_frame,
    _Inout_ w_av_packet* p_packet,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_ffmpeg_encode";
    const auto _ffmpeg_nn = gsl::not_null<w_ffmpeg>(p_ffmpeg);
    const auto _packet_ptr_nn = gsl::not_null<w_av_packet*>(p_packet);
    const auto _packet_nn = gsl::not_null<w_av_packet>(*_packet_ptr_nn);
    const auto _error = gsl::not_null<char*>(p_error);

    av_packet_unref(_packet_nn);
    _packet_nn->data = nullptr;
    _packet_nn->size = 0;

    auto _ret = avcodec_send_frame(_ffmpeg_nn->context, p_av_frame);
    if (_ret < 0)
    {
        std::array<char, W_MAX_PATH> _av_error = { 0 };
        std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);

        std::ignore = snprintf(
            _error,
            W_MAX_PATH,
            "failed to send the frame for encoding because: %s. trace info: %s",
            _av_error.data(),
            TRACE);
        return W_FAILURE;
    }

    _ret = avcodec_receive_packet(p_ffmpeg->context, _packet_nn);
    if (_ret == AVERROR(EAGAIN) || _ret == AVERROR_EOF)
    {
        return W_SUCCESS;
    }
    
    if (_ret < 0)
    {
        std::array<char, W_MAX_PATH> _av_error = { 0 };
        std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);

        std::ignore = snprintf(
            _error,
            W_MAX_PATH,
            "error happened during the encoding because: %s. trace info: %s",
            _av_error.data(),
            TRACE);
        return _ret;
    }

    return _packet_nn->size;
}

int s_ffmpeg_packet_to_frame(
    _In_ const gsl::not_null<w_ffmpeg>& p_ffmpeg, 
    _In_ AVPacket* p_packet,
    _Inout_ gsl::not_null<w_av_frame> p_av_frame,
    _Inout_ const gsl::not_null<char*>& p_error)
{
    constexpr auto TRACE = "ffmpeg::s_ffmpeg_packet_to_frame";
    
    // start decoding
    auto _ret = avcodec_send_packet(p_ffmpeg->context, p_packet);
    if (_ret < 0)
    {
        std::array<char, W_MAX_PATH> _av_error = { 0 };
        std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);

        std::ignore = snprintf(
            p_error,
            W_MAX_PATH,
            "could not parse packet for decoding because: %s. trace info: %s",
            _av_error.data(),
            TRACE);
        return _ret;
    }

    for (;;)
    {
        _ret = avcodec_receive_frame(p_ffmpeg->context, p_av_frame);
        if (_ret == 0 || _ret == AVERROR(EAGAIN) || _ret == AVERROR_EOF)
        {
            break;
        }

        if (_ret < 0)
        {
            std::array<char, W_MAX_PATH> _av_error = { 0 };
            std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);

            std::ignore = snprintf(
                p_error,
                W_MAX_PATH,
                "error happened during the encoding because: %s. trace info: %s",
                _av_error.data(),
                TRACE);
            break;
        }
    }
    return _ret;
}

int w_ffmpeg_decode(
    _In_ w_ffmpeg p_ffmpeg,
    _In_ const w_av_packet p_packet,
    _Inout_ w_av_frame* p_av_frame,
    _Inout_ char* p_error)
{
    constexpr auto TRACE = "ffmpeg::w_ffmpeg_decode";
    const auto _ffmpeg_nn = gsl::not_null<w_ffmpeg>(p_ffmpeg);

    const auto _src_packet_nn = gsl::not_null<const w_av_packet>(p_packet);
    auto _av_frame_ptr_nn = gsl::not_null<w_av_frame*>(p_av_frame);
    auto _av_frame_nn = gsl::not_null<w_av_frame>(*_av_frame_ptr_nn);
    const auto _error = gsl::not_null<char*>(p_error);

    int _ret = 0;

    auto _dst_packet = av_packet_alloc();
    if (_dst_packet == nullptr)
    {
        std::ignore = snprintf(
            _error,
            W_MAX_PATH,
            "could not allocate packet for decoding. trace info: %s",
            TRACE);
        return _ret = -1;
    }

#ifdef __clang__
#pragma unroll
#endif
    for (;;)
    {
        const auto _num = av_parser_parse2(
            _ffmpeg_nn->parser,
            _ffmpeg_nn->context,
            &_dst_packet->data,
            &_dst_packet->size,
            _src_packet_nn->data,
            _src_packet_nn->size,
            AV_NOPTS_VALUE,
            AV_NOPTS_VALUE,
            0);
        if (_num < 0)
        {
            std::array<char, W_MAX_PATH> _av_error = { 0 };
            std::ignore = av_make_error_string(_av_error.data(), _av_error.size(), _ret);

            std::ignore = snprintf(
                _error,
                W_MAX_PATH,
                "could not parse packet for decoding because: %s. trace info: %s",
                _av_error.data(),
                TRACE);
            _ret = -1;
            break;
        }

        _src_packet_nn->data += _num;
        _src_packet_nn->size -= _num;
        
        if (_dst_packet->size == 0)
        {
            break;
        }

        if (_dst_packet->size > 0)
        {
            _ret = s_ffmpeg_packet_to_frame(_ffmpeg_nn, _dst_packet, _av_frame_nn, _error);
            if (_ret != 0)
            {
                break;
            }
        }
    }

    av_packet_free(&_dst_packet);

    return _ret;
}

void w_ffmpeg_fini(_Inout_ w_ffmpeg* p_ffmpeg)
{
    const auto _ffmpeg_ptr_nn = gsl::not_null<w_ffmpeg*>(p_ffmpeg);
    const auto _ffmpeg_nn = gsl::not_null<w_ffmpeg>(*_ffmpeg_ptr_nn);

    if (_ffmpeg_nn->parser)
    {
        av_parser_close(_ffmpeg_nn->parser);
    }
    if (_ffmpeg_nn->context)
    {
        avcodec_close(_ffmpeg_nn->context);
        avcodec_free_context(&_ffmpeg_nn->context);
    }

    free(_ffmpeg_nn);
    *p_ffmpeg = nullptr;
}