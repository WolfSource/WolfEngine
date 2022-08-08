/*
        Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

/**
 * returns wolf version
 * @param p_buf buffer with maximum size of 260 characters
 * @param p_len the length of buffer, maximum size is 260
 * @return string format with the following style
 * "<major>.<minor>.<patch>.<debug>"
 */
W_EXPORT
void w_sys_version(char *p_buf, size_t p_len);

#ifdef __cplusplus
}
#endif