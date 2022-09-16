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

    /**
     * initialize rist data block
     * @param p_block the rist block data,
     * @returns zero on success
     */
    W_API
        int w_rist_init_data_block(_Inout_ w_rist_data_block* p_block);

    /**
     * set rist data block
     * @param p_block the rist block data,
     * @param p_data_len length of rist block data,
     * @param p_data the data,
     * @returns void
     */
    W_API
        void w_rist_set_data_block(
            _In_ w_rist_data_block p_block,
            _In_ size_t p_data_len,
            _In_ const void* p_data);

    /**
     * get rist data block
     * @param p_block the rist block data,
     * @returns pointer to data block
     */
    W_API
        const void* w_rist_get_data_block(_In_ const w_rist_data_block p_block);

    /**
     * get the length of rist data block
     * @returns the length as size_t
     */
    W_API
        size_t w_rist_get_data_block_len(_In_ const w_rist_data_block p_block);

    /**
     * free the rist data block
     * @returns void
     */
    W_API
        void w_rist_free_data_block(_Inout_ w_rist_data_block* p_block);

#ifdef __cplusplus
}
#endif