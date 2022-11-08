#include "lzma.h"
#include <wolf.hpp>

#include <Lzma2Enc.h>
#include <Lzma2Dec.h>
#include <LzmaDec.h>
#include <LzmaEnc.h>

#include <limits>

constexpr auto ERROR_INVALID_HEADER = -2;
constexpr auto ERROR_SET_PROPERTIES = -3;
constexpr auto ERROR_DEC_ALLOCATE = -4;
constexpr auto ERROR_DEC_TO_BUFFER = -5;

constexpr auto LZMA_HEADER_SRC_SIZE = 8;
constexpr auto MINIMUM_SIZE = 256 * 1024 * 1024;
constexpr size_t CHUNK_SIZE = 1024;

static void* s_lzma_alloc(ISzAllocPtr p_ptr, size_t p_size) noexcept
{
    std::ignore = p_ptr;
    return mi_malloc(p_size);
}
static void s_lzma_free(ISzAllocPtr p_ptr, void* p_addr) noexcept
{
    std::ignore = p_ptr;
    mi_free(p_addr);
}

constexpr ISzAlloc s_alloc_funcs = { s_lzma_alloc, s_lzma_free };

static void s_lzma_prop(CLzmaEncProps* p_prop, uint32_t p_level, size_t p_src_size) noexcept
{
    constexpr auto one_mb_dictionary_const = 1 << 12;
    constexpr auto fb_const = 40;
    constexpr auto max_level_const = 9;
    constexpr auto dict_size_const = 3 << 29;

    // set up properties
    LzmaEncProps_Init(p_prop);
    const auto _p_prop_nn = gsl::not_null<CLzmaEncProps*>(p_prop);
    if (p_src_size <= one_mb_dictionary_const)
    {
        _p_prop_nn->dictSize = one_mb_dictionary_const;
    }
    else if (p_src_size >= dict_size_const)
    {
        _p_prop_nn->dictSize = dict_size_const;
    }
    else
    {
        _p_prop_nn->dictSize = gsl::narrow_cast<uint32_t>(p_src_size); //smaller dictionary = faster!
    }
    _p_prop_nn->fb = fb_const;
    _p_prop_nn->level = gsl::narrow_cast<int>(p_level > max_level_const ? max_level_const : p_level);
}

W_RESULT compress_lzma_1(
    _Inout_ uint8_t** p_dst,
    _Inout_ size_t* p_dst_len,
    _In_ const uint8_t* p_src,
    _In_ size_t p_src_len,
    _In_ uint32_t p_level)
{
    constexpr auto min_out_size = 1024;
    const auto _dst_len_nn = gsl::not_null<size_t*>(p_dst_len);

    // set up properties
    CLzmaEncProps _props;
    s_lzma_prop(&_props, p_level, p_src_len);

    // prepare space for the encoded properties
    size_t _props_size = LZMA_PROPS_SIZE;
    uint8_t _props_encoded[LZMA_PROPS_SIZE] = { 0 };

    // allocate some space for the compression output
    // this is way more than necessary in most cases
    auto _output_size_64 = gsl::narrow_cast<size_t>(p_src_len * 2);
    if (_output_size_64 < min_out_size)
    {
        _output_size_64 = min_out_size;
    }

    auto* _compress_mem = gsl::narrow_cast<uint8_t*>(malloc(_output_size_64));
    const auto _compress_mem_nn = gsl::not_null<uint8_t*>(_compress_mem);

    const auto _res = LzmaEncode(
        _compress_mem_nn,
        &_output_size_64,
        p_src,
        p_src_len,
        &_props,
        &_props_encoded[0],
        &_props_size,
        0,
        nullptr,
        &s_alloc_funcs,
        &s_alloc_funcs);

    *_dst_len_nn = _output_size_64 + LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE;
    if (_res == SZ_OK)
    {
        // allocate new memory for compressed lzma data
        auto _dst = gsl::narrow_cast<uint8_t*>(malloc(*_dst_len_nn));
        // just to make sure memory was allocated
        std::ignore = gsl::not_null<uint8_t*>(_dst);

        // tricky: we have to generate the LZMA header
        // 5 bytes properties + 8 byte uncompressed size
        std::memcpy(_dst, _props_encoded, LZMA_PROPS_SIZE);
#ifdef __clang__
#pragma unroll
#endif
        for (auto i = 0; i < LZMA_HEADER_SRC_SIZE; i++)
        {
            _dst[LZMA_PROPS_SIZE + i] = ((p_src_len >> (i * 8)) & 0xFF);
        }
        //copy the compressed size
        constexpr auto _start_index = 13; // 8 + 5 = 13 (0..12)
        std::memcpy(&_dst[_start_index], _compress_mem_nn, _output_size_64);

        //free memory
        free(_compress_mem);

        *p_dst = _dst;

        return W_SUCCESS;
    }
    *p_dst = nullptr;
    return W_FAILURE;
}

