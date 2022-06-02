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
  rist_profile profile;
  bool simulate_loos;
  uint16_t loss_percentage;
  rist_logging_settings* log;
};
C_EXPORT typedef w_rist_ctx_t *w_rist_ctx;
C_EXPORT typedef int (*rist_log_callback)(void *, rist_log_level, const char *);

C_EXPORT
auto w_rist_init(w_rist_ctx *p_rist, const char *p_url, rist_ctx_mode p_mode,
                 rist_profile p_profile, bool p_simulate_loos,
                 unsigned short p_loss_percentage, rist_log_level p_log_level,
                 rist_log_callback p_log_callback) -> int {

  constexpr auto size = sizeof(w_rist_ctx_t);
  auto rist = gsl::narrow_cast<w_rist_ctx>(calloc(1, size));
  if (rist == nullptr) {
    return -1;
  }

  rist->profile = p_profile;
  rist->simulate_loos = p_simulate_loos;
  rist->loss_percentage = p_loss_percentage;

  if (rist_logging_set(&rist->log, p_log_level, p_log_callback,
                       (void *)("receiver"), nullptr, stderr) != 0) {
    return -1;
  }

  switch (p_mode) {
  case rist_ctx_mode::RIST_SENDER_MODE: {
    auto ret = rist_sender_create(&rist->ctx, rist->profile, 0, rist->log);
    if (ret != 0) {
      rist_log(rist->log, RIST_LOG_ERROR,
               "could not create rist sender context");
      return -1;
    }
    rist->ctx->sender_ctx->simulate_loss = p_simulate_loos;
    rist->ctx->sender_ctx->loss_percentage = p_loss_percentage;
    break;
  } case rist_ctx_mode::RIST_RECEIVER_MODE: {
    auto ret = rist_receiver_create(&rist->ctx, rist->profile, rist->log);
    if (ret != 0) {
      rist_log(rist->log, RIST_LOG_ERROR,
               "could not create rist receiver context");
      return -1;
    }
    rist->ctx->receiver_ctx->simulate_loss = p_simulate_loos;
    rist->ctx->receiver_ctx->loss_percentage = p_loss_percentage;

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

  struct rist_peer *peer;
  if (rist_peer_create(rist->ctx, &peer, peer_config) == -1) {
    rist_log(rist->log, RIST_LOG_ERROR,
             "could not add peer connector to receiver");
    return -1;
  }
  free((void *)peer_config);

  if (rist_start(rist->ctx) == -1) {
    rist_log(rist->log, RIST_LOG_ERROR, "could not start rist receiver");
    return -1;
  }

  // send a buffer
  char buffer[5] = {0};
  struct rist_data_block data = {0};

  sprintf(buffer, "INIT");
  data.payload = &buffer;
  data.payload_len = 5;
  auto ret = rist_sender_data_write(rist->ctx, &data);
  if (ret < 0) {
    rist_log(rist->log, RIST_LOG_ERROR, "could not send the first test frame");
    return -1;
  }

  *p_rist = rist;

  return 0;
}

C_EXPORT
auto w_rist_send(w_rist_ctx p_rist, const char *p_buffer, int p_buffer_len)
    -> int {

  rist_data_block data = {0};
  data.payload = p_buffer;
  data.payload_len = p_buffer_len;
  return rist_sender_data_write(p_rist->ctx, &data);
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