/*
        Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

    typedef struct rist_data_block* w_rist_data_block;

    W_API
        int w_rist_init_data_block(_Inout_ w_rist_data_block* p_block);

    W_API
        void w_rist_set_data_block(
            _In_ w_rist_data_block p_block,
            _In_ size_t p_data_len,
            _In_ const void* p_data);

    W_API
        const void* w_rist_get_data_block(_In_ w_rist_data_block p_block);

    W_API
        size_t w_rist_get_data_block_len(_In_ w_rist_data_block p_block);

    W_API
        void w_rist_free_data_block(_Inout_ w_rist_data_block* p_block);

#ifdef __cplusplus
}
#endif