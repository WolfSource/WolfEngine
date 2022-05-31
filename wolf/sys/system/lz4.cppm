/*
        Project: Wolf Engine. Copyright � 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
module;

#include <gsl/gsl>
#include <lz4.h>
#include <wolf.hpp>

export module wolf.system.lz4;

namespace wolf::system::lz4 {

W_EXPORT
auto w_lz4_compress_bound(int p_input_size) -> int {

  return LZ4_compressBound(p_input_size);
}

W_EXPORT
auto w_lz4_compress_default(char *p_dst, int p_dst_size, const char *p_src,
                            int p_src_size) -> int {

  return LZ4_compress_default(p_src, p_dst, p_src_size, p_dst_size);
}

W_EXPORT
auto w_lz4_compress_fast(char *p_dst, int p_dst_size, const char *p_source,
                         int p_src_size, int p_acceleration) -> int {

  return LZ4_compress_fast(p_source, p_dst, p_src_size, p_dst_size,
                           p_acceleration);
}

W_EXPORT
auto w_lz4_decompress_safe(char *p_dst, int p_dst_size, const char *p_src,
                           int p_src_size) -> int {
  return LZ4_decompress_safe(p_src, p_dst, p_src_size, p_dst_size);
}
} // namespace wolf::system::lz4