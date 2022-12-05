/*
    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include <wolf.hpp>

namespace wolf::system::compression {

struct w_lz4 {

  /*
   * get the size of compress bound
   * @param p_size, the input size
   * @returns the size of bound
   */
  W_API static int get_compress_bound(_In_ int p_size);

  /*
   * compress using the default mode of lz4
   * @param p_src, the input source
   * @returns vector of compressed stream
   */
  W_API static boost::leaf::result<std::vector<char>>
  compress_default(_In_ const gsl::span<char> p_src);
};
} // namespace wolf::system::compression