#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <wolf.h>

    /**
     * compress stream using lz4 algorithm
     * @param p_buffer is the source buffer
     * @param p_fast_mode true means Fast mode and zero is Default mode
     * @param p_acceleration is the acceleration of process. the default value is 1
     * @param p_trace is the trace information in string
     * @return error code
     */
    WOLF_API int compress(const uint8_t *p_src_buffer,
                          size_t p_src_buffer_len,
                          uint8_t *p_compressed_buffer,
                          size_t *p_compressed_buffer_len,
                          int p_fast_mode,
                          uint32_t p_acceleration,
                          char *p_trace_buffer,
                          size_t *p_trace_buffer_len);
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