W_RESULT compress_lzma_2(
    _Inout_ uint8_t** p_dst,
    _Inout_ size_t* p_dst_len,
    _In_ const uint8_t* p_src,
    _In_ size_t p_src_len,
    _In_ uint32_t p_level)
{
    const auto _dst_len_nn = gsl::not_null<size_t*>(p_dst_len);
    constexpr auto min_out_size = 1024;

    auto* _enc_handler = Lzma2Enc_Create(&s_alloc_funcs, &s_alloc_funcs);
    const auto _enc_handler_nn = gsl::not_null<CLzma2EncHandle>(_enc_handler);

    // set up properties
    CLzmaEncProps _props_1;
    s_lzma_prop(&_props_1, p_level, p_src_len);

    CLzma2EncProps _props_2;
    Lzma2EncProps_Init(&_props_2);
    _props_2.lzmaProps = _props_1;

    const auto _props_status = Lzma2Enc_SetProps(_enc_handler_nn, &_props_2);
    if (_props_status != SZ_OK)
    {
        return ERROR_SET_PROPERTIES;
    }

    // prepare space for the encoded properties
    const auto properties = Lzma2Enc_WriteProperties(_enc_handler);
    // allocate some space for the compression output
    // this is way more than necessary in most cases.
    size_t _output_size_64 = p_src_len * 2;
    if (_output_size_64 < min_out_size)
    {
        _output_size_64 = min_out_size;
    }

    auto _compress_mem = gsl::narrow_cast<uint8_t*>(malloc(_output_size_64 * sizeof(uint8_t)));
    // check was the memory allocated or not
    std::ignore = gsl::not_null<uint8_t*>(_compress_mem);

    const auto _encode_status = Lzma2Enc_Encode2(
        _enc_handler,
        nullptr,
        _compress_mem,
        &_output_size_64,
        nullptr,
        p_src,
        p_src_len,
        nullptr);
    Lzma2Enc_Destroy(_enc_handler);

    *_dst_len_nn = _output_size_64 + LZMA_HEADER_SRC_SIZE + 1;
    if (_encode_status == SZ_OK)
    {
        auto _dst = gsl::narrow_cast<uint8_t*>(malloc(*_dst_len_nn));
        std::ignore = gsl::not_null<uint8_t*>(_dst);

        /*
            tricky: we have to generate the LZMA header
            1 byte properties + 8 bytes uncompressed size
        */
        _dst[0] = properties;
        for (int i = 0; i < LZMA_HEADER_SRC_SIZE; i++)
        {
            _dst[1 + i] = (p_src_len >> (i * 8)) & 0xFF;
        }

        //copy the compressed size
        memcpy(&_dst[LZMA_HEADER_SRC_SIZE + 1], _compress_mem, _output_size_64);

        //free memory
        free(_compress_mem);

        *p_dst = _dst;

        return W_SUCCESS;
    }
    return W_FAILURE;
}

W_RESULT decompress_lzma_1(
    _Inout_ uint8_t** p_dst,
    _Inout_ size_t* p_dst_len,
    _In_ const uint8_t* p_src,
    _In_ const size_t p_src_len)
{
    const auto _dst_len_nn = gsl::not_null<size_t*>(p_dst_len);

    if (p_src_len < LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE)
    {
        // invalid lzma header
        return ERROR_INVALID_HEADER;
    }

    // extract the size from the header
    uint64_t _size_from_header = 0;
#ifdef __clang__ 
#pragma unroll
#endif
    for (uint64_t i = 0; i < LZMA_HEADER_SRC_SIZE; ++i)
    {
        auto _size = gsl::narrow_cast<uint64_t>(p_src[LZMA_PROPS_SIZE + i]) << (i * 8);
        if (_size > 0 && _size < std::numeric_limits<uint64_t>::max())
        {
            _size_from_header |= _size;
        }
    }

    *_dst_len_nn = gsl::narrow_cast<size_t>(_size_from_header);
    if (_size_from_header <= MINIMUM_SIZE)
    {
        auto* _dst = gsl::narrow_cast<uint8_t*>(malloc(_size_from_header));
        // just to make sure memory was allocated
        std::ignore = gsl::not_null<uint8_t*>(_dst);

        auto _lzma_status = ELzmaStatus::LZMA_STATUS_NOT_SPECIFIED;
        auto _proc_in_size = gsl::narrow_cast<size_t>(p_src_len - LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE);
        const auto status = LzmaDecode(
            _dst,
            _dst_len_nn,
            &p_src[LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE],
            &_proc_in_size,
            p_src,
            5,
            LZMA_FINISH_END,
            &_lzma_status,
            &s_alloc_funcs);
        if (status == SZ_OK && *_dst_len_nn == _size_from_header)
        {
            *p_dst = _dst;
            return W_SUCCESS;
        }
    }
    *p_dst = nullptr;
    return W_FAILURE;
}

