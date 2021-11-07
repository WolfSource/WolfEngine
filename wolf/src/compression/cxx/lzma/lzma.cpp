#include "lzma.hpp"
#include "wolf/src/compression/lzma.rs.h"

#include <LzmaEnc.h>
#include <LzmaDec.h>
#include <Lzma2Enc.h>
#include <Lzma2Dec.h>

#include <limits>
#include <iostream>

namespace lzma_cxx
{
    static void *_lzma_alloc(ISzAllocPtr pPtr, size_t pSize)
    {
        (void)pPtr;
        return malloc(pSize);
    }
    static void _lzma_free(ISzAllocPtr pPtr, void *pAddr)
    {
        (void)pPtr;
        free(pAddr);
    }

    static ISzAlloc _alloc_funcs = {_lzma_alloc, _lzma_free};

    rust::Vec<uint8_t> compress_1(rust::Slice<const uint8_t> pSrcBuffer,
                                  uint32_t pLevel,
                                  rust::String &pTrace)
    {
        auto _src_size = (int)pSrcBuffer.size();
        if (_src_size == 0)
        {
            pTrace = "pSrcBuffer.size() == 0. trace info: lzma_cxx::decompress";
            return rust::Vec<uint8_t>();
        }

        // set up properties
        CLzmaEncProps _props;
        LzmaEncProps_Init(&_props);

        if (_src_size >= (1 << 20))
        {
            _props.dictSize = 1 << 20; //1mb dictionary
        }
        else
        {
            _props.dictSize = (uint32_t)_src_size; //smaller dictionary = faster!
        }
        _props.fb = 40;
        _props.level = pLevel > 9 ? 9 : pLevel;

        // prepare space for the encoded properties
        size_t _props_size = 5;
        uint8_t _props_encoded[5];

        // allocate some space for the compression output
        // this is way more than necessary in most cases...
        // but better safe than sorry
        //   (a smarter implementation would use a growing buffer,
        //    but this requires a bunch of fuckery that is out of
        ///   scope for this simple example)
        size_t _output_size_64 = _src_size * 2;
        if (_output_size_64 < 1024)
        {
            _output_size_64 = 1024;
        }

        uint8_t *_compress_mem = (uint8_t *)malloc(_output_size_64 * sizeof(uint8_t));
        if (!_compress_mem)
        {
            pTrace = "could not allocate memory for compressed buffer. trace info: lzma_cxx::compress";
            return rust::Vec<uint8_t>();
        }

        int _lzma_status = LzmaEncode(
            _compress_mem,
            &_output_size_64,
            (const uint8_t *)pSrcBuffer.data(),
            _src_size,
            &_props,
            _props_encoded,
            &_props_size,
            0,
            NULL,
            &_alloc_funcs,
            &_alloc_funcs);

        auto _compressed_size = _output_size_64 + 13;
        if (_lzma_status == SZ_OK)
        {
            rust::vec<uint8_t> v;
            v.reserve(_compressed_size);

            // tricky: we have to generate the LZMA header

            // 5 bytes properties + 8 byte uncompressed size
            copy(&_props_encoded[0], &_props_encoded[5], std::back_inserter(v));
            for (int i = 0; i < 8; i++)
            {
                v.push_back((_src_size >> (i * 8)) & 0xFF);
            }

            //copy the compressed size
            copy(&_compress_mem[0], &_compress_mem[_output_size_64], std::back_inserter(v));

            //free memory
            free(_compress_mem);

            return v;
        }
        else
        {
            pTrace = "LzmaEncode failed on compressing. trace info: lzma_cxx::compress";
            return rust::Vec<uint8_t>();
        }
    }

    rust::Vec<uint8_t> decompress_1(rust::Slice<const uint8_t> pSrcBuffer,
                                    rust::String &pTrace)
    {
        auto _src_size = (int)pSrcBuffer.size();
        if (_src_size < 13)
        {
            pTrace = "invalid lzma header. trace info: lzma_cxx::decompress";
            return rust::Vec<uint8_t>();
        }

        // extract the size from the header
        uint64_t _size_from_header = 0;
        for (uint64_t i = 0; i < 8; i++)
        {
            auto a = (uint64_t)(pSrcBuffer[5 + i]);
            auto b = (uint64_t)(i * 8);
            auto c = a << b;
            if (c < std::numeric_limits<uint64_t>::max())
            {
                _size_from_header |= c;
            }
        }

        if (_size_from_header <= (256 * 1024 * 1024))
        {
            auto data = (uint8_t *)malloc(_size_from_header);
            if (!data)
            {
                pTrace = "could not allocate memory for lzma decompress buffer. trace info: lzma_cxx::decompress";
                return rust::Vec<uint8_t>();
            }

            ELzmaStatus _lzma_status;
            size_t _proc_out_size = _size_from_header, _proc_in_size = _src_size - 13;
            int status = LzmaDecode(
                data,
                &_proc_out_size,
                &pSrcBuffer[13],
                &_proc_in_size,
                pSrcBuffer.data(),
                5,
                LZMA_FINISH_END,
                &_lzma_status,
                &_alloc_funcs);

            if (status == SZ_OK && _proc_out_size == _size_from_header)
            {
                data[_proc_out_size] = '\0';

                rust::vec<uint8_t> v;
                v.reserve(_proc_out_size);

                //copy data
                copy(&data[0], &data[_proc_out_size], std::back_inserter(v));

                //free memory
                free(data);

                return v;
            }
        }
        else
        {
            pTrace = "invalid lzma header size. trace info: lzma_cxx::decompress";
            return rust::Vec<uint8_t>();
        }
    }
}
