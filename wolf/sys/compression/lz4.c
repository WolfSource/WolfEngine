#include "lz4.h"
#include <lz4.h>
#include <mimalloc.h>

int compress(const char *p_src_buffer,
             int p_src_buffer_len,
             char *p_compressed_buffer,
             int *p_compressed_buffer_len,
             int p_fast_mode,
             int p_acceleration,
             char *p_trace_buffer)
{
    if (p_trace_buffer == NULL)
    {
        return 1;
    }

    // cast source size
    if (p_src_buffer_len == 0)
    {
        (void)snprintf(p_trace_buffer,
                       MAX_TRACE_LEN,
                       "p_src_buffer_len is zero");
        return 1;
    }

    // allocate size for compressed data
    const int _max_dst_size = LZ4_compressBound(p_src_buffer_len);
    char *data = (char *)mi_malloc(_max_dst_size);
    if (data == NULL)
    {
        (void)snprintf(p_trace_buffer,
                       MAX_TRACE_LEN,
                       "could not allocate memory for compressed buffer");
        return 1;
    }

    if (p_fast_mode != 0)
    {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif

        *p_compressed_buffer_len =
            LZ4_compress_fast(
                p_src_buffer,
                data,
                p_src_buffer_len,
                _max_dst_size,
                p_acceleration);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
    }
    else
    {
        *p_compressed_buffer_len = LZ4_compress_default(
            p_src_buffer,
            data,
            p_src_buffer_len,
            _max_dst_size);
    }

    if (*p_compressed_buffer_len > 0)
    {
        // realloc compress_data to free up memory
        data = (char *)mi_realloc(data, *p_compressed_buffer_len);
        if (data == NULL)
        {
            (void)snprintf(p_trace_buffer,
                           MAX_TRACE_LEN,
                           "could not realloc compressed memory");

            return 1;
        }

        p_compressed_buffer = data;

        return 0;
    }

    (void)snprintf(p_trace_buffer,
                   MAX_TRACE_LEN,
                   "could not compress");
    return 1;
}

//     rust::Vec<uint8_t> decompress(
//         rust::Slice<const uint8_t> pSrcBuffer,
//         rust::String& pTrace)
//     {
//         auto _src_size = gsl::narrow_cast<int>(pSrcBuffer.size());
//         if (_src_size == 0)
//         {
//             pTrace = "pSrcBuffer.size() == 0. trace info: lz4_cxx::decompress";
//             return rust::Vec<uint8_t>();
//         }

//         //allocate memory for decompress
//         auto _destination_capacity = _src_size * 2;
//         auto data = gsl::narrow_cast<char*>(mi_malloc(_destination_capacity));
//         if (!data)
//         {
//             pTrace = "could not allocate memory for de-compressed buffer. trace info: lz4_cxx::compress";
//             return rust::Vec<uint8_t>();
//         }

//         int _decompressed_size = -1;
//         int _number_of_try = 10;

//         //try for decompressing
//         while (_number_of_try > 0)
//         {
//             _decompressed_size = LZ4_decompress_safe(
//                 (const char*)pSrcBuffer.data(),
//                 data,
//                 _src_size,
//                 (int)_destination_capacity);
//             if (_decompressed_size >= 0)
//             {
//                 break;
//             }
//             else
//             {
//                 //we need to allocate a larget buffer for decompressing
//                 _destination_capacity *= 2;
//                 data = gsl::narrow_cast<char*>(mi_realloc(data, _destination_capacity));
//                 if (!data)
//                 {
//                     pTrace = "could not re-allocate memory of de-compressed buffer. trace info: lz4_cxx::compress";
//                     break;
//                 }
//             }
//             _number_of_try--;
//         }

//         if (_decompressed_size <= 0)
//         {
//             pTrace = "size of decompress buffer must be greater than zero. trace info: lz4_cxx::decompress";
//             mi_free(data);
//             return rust::Vec<uint8_t>();
//         }
//         else
//         {
//             data = gsl::narrow_cast<char*>(mi_realloc(data, _decompressed_size));
//             if (!data)
//             {
//                 pTrace = "could not realloc memory for de-compressed buffer trace info: lz4_cxx::decompress";
//                 mi_free(data);
//                 return rust::Vec<uint8_t>();
//             }
//             else
//             {
//                 rust::Vec<uint8_t> v;
//                 v.reserve(_decompressed_size);

//                 copy(&data[0], &data[_decompressed_size], std::back_inserter(v));

//                 // free data
//                 mi_free(data);

//                 return v;
//             }
//         }
//     }
// }