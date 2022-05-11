#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <wolf.h>

    /**
     * compress stream using lz4 algorithm
     * @param p_src the source buffer
     * @param p_dst the result of compressed buffer
     * @param p_fast_mode 1 means Fast mode and 0 is Default mode
     * @param p_acceleration the acceleration of process. the default value is 1
     * @param p_trace the trace information in string with maximum size of 256
     * @return error code, 0 means success
     */
    int compress(
        buf_t p_src,
        buf_t p_dst,
        int p_fast_mode,
        int p_acceleration,
        buf_t p_trace);

    /**
     * decompress lz4 stream
     * @param p_src the compressed source buffer
     * @param p_dst the decompressed buffer
     * @param p_trace the trace information in string with maximum size of 256
     * @return error code, 0 means success
     */
    int decompress(
        buf_t p_src,
        buf_t p_dst,
        buf_t p_trace);

    /**
     * free buffer
     * @param p_buf the buffer
     * @return error code, 0 means success
     */
    int free_buf(buf_t p_buf);

#ifdef __cplusplus
}
#endif
