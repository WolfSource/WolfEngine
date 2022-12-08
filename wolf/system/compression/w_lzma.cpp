#include "w_lzma.hpp"

#ifdef WOLF_SYSTEM_LZMA

#include <DISABLE_ANALYSIS_BEGIN>
#include <Lzma2Enc.h>
#include <Lzma2Dec.h>
#include <LzmaDec.h>
#include <LzmaEnc.h>
#include <DISABLE_ANALYSIS_END>

using w_lzma = wolf::system::compression::w_lzma;

constexpr auto LZMA_HEADER_SRC_SIZE = 8;
constexpr auto MAX_HEADER_SIZE = 256 * 1024 * 1024;

static void *s_lzma_alloc(ISzAllocPtr p_ptr, size_t p_size) noexcept {
  std::ignore = p_ptr;
  return malloc(p_size);
}
static void s_lzma_free(ISzAllocPtr p_ptr, void *p_addr) noexcept {
  std::ignore = p_ptr;
  free(p_addr);
}

constexpr ISzAlloc s_alloc_funcs = {s_lzma_alloc, s_lzma_free};

static void s_lzma_prop(_Inout_ CLzmaEncProps *p_prop, _In_ uint32_t p_level,
                        _In_ int p_src_size) noexcept {
  // set up properties
  LzmaEncProps_Init(p_prop);

  constexpr auto _min_dic_size = 1 << 12;
  constexpr auto _max_dic_size = 3 << 29;
  constexpr auto _min_level = 0;
  constexpr auto _max_level = 9;

  constexpr auto _fb = 40;

  if (p_src_size <= _min_dic_size) {
    p_prop->dictSize = _min_dic_size;
  } else if (p_src_size >= _max_dic_size) {
    p_prop->dictSize = _max_dic_size;
  } else {
    p_prop->dictSize =
        gsl::narrow_cast<uint32_t>(p_src_size); // smaller dictionary = faster!
  }
  p_prop->fb = _fb;

  if (p_level >= _max_level) {
    p_level = _max_level;
  } else if (p_level <= _min_level) {
    p_level = _min_level;
  }
  p_prop->level = p_level;
}

boost::leaf::result<std::vector<std::byte>>
w_lzma::compress_lzma1(_In_ const gsl::span<const std::byte> p_src,
                       _In_ uint32_t p_level) {
  const auto _src_size = gsl::narrow_cast<int>(p_src.size());
  if (_src_size == 0) {
    return W_FAILURE(std::errc::invalid_argument, "the source is empty");
  }

  // set up properties
  CLzmaEncProps _props = {};
  s_lzma_prop(&_props, p_level, _src_size);

  // prepare space for the encoded properties
  size_t _props_size = LZMA_PROPS_SIZE;
  std::array<std::byte, LZMA_PROPS_SIZE> _props_encoded = {};

  // allocate some space for the compression output
  // this is way more than necessary in most cases
  auto _output_size_64 = gsl::narrow_cast<size_t>(_src_size) * 2;
  if (_output_size_64 < 1024) {
    _output_size_64 = 1024;
  }

  std::vector<std::byte> _tmp;
  _tmp.resize(_output_size_64);

  const auto _lzma_status =
      LzmaEncode((Byte *)_tmp.data(), &_output_size_64, (Byte *)p_src.data(),
                 p_src.size(), &_props, (Byte *)_props_encoded.data(),
                 &_props_size, 0, nullptr, &s_alloc_funcs, &s_alloc_funcs);

  const auto _compressed_size =
      _output_size_64 + LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE;

  if (_lzma_status == SZ_OK) {
    std::vector<std::byte> _dst;
    _dst.reserve(_compressed_size);

    // tricky: we have to generate the LZMA header
    // 5 bytes properties + 8 byte uncompressed size
    std::copy(_props_encoded.begin(), _props_encoded.begin() + LZMA_PROPS_SIZE,
              std::back_inserter(_dst));

    for (int i = 0; i < LZMA_HEADER_SRC_SIZE; i++) {
      _dst.push_back(std::byte((_src_size >> (i * 8)) & 0xFF));
    }

    // copy the compressed size
    std::copy(_tmp.begin(), _tmp.begin() + _output_size_64,
              std::back_inserter(_dst));

    return _dst;
  }
  return W_FAILURE(std::errc::operation_canceled, "lzma1 compress failed");
}

