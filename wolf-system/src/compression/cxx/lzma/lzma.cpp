#include "lzma.hpp"
#include "wolf-system/src/compression/lzma.rs.h"

#include <LzmaEnc.h>
#include <LzmaDec.h>
#include <Lzma2Enc.h>
#include <Lzma2Dec.h>

#include <limits>
#include <iostream>

#define LZMA_HEADER_SRC_SIZE 8

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

    static void _lzma_prop(CLzmaEncProps *pProp, uint32_t pLevel, int pSrcSize)
    {
        // set up properties
        LzmaEncProps_Init(pProp);

        if (pSrcSize <= (1 << 12))
        {
            pProp->dictSize = 1 << 12; //1mb dictionary
        }
        else if (pSrcSize >= (3 << 29))
        {
            pProp->dictSize = 3 << 29;
        }
        else
        {
            pProp->dictSize = (uint32_t)pSrcSize; //smaller dictionary = faster!
        }

        pProp->fb = 40;
        pProp->level = pLevel > 9 ? 9 : pLevel;
    }

    rust::Vec<uint8_t> compress_1(rust::Slice<const uint8_t> pSrcBuffer,
                                  uint32_t pLevel,
                                  rust::String &pTrace)
    {
        auto _src_size = (uint64_t)pSrcBuffer.size();
        if (_src_size == 0)
        {
            pTrace = "pSrcBuffer.size() == 0. trace info: lzma_cxx::compress_1";
            return rust::Vec<uint8_t>();
        }

        // set up properties
        CLzmaEncProps _props;
        _lzma_prop(&_props, pLevel, _src_size);

        // prepare space for the encoded properties
        size_t _props_size = LZMA_PROPS_SIZE;
        uint8_t _props_encoded[LZMA_PROPS_SIZE];

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
            pTrace = "could not allocate memory for compressed buffer. trace info: lzma_cxx::compress_1";
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

        auto _compressed_size = _output_size_64 + LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE;
        if (_lzma_status == SZ_OK)
        {
            rust::vec<uint8_t> v;
            v.reserve(_compressed_size);

            // tricky: we have to generate the LZMA header

            // 5 bytes properties + 8 byte uncompressed size
            copy(&_props_encoded[0], &_props_encoded[LZMA_PROPS_SIZE], std::back_inserter(v));
            for (int i = 0; i < LZMA_HEADER_SRC_SIZE; i++)
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
            pTrace = "LzmaEncode failed on compressing. trace info: lzma_cxx::compress_1";
            return rust::Vec<uint8_t>();
        }
    }

    rust::Vec<uint8_t> decompress_1(rust::Slice<const uint8_t> pSrcBuffer,
                                    rust::String &pTrace)
    {
        auto _src_size = (uint64_t)pSrcBuffer.size();
        if (_src_size < LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE)
        {
            pTrace = "invalid lzma header. trace info: lzma_cxx::decompress_1";
            return rust::Vec<uint8_t>();
        }

        // extract the size from the header
        uint64_t _size_from_header = 0;
        for (uint64_t i = 0; i < LZMA_HEADER_SRC_SIZE; i++)
        {
            auto a = (uint64_t)(pSrcBuffer[LZMA_PROPS_SIZE + i]);
            auto b = (uint64_t)(i * 8);
            auto c = a << b;
            if (c < std::numeric_limits<uint64_t>::max())
            {
                _size_from_header |= c;
            }
        }

        rust::Vec<uint8_t> v;

        if (_size_from_header <= (256 * 1024 * 1024))
        {
            auto data = (uint8_t *)malloc(_size_from_header);
            if (!data)
            {
                pTrace = "could not allocate memory for lzma decompress buffer. trace info: lzma_cxx::decompress_1";
                return rust::Vec<uint8_t>();
            }

            ELzmaStatus _lzma_status;
            size_t _proc_out_size = _size_from_header, _proc_in_size = _src_size - LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE;
            int status = LzmaDecode(
                data,
                &_proc_out_size,
                &pSrcBuffer[LZMA_HEADER_SRC_SIZE + LZMA_PROPS_SIZE],
                &_proc_in_size,
                pSrcBuffer.data(),
                5,
                LZMA_FINISH_END,
                &_lzma_status,
                &_alloc_funcs);

            if (status == SZ_OK && _proc_out_size == _size_from_header)
            {
                data[_proc_out_size] = '\0';

                v.reserve(_proc_out_size);

                //copy data
                copy(&data[0], &data[_proc_out_size], std::back_inserter(v));

                //free memory
                free(data);
            }
        }
        else
        {
            pTrace = "invalid lzma header size. trace info: lzma_cxx::decompress_1";
            return rust::Vec<uint8_t>();
        }

        return v;
    }

    rust::Vec<uint8_t> compress_2(rust::Slice<const uint8_t> pSrcBuffer,
                                  uint32_t pLevel,
                                  rust::String &pTrace)
    {
        auto _src_size = (uint64_t)pSrcBuffer.size();
        if (_src_size == 0)
        {
            pTrace = "pSrcBuffer.size() == 0. trace info: lzma_cxx::compress_2";
            return rust::Vec<uint8_t>();
        }

        CLzma2EncHandle _enc_handler = Lzma2Enc_Create(&_alloc_funcs, &_alloc_funcs);
        if (!_enc_handler)
        {
            pTrace = "Lzma2Enc_Create(...) == NULL. trace info: lzma_cxx::compress_2";
            return rust::Vec<uint8_t>();
        }

        // set up properties
        CLzmaEncProps _props_1;
        _lzma_prop(&_props_1, pLevel, _src_size);

        CLzma2EncProps _props_2;
        Lzma2EncProps_Init(&_props_2);
        _props_2.lzmaProps = _props_1;

        SRes _props_status = Lzma2Enc_SetProps(_enc_handler, &_props_2);
        if (_props_status != SZ_OK)
        {
            pTrace = "Lzma2Enc_SetProps(...) == SZ_OK. trace info: lzma_cxx::compress_2";
            return rust::Vec<uint8_t>();
        }

        // prepare space for the encoded properties
        Byte properties = Lzma2Enc_WriteProperties(_enc_handler);

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

        std::vector<uint8_t> _out_buffer;

        uint8_t *_compress_mem = (uint8_t *)malloc(_output_size_64 * sizeof(uint8_t));
        if (!_compress_mem)
        {
            pTrace = "could not allocate memory for compressed buffer. trace info: lzma_cxx::compress";
            return rust::Vec<uint8_t>();
        }

        SRes _encode_status = Lzma2Enc_Encode2(
            _enc_handler,
            NULL,
            _compress_mem,
            &_output_size_64,
            NULL,
            (const Byte *)pSrcBuffer.data(),
            _src_size,
            NULL);

        Lzma2Enc_Destroy(_enc_handler);

        auto _compressed_size = _output_size_64 + LZMA_HEADER_SRC_SIZE + sizeof(properties);

        if (_encode_status == SZ_OK)
        {
            rust::Vec<uint8_t> v;
            v.reserve(_compressed_size);

            // tricky: we have to generate the LZMA header

            // 1 byte properties + 8 bytes uncompressed size
            v.push_back(properties);
            for (int i = 0; i < LZMA_HEADER_SRC_SIZE; i++)
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

    rust::Vec<uint8_t> decompress_2(rust::Slice<const uint8_t> pSrcBuffer,
                                    rust::String &pTrace)
    {
        auto _src_size = (uint64_t)pSrcBuffer.size();
        if (_src_size < LZMA_HEADER_SRC_SIZE + sizeof(Byte))
        {
            pTrace = "invalid lzma2 header. trace info: lzma_cxx::decompress_2";
            return rust::Vec<uint8_t>();
        }

        // extract the size from the header
        uint64_t _size_from_header = 0;
        for (uint64_t i = 0; i < LZMA_HEADER_SRC_SIZE; i++)
        {
            auto a = (uint64_t)(pSrcBuffer[sizeof(Byte) + i]);
            auto b = (uint64_t)(i * 8);
            auto c = a << b;
            if (c < std::numeric_limits<uint64_t>::max())
            {
                _size_from_header |= c;
            }
        }

        uint64_t _pre_out_size = _size_from_header * 2;

        if (_size_from_header <= (256 * 1024 * 1024))
        {
            rust::Vec<uint8_t> _output_buffer;

            auto _data = (uint8_t *)malloc(_size_from_header);
            if (!_data)
            {
                pTrace = "could not allocate memory for lzma decompress buffer. trace info: lzma_cxx::decompress_2";
                return rust::Vec<uint8_t>();
            }

            CLzma2Dec _dec;
            Lzma2Dec_Construct(&_dec);

            SRes _res = Lzma2Dec_Allocate(&_dec, 0, &_alloc_funcs);

            if (_res != SZ_OK)
            {
                pTrace = "Lzma2Dec_Allocate(...) == SZ_OK. trace info: lzma_cxx::decompress_2";
                return rust::Vec<uint8_t>();
            }

            Lzma2Dec_Init(&_dec);

            Byte properties = pSrcBuffer[0];

            unsigned _out_pos = 0, _in_pos = LZMA_HEADER_SRC_SIZE + sizeof(properties);

            ELzmaStatus _status;
            const unsigned int BUF_SIZE = 10240;

            while (_out_pos < _pre_out_size)
            {

                size_t _dest_len = std::min((uint64_t)BUF_SIZE, _pre_out_size - _out_pos);
                size_t _src_len = std::min((size_t)BUF_SIZE, (size_t)_src_size - (size_t)_in_pos);

                _res = Lzma2Dec_DecodeToBuf(&_dec,
                                            &_data[_out_pos], &_dest_len,
                                            &pSrcBuffer[_in_pos], &_src_len,
                                            (_out_pos + _dest_len == _size_from_header) ? LZMA_FINISH_END : LZMA_FINISH_ANY,
                                            &_status);

                if (_res != SZ_OK)
                {
                    pTrace = "Lzma2Dec_DecodeToBuf(...) == SZ_OK. trace info: lzma_cxx::decompress_2";
                    return rust::Vec<uint8_t>();
                }

                _in_pos += _src_len;
                _out_pos += _dest_len;

                if (_status == LZMA_STATUS_FINISHED_WITH_MARK)
                {
                    break;
                }
            }

            Lzma2Dec_Free(&_dec, &_alloc_funcs);

            if (_out_pos == _size_from_header)
            {
                _data[_out_pos] = '\0';
                _output_buffer.reserve(_out_pos);

                //copy data
                copy(&_data[0], &_data[_out_pos], std::back_inserter(_output_buffer));

                //free memory
                free(_data);
            }

            return _output_buffer;
        }
        else
        {
            pTrace = "invalid lzma2 header size. trace info: lzma_cxx::decompress_2";
            return rust::Vec<uint8_t>();
        }
    }
}
