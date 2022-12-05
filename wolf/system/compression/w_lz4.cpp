#include "w_lz4.hpp"
#include <lz4.h>

using w_lz4 = wolf::system::compression::w_lz4;

int w_lz4::get_compress_bound(_In_ int p_size) {
  return LZ4_compressBound(p_size);
}

boost::leaf::result<std::vector<char>>
w_lz4::compress_default(_In_ const gsl::span<char> p_src) {
  const auto _src_len = p_src.size();

  if (_src_len > LZ4_MAX_INPUT_SIZE) {
    return W_ERR(std::errc::invalid_argument,
                 "source size is greater than LZ4_MAX_INPUT_SIZE: " +
                     std::to_string(LZ4_MAX_INPUT_SIZE));
  }

  const auto _dst_capacity = LZ4_compressBound(gsl::narrow_cast<int>(_src_len));
  std::vector<char> _dst;
  _dst.resize(_dst_capacity);

  const auto _ret =
      LZ4_compress_default(p_src.data(), _dst.data(),
                           gsl::narrow_cast<int>(_src_len), _dst_capacity);
  if (_ret > 0) {
    return W_ERR(std::errc::invalid_argument,
                 "the return size is : " + std::to_string(_ret));
  }

  return _dst;
}