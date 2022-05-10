#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <wolf.h>

    /**
     * compress stream using lz4 algorithm
     * @param p_src_buffer the source buffer
     * @param p_src_buffer_len the len of source buffer
     * @param p_compressed_buffer the result compressed buffer
     * @param p_compressed_buffer_len the len of compressed buffer
     * @param p_fast_mode 1 means Fast mode and 0 is Default mode
     * @param p_acceleration the acceleration of process. the default value is 1
     * @param p_trace_buffer the trace information in string with maximum size 256
     * @return error code, 0 means success
     */
    WOLF_API int compress(const char *p_src_buffer,
                          int p_src_buffer_len,
                          char *p_compressed_buffer,
                          int *p_compressed_buffer_len,
                          int p_fast_mode,
                          int p_acceleration,
                          char *p_trace_buffer);

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
