#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <wolf.h>
#include <memory/buffer.h>

    // /**
    //  * compress stream using lz4 algorithm
    //  * @param p_buffer is the source buffer
    //  * @param p_fast_mode true means Fast mode and zero is Default mode
    //  * @param p_acceleration is the acceleration of process. the default value is 1
    //  * @param p_trace is the trace information in string
    //  * @return error code
    //  */
    // WOLF_API int compress(buffer *p_src_buffer,
    //                       buffer *p_compressed_buffer,
    //                       bool p_fast_mode,
    //                       uint32_t p_acceleration,
    //                       buffer *p_trace);
    //  rust::Vec<uint8_t> compress(
    //      rust::Slice<const uint8_t> pSrcBuffer,
    //      bool pFastMode,
    //      uint32_t pAcceleration,
    //      rust::String &pTrace);

    // /**
    //  * decompress stream using lz4 algorithm
    //  * @param pSrcBuffer is the compressed source buffer
    //  * @param pTrace is the trace information
    //  * @return decompressed memory
    //  */
    // rust::Vec<uint8_t> decompress(
    //     rust::Slice<const uint8_t> pSrcBuffer,
    //     rust::String &pTrace);

#ifdef __cplusplus
}
#endif