W_RESULT decompress_lzma_2(
    _Inout_ uint8_t** p_dst,
    _Inout_ size_t* p_dst_len,
    _In_ const uint8_t* p_src,
    _In_ const size_t p_src_len)
{
    const auto _dst_len_nn = gsl::not_null<size_t*>(p_dst_len);
    constexpr auto INVALID_HEADER_ERROR = -2;

    if (p_src_len < LZMA_HEADER_SRC_SIZE + 1)
    {
        return INVALID_HEADER_ERROR;
    }

    // extract the size from the header
    uint64_t _size_from_header = 0;
    for (uint64_t i = 0; i < LZMA_HEADER_SRC_SIZE; i++)
    {
        auto _header_part_1 = gsl::narrow_cast<uint64_t>(p_src[1 + i]);
        auto _header_part_2 = gsl::narrow_cast<uint64_t>(i * 8);
        auto _header_size = _header_part_1 << _header_part_2;
        if (_header_size > 0 && _header_size < std::numeric_limits<uint64_t>::max())
        {
            _size_from_header |= _header_size;
        }
    }
    const auto _pre_out_size = _size_from_header * 2;
    if (_size_from_header <= MINIMUM_SIZE)
    {
        auto* _dst = gsl::narrow_cast<uint8_t*>(malloc(_size_from_header));
        // just to make sure memory was allocated
        std::ignore = gsl::not_null<uint8_t*>(_dst);

        CLzma2Dec _dec;
        Lzma2Dec_Construct(&_dec);

        auto _res = Lzma2Dec_Allocate(&_dec, 0, &s_alloc_funcs);
        if (_res != SZ_OK)
        {
            return ERROR_DEC_ALLOCATE;
        }

        Lzma2Dec_Init(&_dec);

        size_t _out_pos = 0;
        size_t _in_pos = LZMA_HEADER_SRC_SIZE + 1;

        ELzmaStatus _status = LZMA_STATUS_NOT_SPECIFIED;
        while (_out_pos < _pre_out_size)
        {
            *_dst_len_nn = std::min(CHUNK_SIZE, _pre_out_size - _out_pos);
            auto _src_len = std::min(CHUNK_SIZE, p_src_len - _in_pos);
            _res = Lzma2Dec_DecodeToBuf(
                &_dec,
                &_dst[_out_pos],
                _dst_len_nn,
                &p_src[_in_pos],
                &_src_len,
                (_out_pos + *_dst_len_nn == _size_from_header) ? LZMA_FINISH_END : LZMA_FINISH_ANY,
                &_status);
            if (_res != SZ_OK)
            {
                return ERROR_DEC_TO_BUFFER;
            }
            _in_pos += _src_len;
            _out_pos += *_dst_len_nn;
            if (_status == LZMA_STATUS_FINISHED_WITH_MARK)
            {
                break;
            }
        }
        Lzma2Dec_Free(&_dec, &s_alloc_funcs);
        if (_out_pos == _size_from_header)
        {
            *p_dst = _dst;
            return W_SUCCESS;
        }
    }
    return W_FAILURE;
}

W_RESULT w_lzma_compress(
    _Inout_ uint8_t** p_dst,
    _Inout_ size_t* p_dst_len,
    _In_ const uint8_t* p_src,
    _In_ const size_t p_src_len,
    _In_ uint8_t p_type,
    _In_ uint32_t p_level)
{
    if (p_type == 0)
    {
        return compress_lzma_1(
            p_dst,
            p_dst_len,
            p_src,
            p_src_len,
            p_level);
    }
    else
    {
        return compress_lzma_2(
            p_dst,
            p_dst_len,
            p_src,
            p_src_len,
            p_level);
    }
}

W_RESULT w_lzma_decompress(
    _Inout_ uint8_t** p_dst,
    _Inout_ size_t* p_dst_len,
    _In_ const uint8_t* p_src,
    _In_ size_t p_src_len,
    _In_ uint8_t p_type)
{
    if (p_type == 0)
    {
        return decompress_lzma_1(
            p_dst,
            p_dst_len,
            p_src,
            p_src_len);
    }
    else
    {
        return decompress_lzma_2(
            p_dst,
            p_dst_len,
            p_src,
            p_src_len);
    }
}