boost::leaf::result<std::vector<std::byte>>
w_lzma::compress_lzma2(_In_ const gsl::span<const std::byte> p_src,
                       _In_ uint32_t p_level) {
  const auto _src_size = gsl::narrow_cast<int>(p_src.size());
  if (_src_size == 0) {
    return W_FAILURE(std::errc::invalid_argument, "the source is empty");
  }

  auto _enc_handler = Lzma2Enc_Create(&s_alloc_funcs, &s_alloc_funcs);
  if (!_enc_handler) {
    return W_FAILURE(std::errc::operation_canceled,
                     "failed on creating lzma2 encoder");
  }

  // set up properties
  CLzmaEncProps _props_1{};
  s_lzma_prop(&_props_1, p_level, _src_size);

  CLzma2EncProps _props_2{};
  Lzma2EncProps_Init(&_props_2);
  _props_2.lzmaProps = _props_1;

  const auto _props_status = Lzma2Enc_SetProps(_enc_handler, &_props_2);
  if (_props_status != SZ_OK) {
    return W_FAILURE(std::errc::operation_canceled,
                     "failed on setting lzma2 encoder properties");
  }

  // prepare space for the encoded properties
  const auto properties = Lzma2Enc_WriteProperties(_enc_handler);
  // allocate some space for the compression output
  // this is way more than necessary in most cases.
  auto _output_size_64 = gsl::narrow_cast<size_t>(_src_size) * 2;
  if (_output_size_64 < 1024) {
    _output_size_64 = 1024;
  }

  std::vector<std::byte> _tmp;
  _tmp.resize(_output_size_64);

  const auto _encode_status = Lzma2Enc_Encode2(
      _enc_handler, NULL, (Byte *)_tmp.data(), &_output_size_64, nullptr,
      (const Byte *)p_src.data(), _src_size, nullptr);
  Lzma2Enc_Destroy(_enc_handler);

  const auto _compressed_size =
      _output_size_64 + LZMA_HEADER_SRC_SIZE + sizeof(properties);

  if (_encode_status == SZ_OK) {
    std::vector<std::byte> _dst;
    _dst.reserve(_compressed_size);

    /*
        tricky: we have to generate the LZMA header
        1 byte properties + 8 bytes uncompressed size
    */
    _dst.push_back(std::byte(properties));
    for (int i = 0; i < LZMA_HEADER_SRC_SIZE; i++) {
      _dst.push_back(std::byte((_src_size >> (i * 8)) & 0xFF));
    }

    // copy the compressed size
    std::copy(_tmp.begin(), _tmp.begin() + _output_size_64,
              std::back_inserter(_dst));

    return _dst;
  }
  return W_FAILURE(std::errc::operation_canceled, "lzma2 compress failed");
}

boost::leaf::result<std::vector<std::byte>>
w_lzma::decompress_lzma1(_In_ const gsl::span<const std::byte> p_src) {
  const auto _src_size = p_src.size();

  if (_src_size < LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE) {
    return W_FAILURE(std::errc::invalid_argument, "invalid lzma1 header size");
  }

  // extract the size from the header
  uint64_t _size_from_header = 0;
  for (uint64_t i = 0; i < LZMA_HEADER_SRC_SIZE; i++) {
    const auto _h1 = gsl::at(p_src, LZMA_PROPS_SIZE + i);
    const auto _h2 = i * 8;
    const auto _h3 = gsl::narrow_cast<uint64_t>(_h1 << _h2);
    if (_h3 < std::numeric_limits<uint64_t>::max()) {
      _size_from_header |= _h3;
    }
  }

  std::vector<std::byte> _dst;
  if (_size_from_header <= MAX_HEADER_SIZE) {
    // allocate memory
    _dst.resize(_size_from_header);

    ELzmaStatus _lzma_status{};
    size_t _proc_out_size = _size_from_header,
           _proc_in_size = _src_size - LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE;
    // decode via lzma
    const auto _status = LzmaDecode(
        (Byte *)_dst.data(), &_proc_out_size,
        (Byte *)(&gsl::at(p_src, LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE)),
        &_proc_in_size, (const Byte *)p_src.data(), LZMA_PROPS_SIZE,
        LZMA_FINISH_END,
        &_lzma_status, &s_alloc_funcs);
    // return on success
    if (_status == SZ_OK && _proc_out_size == _size_from_header) {
      return _dst;
    }
  }
  return W_FAILURE(std::errc::operation_canceled, "lzma1 decompress failed");
}

