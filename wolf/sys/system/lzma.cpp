#include "lzma.h"
#include <wolf.hpp>

#include <Lzma2Enc.h>
#include <Lzma2Dec.h>
#include <LzmaDec.h>
#include <LzmaEnc.h>

#include <limits>

constexpr auto LZMA_HEADER_SRC_SIZE = 8;

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
    // set up properties
    LzmaEncProps_Init(p_prop);

    if (p_src_size <= (1 << 12))
    {
        p_prop->dictSize = 1 << 12; //1mb dictionary
    }
    else if (p_src_size >= (3 << 29))
    {
        p_prop->dictSize = 3 << 29;
    }
    else
    {
        p_prop->dictSize = gsl::narrow_cast<uint32_t>(p_src_size); //smaller dictionary = faster!
    }
    p_prop->fb = 40;
    p_prop->level = gsl::narrow_cast<int>(p_level > 9 ? 9 : p_level);
}

W_RESULT compress_lzma_1(
    uint8_t** p_dst,
    size_t* p_dst_len,
    const uint8_t* p_src,
    const size_t p_src_len,
    uint32_t p_level)
{
    const auto _dst_ptr_nn = gsl::not_null<uint8_t**>(p_dst);
    auto _dst_nn = gsl::not_null<uint8_t*>(*_dst_ptr_nn);
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
    if (_output_size_64 < 1024)
    {
        _output_size_64 = 1024;
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
        _dst_nn = gsl::narrow_cast<uint8_t*>(malloc(*_dst_len_nn));
        auto _dst_ptr = _dst_nn.get();

        // tricky: we have to generate the LZMA header
        // 5 bytes properties + 8 byte uncompressed size
        std::memcpy(_dst_ptr, _props_encoded, LZMA_PROPS_SIZE);
#ifdef __clang__
#pragma unroll
#endif
        for (auto i = 0; i < LZMA_HEADER_SRC_SIZE; i++)
        {
            _dst_ptr[i] = ((p_src_len >> (i * 8)) & 0xFF);
        }
        //copy the compressed size
        constexpr auto _start_index = 13; // 8 + 5 = 13 (0..12)
        std::memcpy(&_dst_ptr[_start_index], _compress_mem_nn, _output_size_64);

        //free memory
        free(_compress_mem);

        return W_SUCCESS;
    }
    return W_FAILURE;
}

// W_RESULT decompress_lzma_1(
//     uint8_t** p_dst,
//     size_t* p_dst_len,
//     const uint8_t* p_src,
//     const size_t p_src_len)
// {
//     const auto _dst_ptr_nn = gsl::not_null<uint8_t**>(p_dst);
//     auto _dst_nn = gsl::not_null<uint8_t*>(*_dst_ptr_nn);
//     const auto _dst_len_nn = gsl::not_null<size_t*>(p_dst_len);

//     constexpr auto INVALID_HEADER_ERROR = -2;

//     if (p_src_len < LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE)
//     {
//         // invalid lzma header
//         return INVALID_HEADER_ERROR;
//     }

//     // extract the size from the header
//     uint64_t _size_from_header = 0;

// #ifdef __clang__ 
// #pragma unroll
// #endif
//     for (uint64_t i = 0; i < LZMA_HEADER_SRC_SIZE; i++)
//     {
//         const auto header_data_1 = gsl::narrow_cast<uint64_t>(p_src[LZMA_PROPS_SIZE + i]);
//         const auto header_data_2 = gsl::narrow_cast<uint64_t>(i * 8);
//         const auto header_data_3 = header_data_1 << header_data_2;
//         if (header_data_3 < std::numeric_limits<uint64_t>::max())
//         {
//             _size_from_header |= header_data_3;
//         }
//     }

//     constexpr auto _minimum_size = 256 * 1024 * 1024;
//     if (_size_from_header <= _minimum_size)
//     {
//         _dst_nn = gsl::narrow_cast<uint8_t*>(malloc(_size_from_header));
        
//         auto _lzma_status = ELzmaStatus::LZMA_STATUS_NOT_SPECIFIED;
//         auto _proc_out_size = gsl::narrow_cast<size_t>(_size_from_header);
//         auto _proc_in_size = gsl::narrow_cast<size_t>(p_src_len - LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE);
//         const auto status = LzmaDecode(
//             data,
//             &_proc_out_size,
//             &gsl::at(pSrcBuffer, LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE),
//             &_proc_in_size,
//             p_src,
//             5,
//             LZMA_FINISH_END,
//             &_lzma_status,
//             &s_alloc_funcs);
//         if (status == SZ_OK && _proc_out_size == _size_from_header)
//         {
//             v.reserve(_proc_out_size);
//             //copy data
//             copy(&data[0], &data[_proc_out_size], std::back_inserter(v));
//             //free memory
//             mi_free(data);
//             return W_SUCCESS;
//         }
//     }
//     return W_FAILURE;
// }