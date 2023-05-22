#include "w_lz4.hpp"

#ifdef WOLF_SYSTEM_LZ4

#include <DISABLE_ANALYSIS_BEGIN>
#include <lz4.h>
#include <DISABLE_ANALYSIS_END>

using w_lz4 = wolf::system::compression::w_lz4;

static boost::leaf::result<int>
s_check_input_len(_In_ const size_t p_src_size) noexcept {
  if (p_src_size < LZ4_MAX_INPUT_SIZE) {
    return 0;
  }

  std::string _src_size_msg;
  try {
    _src_size_msg = std::to_string(LZ4_MAX_INPUT_SIZE);
  } catch (...) {
  }
  return W_FAILURE(std::errc::invalid_argument,
                   "source size is greater than LZ4_MAX_INPUT_SIZE: " +
                       _src_size_msg);
}

static std::vector<std::byte>
s_shrink_to_fit(_In_ const std::vector<std::byte> &p_src, _In_ const size_t p_shrink_size) noexcept {
  std::vector<std::byte> _dst;
  _dst.resize(p_shrink_size);

  std::copy(p_src.begin(), p_src.begin() + gsl::narrow_cast<int>(p_shrink_size), _dst.begin());
  return _dst;
}

int w_lz4::get_compress_bound(_In_ int p_size) noexcept {
  return LZ4_compressBound(p_size);
}

boost::leaf::result<std::vector<std::byte>>
w_lz4::compress_default(_In_ const gsl::span<const std::byte> p_src) noexcept {
  const auto _src_size = p_src.size();
  if (_src_size == 0) {
    return W_FAILURE(std::errc::invalid_argument, "the source is empty");
  }

  BOOST_LEAF_CHECK(s_check_input_len(_src_size));

  const auto _dst_capacity = LZ4_compressBound(gsl::narrow_cast<int>(_src_size));
  std::vector<std::byte> _tmp;
  _tmp.resize(_dst_capacity);

  const auto _bytes =
      LZ4_compress_default(reinterpret_cast<const char *>(p_src.data()), reinterpret_cast<char *>(_tmp.data()),
                           gsl::narrow_cast<int>(_src_size), _dst_capacity);
  if (_bytes > 0) {
    return s_shrink_to_fit(_tmp, _bytes);
  }

  return W_FAILURE(std::errc::operation_canceled,
                   "lz4 compress default failed");
}

boost::leaf::result<std::vector<std::byte>>
w_lz4::compress_fast(_In_ const gsl::span<const std::byte> p_src,
                     _In_ const int p_acceleration) noexcept {
  const auto _src_size = p_src.size();
  if (_src_size == 0) {
    return W_FAILURE(std::errc::invalid_argument, "the source is empty");
  }

  BOOST_LEAF_CHECK(s_check_input_len(_src_size));

  const auto _dst_capacity = LZ4_compressBound(gsl::narrow_cast<int>(_src_size));
  std::vector<std::byte> _tmp;
  _tmp.resize(_dst_capacity);

  const auto _bytes = LZ4_compress_fast(
      reinterpret_cast<const char *>(p_src.data()), reinterpret_cast<char *>(_tmp.data()),
      gsl::narrow_cast<int>(_src_size), _dst_capacity, p_acceleration);
  if (_bytes > 0) {
    return s_shrink_to_fit(_tmp, _bytes);
  }

  return W_FAILURE(std::errc::operation_canceled, "lz4 compress fast failed");
}

boost::leaf::result<std::vector<std::byte>>
w_lz4::decompress(_In_ const gsl::span<const std::byte> p_src,
                  _In_ const size_t p_max_retry) noexcept {
  const auto _src_size = p_src.size();
  if (_src_size == 0) {
    return W_FAILURE(std::errc::invalid_argument, "the source is empty");
  }

  // we will increase our size per each step
  std::vector<std::byte> _tmp;
  auto _resize = _src_size * 2;

#ifdef __clang
#pragma unroll
#endif
  for (auto i = 0; i < p_max_retry; ++i) {
    // resize it for next round
    _tmp.resize(_resize);

    const auto _bytes = LZ4_decompress_safe(
        reinterpret_cast<const char *>(p_src.data()), reinterpret_cast<char *>(_tmp.data()),
        gsl::narrow_cast<int>(_src_size), gsl::narrow_cast<int>(_tmp.size()));
    if (_bytes > 0) {
      return s_shrink_to_fit(_tmp, _bytes);
    }

    _resize *= 2;
  }

  std::string _max_retry_str;
  try {
    _max_retry_str = std::to_string(p_max_retry);
  } catch (...) {
  }
  return W_FAILURE(std::errc::operation_canceled,
                   "could not decompress lz4 stream after " + _max_retry_str);
}

#endif // WOLF_SYSTEM_LZ4
