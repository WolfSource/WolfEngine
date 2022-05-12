#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <wolf.h>

    typedef struct w_rist_t *w_rist;

    /**
     * initialize rist
     * @param p_rist the object of rist
     * @param p_trace the trace information in the string format with maximum size of 256
     * @return 0 means success
     */
    int w_rist_init(w_rist p_rist, w_buf p_trace);

    // /**
    //  * compress stream using lz4 algorithm
    //  * @param p_src the source buffer
    //  * @param p_dst the result of compressed buffer
    //  * @param p_fast_mode 1 means Fast mode and 0 is Default mode
    //  * @param p_acceleration the acceleration of process. the default value is 1
    //  * @param p_trace the trace information in the string fromat with maximum size of 256
    //  * @return 0 means success
    //  */
    // int receive();

    // /**
    //  * compress stream using lz4 algorithm
    //  * @param p_src the source buffer
    //  * @param p_dst the result of compressed buffer
    //  * @param p_fast_mode 1 means Fast mode and 0 is Default mode
    //  * @param p_acceleration the acceleration of process. the default value is 1
    //  * @param p_trace the trace information in the string format with the maximum size of 256
    //  * @return 0 means success
    //  */
    // int fini();

#ifdef __cplusplus
}
#endif
