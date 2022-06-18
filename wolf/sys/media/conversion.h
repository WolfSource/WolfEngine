/*
        Project: Wolf Engine. Copyright � 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <libavutil/frame.h>

#ifdef __cplusplus
}
#endif

#include <wolf.h>
#include <stdint.h>

/**
 * converts rgb buffer to yuv buffer
 * @param p_rgb the input rgb array
 * @param p_width the width of the input frame
 * @param p_height the height of the input frame
 * @param p_frame the output yuv frame
 * @param p_error the error buffer
 * @return int the result of converting rgb to yuv
 */
int rgb2yuv(uint8_t *p_rgb, int p_width, int p_height, AVFrame **p_frame,
            char *p_error);

/**
 * converts yuv buffer to rgb buffer
 * @param p_frame_in the input yuv frame
 * @param p_width the width of the input frame
 * @param p_height the height of the input frame
 * @param p_frame_out the output rgb frame
 * @param p_error the error buffer
 * @return int the result of converting yuv to rgb
 */
int yuv2rgb(AVFrame* p_frame_in, int p_width, int p_height,
            AVFrame* p_frame_out, char *p_error);