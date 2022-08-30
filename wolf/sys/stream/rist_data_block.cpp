#include "rist_data_block.h"
#include <gsl/gsl>
#include <librist/librist.h>
#include <rist-private.h>

int w_rist_init_data_block(_Inout_ w_rist_data_block* p_block)
{
    auto* block = gsl::owner<rist_data_block*>(calloc(1, sizeof(rist_data_block)));
    if (block == nullptr)
    {
        return -1;
    }
    *p_block = block;
    return 0;
}

void w_rist_set_data_block(
    _In_ w_rist_data_block p_block,
    _In_ size_t p_data_len,
    _In_ const void* p_data)
{
    p_block->payload_len = p_data_len;
    p_block->payload = p_data;
}

const void *w_rist_get_data_block(_In_ w_rist_data_block p_block) 
{
  return p_block->payload;
}

size_t w_rist_get_data_block_len(_In_ w_rist_data_block p_block) 
{
  return p_block->payload_len;
}

void w_rist_free_data_block(_Inout_ w_rist_data_block* p_block)
{
    if (p_block != nullptr)
    {
        rist_receiver_data_block_free2(p_block);

        if (*p_block != nullptr)
        {
            free(*p_block);
            *p_block = nullptr;
        }
    }
}
