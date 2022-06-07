/*
        Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
module;

#include <gsl/gsl> //NO-LINT
#include <librist/librist.h>
#include <wolf.hpp>

export module wolf.stream.rist_data_block;

namespace wolf::stream::rist {

#pragma region types

C_EXPORT typedef rist_data_block *w_rist_data_block;

#pragma endregion

#pragma region functions

C_EXPORT
auto w_rist_init_data_block(w_rist_data_block *p_block) -> int {
  auto block =
      gsl::narrow_cast<rist_data_block *>(calloc(1, sizeof(rist_data_block)));
  if (block == nullptr) {
    return -1;
  }

  *p_block = block;
  return 0;
}

C_EXPORT
auto w_rist_set_data_block(w_rist_data_block p_block, const void *p_data,
                           size_t p_data_len) -> void {
  p_block->payload = p_data;
  p_block->payload_len = p_data_len;
}

C_EXPORT
auto w_rist_get_data_block(w_rist_data_block p_block) -> const void * {
  return p_block->payload;
}

C_EXPORT
auto w_rist_get_data_block_len(w_rist_data_block p_block) -> size_t {
  return p_block->payload_len;
}

C_EXPORT
auto w_rist_free_data_block(w_rist_data_block *p_block) -> void {
  if (p_block != nullptr) {
    rist_receiver_data_block_free2(p_block);
  }
  if (*p_block != nullptr) {
    free(*p_block);
    *p_block = nullptr;
  }
}

#pragma endregion

} // namespace wolf::stream::rist