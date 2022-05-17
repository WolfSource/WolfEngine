#include "lz4.h"
#include <lz4.h>
#include "common.hpp"

auto w_lz4_compress(
    w_buf p_src,
    w_buf p_dst,
    bool p_is_fast_mode,
    int p_acceleration,
    w_buf p_trace) -> int
{
    if (!is_valid_buffer(p_src) || !is_valid_buffer(p_trace))
    {
        return 1;
    }

    auto *trace_buf = reinterpret_cast<char *>(p_trace->data);
    auto trace_buf_len = p_trace->len;

    // allocate size for compressed data
    auto size = gsl::narrow_cast<int>(p_src->len);
    if (size <= 0)
    {
        return 1;
    }

    // allocate size for compressed data
    const auto _max_dst_size = LZ4_compressBound(size);
    auto *data = gsl::narrow_cast<char *>(mi_malloc(_max_dst_size));
    if (data == nullptr)
    {
        (void)snprintf(trace_buf,
                       trace_buf_len,
                       "could not allocate memory for compressed buffer");
        return -1;
    }

    auto len = -1;
    const auto *src_data = reinterpret_cast<char *>(p_src->data);
    const auto src_len = gsl::narrow_cast<int>(p_src->len);
    if (p_is_fast_mode)
    {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif
        len =
            LZ4_compress_fast(
                src_data,
                data,
                src_len,
                _max_dst_size,
                p_acceleration);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
    }
    else
    {
        len = LZ4_compress_default(
            src_data,
            data,
            src_len,
            _max_dst_size);
    }

    if (len <= 0)
    {
        (void)snprintf(trace_buf,
                       trace_buf_len,
                       "could not compress");
        return -1;
    }

    // realloc compress_data to free up memory
    data = gsl::narrow_cast<char *>(mi_realloc(data, len));
    if (data == nullptr)
    {
        (void)snprintf(trace_buf,
                       trace_buf_len,
                       "could not realloc compressed memory");
        return -1;
    }

    p_dst->data = reinterpret_cast<uint8_t *>(data);
    p_dst->len = len;

    return 0;
}

auto w_lz4_decompress(
    w_buf p_src,
    w_buf p_dst,
    w_buf p_trace) -> int
{
    if (!is_valid_buffer(p_src) || !is_valid_buffer(p_trace))
    {
        return 1;
    }

    auto *trace_buf = reinterpret_cast<char *>(p_trace->data);
    auto trace_buf_len = p_trace->len;

    // allocate memory for decompress
    auto _destination_len = gsl::narrow_cast<int>(p_src->len * 2);
    auto *data = gsl::narrow_cast<char *>(mi_malloc(_destination_len));
    if (data == nullptr)
    {
        (void)snprintf(trace_buf,
                       trace_buf_len,
                       "could not allocate memory for decompressed buffer");
        return -1;
    }

    auto _decompressed_len = -1;
    const auto _max_num_try = 10;
    const auto *src_data = reinterpret_cast<const char *>(p_src->data);
    const auto src_len = gsl::narrow_cast<int>(p_src->len);

// try for decompressing
#pragma unroll
    for (auto i = 0; i < _max_num_try; ++i)
    {
        _decompressed_len = LZ4_decompress_safe(
            src_data,
            data,
            src_len,
            _destination_len);
        if (_decompressed_len > 0)
        {
            break;
        }

        // we need to reallocate a larget buffer for decompressing
        _destination_len *= 2;
        data = gsl::narrow_cast<char *>(mi_realloc(data, _destination_len));
        if (data == nullptr)
        {
            (void)snprintf(trace_buf,
                           trace_buf_len,
                           "could not re-allocate memory of decompressed buffer");
            break;
        }
    }

    if (_decompressed_len <= 0)
    {
        (void)snprintf(trace_buf,
                       trace_buf_len,
                       "size of decompress buffer must be greater than zero");
        mi_free(data);
        return -1;
    }

    data = gsl::narrow_cast<char *>(mi_realloc(data, _decompressed_len));
    if (data == nullptr)
    {
        (void)snprintf(trace_buf,
                       trace_buf_len,
                       "could not reallocate memory for decompressed buffer");
        mi_free(data);
        return -1;
    }

    p_dst->data = reinterpret_cast<uint8_t *>(data);
    p_dst->len = _decompressed_len;

    return 0;
}

auto w_lz4_free_buf(w_buf p_buf) -> int
{
    if (p_buf == nullptr || p_buf->data == nullptr)
    {
        return 1;
    }

    mi_free(p_buf->data);
    return 0;
}