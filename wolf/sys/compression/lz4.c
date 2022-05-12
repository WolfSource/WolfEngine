#include "lz4.h"
#include <lz4.h>
#include <mimalloc.h>

int w_lz4_compress(
    w_buf p_src,
    w_buf p_dst,
    int p_fast_mode,
    int p_acceleration,
    w_buf p_trace)
{
    if (is_buf_valid(p_src) != 0 || is_buf_valid(p_trace) != 0)
    {
        return 1;
    }

    // allocate size for compressed data
    const int _max_dst_size = LZ4_compressBound((int)(p_src->len));
    char *data = (char *)mi_malloc(_max_dst_size);
    if (data == NULL)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not allocate memory for compressed buffer");
        return -1;
    }

    int len = -1;
    if (p_fast_mode != 0)
    {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif
        len =
            LZ4_compress_fast(
                (const char *)p_src->data,
                data,
                (int)p_src->len,
                _max_dst_size,
                p_acceleration);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
    }
    else
    {
        len = LZ4_compress_default(
            (const char *)p_src->data,
            data,
            (int)p_src->len,
            _max_dst_size);
    }

    if (len <= 0)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not compress");
        return -1;
    }

    // realloc compress_data to free up memory
    data = (char *)mi_realloc(data, len);
    if (data == NULL)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not realloc compressed memory");
        return -1;
    }

    p_dst->data = (uint8_t *)data;
    p_dst->len = len;

    return 0;
}

int w_lz4_decompress(
    w_buf p_src,
    w_buf p_dst,
    w_buf p_trace)
{
    if (is_buf_valid(p_src) != 0 || is_buf_valid(p_trace) != 0)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "missing p_src or p_trace data");
        return 1;
    }

    // allocate memory for decompress
    int _destination_len = (int)p_src->len * 2;
    char *data = (char *)mi_malloc(_destination_len);
    if (data == NULL)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not allocate memory for decompressed buffer");
        return -1;
    }

    int _decompressed_len = -1;
    const int _max_num_try = 10;

// try for decompressing
#pragma unroll
    for (int i = 0; i < _max_num_try; ++i)
    {
        _decompressed_len = LZ4_decompress_safe(
            (const char *)p_src->data,
            data,
            (int)p_src->len,
            _destination_len);
        if (_decompressed_len > 0)
        {
            break;
        }

        // we need to reallocate a larget buffer for decompressing
        _destination_len *= 2;
        data = (char *)mi_realloc(data, _destination_len);
        if (!data)
        {
            (void)snprintf((char *)p_trace->data,
                           p_trace->len,
                           "could not re-allocate memory of decompressed buffer");
            break;
        }
    }

    if (_decompressed_len <= 0)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "size of decompress buffer must be greater than zero");
        mi_free(data);
        return -1;
    }

    data = (char *)mi_realloc(data, _decompressed_len);
    if (data == NULL)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not reallocate memory for decompressed buffer");
        mi_free(data);
        return -1;
    }

    p_dst->data = (uint8_t *)data;
    p_dst->len = _decompressed_len;

    return 0;
}

int w_lz4_free_buf(w_buf p_buf)
{
    if (p_buf == NULL ||
        p_buf->data == NULL)
    {
        return 1;
    }

    mi_free(p_buf->data);

    return 0;
}