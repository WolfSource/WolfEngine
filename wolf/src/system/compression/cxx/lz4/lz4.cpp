#include "LZ4.hpp"
#include "wolfengine/src/system/compression/lz4.rs.h"
#include <mimalloc.h>
#include <gsl/gsl>

#include <lz4.h>

namespace lz4_cxx
{
    rust::Vec<uint8_t> compress(
        rust::Slice<const uint8_t> pSrcBuffer,
        bool pFastMode,
        uint32_t pAcceleration,
        rust::String& pTrace)
    {
        const auto _src_size = gsl::narrow_cast<int>(pSrcBuffer.size());
        if (_src_size == 0)
        {
            pTrace = "pSrcBuffer.size() == 0 . trace info: lz4_cxx::compress";
            return rust::Vec<uint8_t>();
        }

        //allocate size for compressed data
        const auto _max_dst_size = LZ4_compressBound(_src_size);
        auto data = gsl::narrow_cast<char*>(mi_malloc(_max_dst_size));
        if (!data)
        {
            pTrace = "could not allocate memory for compressed buffer. trace info: lz4_cxx::compress";
            return rust::Vec<uint8_t>();
        }

        int _compressed_buffer_size = -1;
        if (pFastMode)
        {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif

            _compressed_buffer_size =
                LZ4_compress_fast(
                    (const char*)pSrcBuffer.data(),
                    data,
                    _src_size,
                    _max_dst_size,
                    (int)pAcceleration);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
        }
        else
        {
            _compressed_buffer_size = LZ4_compress_default(
                (const char*)pSrcBuffer.data(),
                data,
                _src_size,
                _max_dst_size);
        }

        if (_compressed_buffer_size > 0)
        {
            //realloc compress_data to free up memory
            data = gsl::narrow_cast<char*>(mi_realloc(data, _compressed_buffer_size));
            if (!data)
            {
                pTrace = "could not realloc compressed memory. trace info: lz4_cxx::compress";
                return rust::Vec<uint8_t>();
            }
            else
            {
                rust::Vec<uint8_t> v;
                v.reserve(_compressed_buffer_size);

                copy(&data[0], &data[_compressed_buffer_size], std::back_inserter(v));

                // free data
                mi_free(data);

                return v;
            }
        }
        else
        {
            pTrace = "could not compress. trace info: lz4_cxx::compress";
            return rust::Vec<uint8_t>();
        }
    }

    rust::Vec<uint8_t> decompress(
        rust::Slice<const uint8_t> pSrcBuffer,
        rust::String& pTrace)
    {
        auto _src_size = gsl::narrow_cast<int>(pSrcBuffer.size());
        if (_src_size == 0)
        {
            pTrace = "pSrcBuffer.size() == 0. trace info: lz4_cxx::decompress";
            return rust::Vec<uint8_t>();
        }

        //allocate memory for decompress
        auto _destination_capacity = _src_size * 2;
        auto data = gsl::narrow_cast<char*>(mi_malloc(_destination_capacity));
        if (!data)
        {
            pTrace = "could not allocate memory for de-compressed buffer. trace info: lz4_cxx::compress";
            return rust::Vec<uint8_t>();
        }

        int _decompressed_size = -1;
        int _number_of_try = 10;

        //try for decompressing
        while (_number_of_try > 0)
        {
            _decompressed_size = LZ4_decompress_safe(
                (const char*)pSrcBuffer.data(),
                data,
                _src_size,
                (int)_destination_capacity);
            if (_decompressed_size >= 0)
            {
                break;
            }
            else
            {
                //we need to allocate a larget buffer for decompressing
                _destination_capacity *= 2;
                data = gsl::narrow_cast<char*>(mi_realloc(data, _destination_capacity));
                if (!data)
                {
                    pTrace = "could not re-allocate memory of de-compressed buffer. trace info: lz4_cxx::compress";
                    break;
                }
            }
            _number_of_try--;
        }

        if (_decompressed_size <= 0)
        {
            pTrace = "size of decompress buffer must be greater than zero. trace info: lz4_cxx::decompress";
            mi_free(data);
            return rust::Vec<uint8_t>();
        }
        else
        {
            data = gsl::narrow_cast<char*>(mi_realloc(data, _decompressed_size));
            if (!data)
            {
                pTrace = "could not realloc memory for de-compressed buffer trace info: lz4_cxx::decompress";
                mi_free(data);
                return rust::Vec<uint8_t>();
            }
            else
            {
                rust::Vec<uint8_t> v;
                v.reserve(_decompressed_size);

                copy(&data[0], &data[_decompressed_size], std::back_inserter(v));

                // free data
                mi_free(data);

                return v;
            }
        }
    }
}