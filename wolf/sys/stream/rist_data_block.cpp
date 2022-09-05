#include "rist_data_block.h"
#include <wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>
#include <librist/librist.h>
#include <rist-private.h>
#include <DISABLE_ANALYSIS_END>

int w_rist_init_data_block(_Inout_ w_rist_data_block* p_block)
{
    *p_block = gsl::owner<rist_data_block*>(calloc(1, sizeof(rist_data_block)));
    return (*p_block == nullptr) ? -1 : 0;
}

void w_rist_set_data_block(
    _In_ w_rist_data_block p_block,
    _In_ size_t p_data_len,
    _In_ const void* p_data)
{
    p_block->payload_len = p_data_len;
    p_block->payload = p_data;
}

const void* w_rist_get_data_block(_In_ const w_rist_data_block p_block)
{
    const auto _not_null = gsl::not_null<const w_rist_data_block>(p_block);
    return _not_null->payload;
}

size_t w_rist_get_data_block_len(_In_ const w_rist_data_block p_block)
{
    const auto _not_null = gsl::not_null<const w_rist_data_block>(p_block);
    return _not_null->payload_len;
}

void w_rist_free_data_block(_Inout_ w_rist_data_block* p_block)
{
    const auto _not_null_ptr = gsl::not_null<w_rist_data_block*>(p_block);
    rist_receiver_data_block_free2(_not_null_ptr);

    const auto _not_null_block = gsl::not_null<w_rist_data_block>(*_not_null_ptr);
    free(_not_null_block);

    p_block = nullptr;
}