boost::leaf::result<std::vector<std::byte>>
w_lzma::decompress_lzma2(_In_ gsl::span<const std::byte> p_src) {
  const auto _src_size = p_src.size();

  if (_src_size < LZMA_HEADER_SRC_SIZE + sizeof(Byte)) {
    return W_FAILURE(std::errc::invalid_argument, "invalid lzma2 header size");
  }

  // extract the size from the header
  uint64_t _size_from_header = 0;
  for (uint64_t i = 0; i < LZMA_HEADER_SRC_SIZE; i++) {
    const auto _h1 = gsl::at(p_src, sizeof(Byte) + i);
    const auto _h2 = i * 8;
    const auto _h3 = gsl::narrow_cast<uint64_t>(_h1 << _h2);
    if (_h3 < std::numeric_limits<uint64_t>::max()) {
      _size_from_header |= _h3;
    }
  }

  const auto _pre_out_size = _size_from_header * 2;
  if (_size_from_header <= MAX_HEADER_SIZE) {
    std::vector<std::byte> _dst;
    _dst.resize(_size_from_header);

    CLzma2Dec _dec{};
    Lzma2Dec_Construct(&_dec);

    auto _res = Lzma2Dec_Allocate(&_dec, 0, &s_alloc_funcs);
    if (_res != SZ_OK) {
      return W_FAILURE(std::errc::invalid_argument,
                       "could not allocate memory for lzma2 decoder");
    }

    Lzma2Dec_Init(&_dec);
    const auto properties = gsl::at(p_src, 0);
    unsigned _out_pos = 0, _in_pos = LZMA_HEADER_SRC_SIZE + sizeof(properties);

    ELzmaStatus _status = LZMA_STATUS_NOT_SPECIFIED;
    constexpr uint32_t BUF_SIZE = 10240;

    while (_out_pos < _pre_out_size) {
      size_t _dest_len = std::min((uint64_t)BUF_SIZE, _pre_out_size - _out_pos);
      size_t _src_len = std::min(gsl::narrow_cast<size_t>(BUF_SIZE),
                                 _src_size - gsl::narrow_cast<size_t>(_in_pos));

      _res = Lzma2Dec_DecodeToBuf(
          &_dec, (Byte *)&gsl::at(_dst, _out_pos), &_dest_len,
          (const Byte *)&gsl::at(p_src, _in_pos), &_src_len,
          (_out_pos + _dest_len == _size_from_header) ? LZMA_FINISH_END
                                                      : LZMA_FINISH_ANY,
          &_status);
      if (_res != SZ_OK) {
        return W_FAILURE(std::errc::invalid_argument,
                         "Lzma2Dec_DecodeToBuf failed");
      }

      _in_pos += _src_len;
      _out_pos += _dest_len;
      if (_status == LZMA_STATUS_FINISHED_WITH_MARK) {
        break;
      }
    }

    Lzma2Dec_Free(&_dec, &s_alloc_funcs);

    if (_out_pos == _size_from_header) {
      return _dst;
    }
  }

  return W_FAILURE(std::errc::operation_canceled, "lzma2 decompress failed");
}


#endif // WOLF_SYSTEM_LZMA