/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_LZMA

#include <wolf.hpp>

namespace wolf::system::compression {

struct w_lzma {

  /*
   * compress a stream via lzma1 algorithm
   * @param p_src, the input source
   * @param p_level, the level of compression
   * @returns a vector of compressed stream
   */
  W_API static boost::leaf::result<std::vector<std::byte>>
  compress_lzma1(_In_ const gsl::span<const std::byte> p_src,
                 _In_ uint32_t p_level);

  /*
   * compress a stream via lzma2 algorithm
   * @param p_src, the input source
   * @param p_level, the level of compression
   * @returns a vector of compressed stream
   */
  W_API static boost::leaf::result<std::vector<std::byte>>
  compress_lzma2(_In_ const gsl::span<const std::byte> p_src,
                 _In_ uint32_t p_level);

  /*
   * decompress a stream via lzma1 algorithm
   * @param p_src, the input source
   * @returns a vector of decompressed stream
   */
  W_API static boost::leaf::result<std::vector<std::byte>>
  decompress_lzma1(_In_ const gsl::span<const std::byte> p_src);

  /*
   * decompress a stream via lzma2 algorithm
   * @param p_src, the input source
   * @returns a vector of decompressed stream
   */
  W_API static boost::leaf::result<std::vector<std::byte>>
  decompress_lzma2(_In_ gsl::span<const std::byte> p_src);
};
} // namespace wolf::system::compression

#endif // WOLF_SYSTEM_LZMA