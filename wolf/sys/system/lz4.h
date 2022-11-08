/*
        Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    W_API
        int w_lz4_compress_bound(_In_ int p_size);

    W_API
        int w_lz4_compress_default(
            _In_ int p_src_len,
            _In_z_ const char* p_src,
            _In_ int p_dst_len,
            _Inout_z_ char* p_dst);

    W_API
        int w_lz4_compress_fast(
            _In_ int p_acceleration,
            _In_ int p_src_len,
            _In_z_ const char* p_src,
            _In_ int p_dst_len,
            _Inout_z_ char* p_dst);

    W_API
        int w_lz4_decompress_safe(
            _In_ int p_src_len,
            _In_z_ const char* p_src,
            _In_ int p_dst_len,
            _Inout_z_ char* p_dst);

#ifdef __cplusplus
}
#endif