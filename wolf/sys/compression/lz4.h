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
     * @param p_is_fast_mode true means Fast mode and false is Default mode
     * @param p_acceleration the acceleration of process. the default value is 1
     * @param p_trace the trace information in the string format with maximum size of 256
     * @return 0 on success, 1 on invalid parameter and -1 on error which the error description will be printed into the p_trace
     */
    int w_lz4_compress(
        w_buf p_src,
        w_buf p_dst,
        bool p_is_fast_mode,
        int p_acceleration,
        w_buf p_trace);

    /**
     * decompress lz4 stream
     * @param p_src the compressed source buffer
     * @param p_dst the decompressed buffer
     * @param p_trace the trace information in the string fromat with maximum size of 256
     * @return 0 on success, 1 on invalid parameter and -1 on error which the error description will be printed into the p_trace
     */
    int w_lz4_decompress(
        w_buf p_src,
        w_buf p_dst,
        w_buf p_trace);

    /**
     * free buffer
     * @param p_buf the buffer
     * @return 0 on success, 1 on invalid parameter and -1 on error which the error description will be printed into the p_trace
     */
    int w_lz4_free_buf(w_buf p_buf);

#ifdef __cplusplus
}
#endif
