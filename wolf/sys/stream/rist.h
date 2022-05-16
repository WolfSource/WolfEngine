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

    struct w_rist_config_t
    {
        w_rist_mode mode;
        int profile;
        int loss_percent;
        int timeout; // will be used for receiving
    } _ALIGNMENT_16_;
    typedef struct w_rist_config_t *w_rist_config;

    /**
     * initialize rist
     * @param p_rist the object of rist
     * @param p_url the url
     * @param p_profile the profile of rist
     * @param p_trace the trace information in the string format with maximum size of 256
     * @return 0 on success, 1 on invalid parameter and -1 on error which the error description will be printed into p_trace
     */
    int w_rist_init(w_rist *p_rist,
                    const char *p_url,
                    w_rist_config p_config,
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
     * is rist stream stopped
     * @param p_rist the object of rist
     * @return true on stop condition
     */
    bool w_rist_is_stopped(w_rist p_rist);

    /**
     * release rist resources
     * @param p_rist the object of rist
     * @return 0 on success, 1 on invalid parameter and -1 on error
     */
    int w_rist_fini(w_rist *p_rist);

#ifdef __cplusplus
}
#endif
