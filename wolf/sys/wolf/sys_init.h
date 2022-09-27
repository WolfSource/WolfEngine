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
     * initialize the wolf sys
     * @param p_dump_path, the crash dump path
     * @param p_buf, an inout buffer with maximum size of 260 characters
     * @param p_buf_len, the input length of buffer. The version will be printed like this: "<major>.<minor>.<patch>.<debug>"
     * @returns zero on success
     */
    W_API
        int w_sys_init(_Inout_z_ char* p_buf, _In_ size_t p_buf_len);

#ifdef __cplusplus
}
#endif