/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_LZ4

#include <wolf.hpp>

namespace wolf::system::compression {

struct w_lz4 {

  /*
   * get the size of compress bound
   * @param p_size, the input size
   * @returns the size of bound
   */
  W_API [[maybe_unused]] static int
  get_compress_bound(_In_ int p_size) noexcept;

  /*
   * compress using the default mode of lz4
   * @param p_src, the input source
   * @returns the vector of compressed stream
   */
  W_API static boost::leaf::result<std::vector<std::byte>>
  compress_default(_In_ const gsl::span<const std::byte> p_src) noexcept;

  /*
   * compress using the fast mode of lz4
   * @param p_src, the input source
   * @param p_acceleration, a value between 1 - 65536
   * @returns the vector of compressed stream
   */
  W_API static boost::leaf::result<std::vector<std::byte>>
  compress_fast(_In_ const gsl::span<const std::byte> p_src,
                _In_ const int p_acceleration) noexcept;

  /*
   * decompress the compressed stream
   * @param p_src, the input source
   * @returns the vector of decompressed stream
   */
  W_API static boost::leaf::result<std::vector<std::byte>>
  decompress(_In_ const gsl::span<const std::byte> p_src,
             _In_ const size_t p_max_retry) noexcept;
};
} // namespace wolf::system::compression

#endif // WOLF_SYSTEM_LZ4