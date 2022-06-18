/*
        Project: Wolf Engine. Copyright � 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
#include "lz4.h"
#include <lz4.h>

int w_lz4_compress_bound(int p_input_size) {

  return LZ4_compressBound(p_input_size);
}

int w_lz4_compress_default(const char *p_src, char *p_dst, int p_src_size, int p_dst_size) {

  return LZ4_compress_default(p_src, p_dst, p_src_size, p_dst_size);
}

int w_lz4_compress_fast(const char *p_source, char *p_dst, int p_src_size, int p_dst_size, int p_acceleration) {

  return LZ4_compress_fast(p_source, p_dst, p_src_size, p_dst_size,
                           p_acceleration);
}

int w_lz4_decompress_safe(const char *p_src, char *p_dst,  int p_src_size, int p_dst_size) {
  return LZ4_decompress_safe(p_src, p_dst, p_src_size, p_dst_size);
}