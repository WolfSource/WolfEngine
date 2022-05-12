#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <wolf.h>

    typedef struct w_rist_t *w_rist;

    typedef enum w_rist_mode
    {
        w_rist_mode_receiver = 0,
        w_rist_mode_sender
    } w_rist_mode;

    /**
     * initialize rist
     * @param p_rist the object of rist
     * @param p_mode the sender or receiver modes
     * @param p_profile the profile of rist
     * @param p_url the url
     * @param p_loss_percent the loss percent an int value between 0 to 100
     * @param p_trace the trace information in the string format with maximum size of 256
     * @return 0 on success, 1 on invalid parameter and -1 on error which the error description will be printed into p_trace
     */
    int w_rist_init(w_rist *p_rist,
                    w_rist_mode p_mode,
                    int p_profile,
                    const char *p_url,
                    int p_loss_percent,
                    w_buf p_trace);

    /**
     * start rist stream
     * @param p_rist the object of rist
     * @param p_trace the trace information in the string format with maximum size of 256
     * @return 0 on success, 1 on invalid parameter and -1 on error which the error description will be printed into p_trace
     */
    int w_rist_start(w_rist p_rist, w_buf p_trace);

    /**
     * stop rist stream
     * @param p_rist the object of rist
     * @param p_trace the trace information in the string format with maximum size of 256
     * @return 0 on success, 1 on invalid parameter and -1 on error which the error description will be printed into p_trace
     */
    int w_rist_stop(w_rist p_rist, w_buf p_trace);

    /**
     * release rist resources
     * @param p_rist the object of rist
     * @return 0 on success, 1 on invalid parameter and -1 on error
     */
    int w_rist_fini(w_rist *p_rist);

#ifdef __cplusplus
}
#endif
