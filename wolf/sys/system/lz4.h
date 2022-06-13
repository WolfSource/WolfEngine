/*
        Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

W_EXPORT
int w_lz4_compress_bound(int p_input_size);

W_EXPORT
int w_lz4_compress_default(const char *p_src, char *p_dst, int p_src_size, int p_dst_size);

W_EXPORT
int w_lz4_compress_fast(const char *p_source, char *p_dst, int p_src_size, int p_dst_size, int p_acceleration);

W_EXPORT
int w_lz4_decompress_safe(const char *p_src, char *p_dst,  int p_src_size, int p_dst_size);

#ifdef __cplusplus
}
#endif