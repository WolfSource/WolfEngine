#include "lz4.h"
#include <lz4.h>

int w_lz4_compress_bound(_In_ int p_size)
{
    return LZ4_compressBound(p_size);
}

int w_lz4_compress_default(
    _In_ int p_src_len,
    _In_z_ const char* p_src,
    _In_ int p_dst_len,
    _Inout_z_ char* p_dst)
{
    return LZ4_compress_default(p_src, p_dst, p_src_len, p_dst_len);
}

int w_lz4_compress_fast(
    _In_ int p_acceleration,
    _In_ int p_src_len,
    _In_z_ const char* p_src,
    _In_ int p_dst_len,
    _Inout_z_ char* p_dst)
{
    return LZ4_compress_fast(
        p_src,
        p_dst,
        p_src_len,
        p_dst_len,
        p_acceleration);
}

int w_lz4_decompress_safe(
    _In_ int p_src_len,
    _In_z_ const char* p_src,
    _In_ int p_dst_len,
    _Inout_z_ char* p_dst)
{
    return LZ4_decompress_safe(p_src, p_dst, p_src_len, p_dst_len);
}