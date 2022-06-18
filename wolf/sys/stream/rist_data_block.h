/*
        Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

typedef struct rist_data_block *w_rist_data_block;

W_EXPORT
int w_rist_init_data_block(w_rist_data_block *p_block);

W_EXPORT
void w_rist_set_data_block(w_rist_data_block p_block, const void *p_data,
                          size_t p_data_len);

W_EXPORT
const void *w_rist_get_data_block(w_rist_data_block p_block);

W_EXPORT
size_t w_rist_get_data_block_len(w_rist_data_block p_block);

W_EXPORT
void w_rist_free_data_block(w_rist_data_block *p_block);

#ifdef __cplusplus
}
#endif