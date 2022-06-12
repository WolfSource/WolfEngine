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
 * @return string format with the following style
 * "<major>.<minor>.<patch>.<debug>"
 */
W_EXPORT
void w_sys_version(char *p_buf, size_t p_len);

#ifdef __cplusplus
}
#endif