/*
        Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
module;

#include <gsl/gsl> //NO-LINT
#include <librist/librist.h>
#include <rist-private.h>
#include <wolf.hpp>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

export module wolf.stream.rist;

namespace wolf::stream::rist {

export struct w_rist_ctx_t {
  rist_ctx* ctx;
  rist_peer *peer;
  rist_profile profile;
  uint16_t loss_percentage;
  rist_logging_settings* log;
};

C_EXPORT typedef w_rist_ctx_t *w_rist_ctx;
C_EXPORT typedef rist_data_block *w_rist_data_block;
C_EXPORT typedef int (*rist_log_callback)(void *, rist_log_level, const char *);

C_EXPORT
auto w_rist_init(w_rist_ctx *p_rist, const char *p_url, rist_ctx_mode p_mode,
                 rist_profile p_profile,
                 unsigned short p_loss_percentage, rist_log_level p_log_level,
                 rist_log_callback p_log_callback) -> int {
  constexpr auto size = sizeof(w_rist_ctx_t);
  auto rist = gsl::narrow_cast<w_rist_ctx>(calloc(1, size));
  if (rist == nullptr) {
    return -1;
  }

  rist->profile = p_profile;
  rist->loss_percentage = p_loss_percentage;

  switch (p_mode) {
  case rist_ctx_mode::RIST_SENDER_MODE: {
    // create a log
    if (rist_logging_set(&rist->log, p_log_level, p_log_callback,
                         (void *)("sender"), nullptr, stderr) != 0) {
      return -1;
    }
    // create a sender
    auto ret = rist_sender_create(&rist->ctx, rist->profile, 0, rist->log);
    if (ret != 0) {
      rist_log(rist->log, RIST_LOG_ERROR,
               "could not create rist sender context");
      return -1;
    }
    if (p_loss_percentage > 0) {
      rist->ctx->sender_ctx->simulate_loss = true;
      rist->ctx->sender_ctx->loss_percentage = p_loss_percentage * 10;
    }
    break;
  }
  case rist_ctx_mode::RIST_RECEIVER_MODE: {
    // create a log
    if (rist_logging_set(&rist->log, p_log_level, p_log_callback,
                         (void *)("receiver"), nullptr, stderr) != 0) {
      return -1;
    }
    // create a receiver
    auto ret = rist_receiver_create(&rist->ctx, rist->profile, rist->log);
    if (ret != 0) {
      rist_log(rist->log, RIST_LOG_ERROR,
               "could not create rist receiver context");
      return -1;
    }
    if (p_loss_percentage > 0) {
      rist->ctx->receiver_ctx->simulate_loss = true;
      rist->ctx->receiver_ctx->loss_percentage = p_loss_percentage * 10;
    }

    break;
  }
  };

  // rely on the library to parse the url
  rist_peer_config *peer_config = nullptr;
  if (rist_parse_address2(p_url, &peer_config)) {
    rist_log(rist->log, RIST_LOG_ERROR,
             "could not parse peer options for receiver");
    return -1;
  }

  if (rist_peer_create(rist->ctx, &rist->peer, peer_config) == -1) {
    rist_log(rist->log, RIST_LOG_ERROR,
             "could not add peer connector to receiver");
    return -1;
  }
  free((void *)peer_config);

  if (rist_start(rist->ctx) == -1) {
    rist_log(rist->log, RIST_LOG_ERROR, "could not start rist receiver");
    return -1;
  }

  if (p_mode == rist_ctx_mode::RIST_SENDER_MODE) {
    // send a init buffer, this will be hrlp us to destroy rist on any situation 
    char buffer[5] = {0};
    rist_data_block data = {0};

    sprintf_s(buffer, "RIST\0");
    data.payload = &buffer;
    data.payload_len = 5;
    auto ret = rist_sender_data_write(rist->ctx, &data);
    if (ret < 0) {
      rist_log(rist->log, RIST_LOG_ERROR,
               "could not send the first test frame");
      return -1;
    }
  }

  *p_rist = rist;

  return 0;
}

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
auto w_rist_get_data_block(w_rist_data_block p_block) -> const void* {
  return p_block->payload;
}

C_EXPORT
auto w_rist_get_data_block_len(w_rist_data_block p_block) -> size_t {
  return p_block->payload_len;
}

C_EXPORT
auto w_rist_free_data_block(w_rist_data_block* p_block) -> void {
  if (p_block != nullptr) {
    rist_receiver_data_block_free2(p_block);
  }
  if (*p_block != nullptr) {
    free(*p_block);
    *p_block = nullptr;
  }
}

C_EXPORT
auto w_rist_send_data_block(w_rist_ctx p_rist, w_rist_data_block p_block) -> int {
  return rist_sender_data_write(p_rist->ctx, p_block);
}

C_EXPORT
auto w_rist_read_data_block(w_rist_ctx p_rist, w_rist_data_block *p_block, int p_timeout)
    -> int {
  return rist_receiver_data_read2(p_rist->ctx, p_block, p_timeout);
}

C_EXPORT
auto w_rist_fini(w_rist_ctx *p_rist) -> void {
  if (p_rist == nullptr || (*p_rist) == nullptr)
    return;

  if ((*p_rist)->ctx) {
    rist_destroy((*p_rist)->ctx);
  }

  // use free instead of mi_free
  // there is a bug with mi_free on windows
   if ((*p_rist)->log) {
     free ((*p_rist)->log);
   }

  free(*p_rist);
}

} // namespace wolf::stream::rist