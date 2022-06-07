/*
        Project: Wolf Engine. Copyright   2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
module;

#include <gsl/gsl> //NO-LINT
#include <librist/librist.h>
#include <rist-private.h>
#include <wolf.hpp>

export import wolf.stream.rist_data_block;
export module wolf.stream.rist;

namespace wolf::stream::rist {

#pragma region types

C_EXPORT typedef rist_peer *w_rist_peer;
C_EXPORT typedef const rist_oob_block *w_rist_oob_block;
C_EXPORT typedef const rist_stats *w_rist_stats;

#pragma endregion

export struct w_rist_ctx_t {
  rist_ctx *ctx;
  w_rist_peer peer;
  rist_profile profile;
  uint16_t loss_percentage;
  rist_logging_settings *log;
};
C_EXPORT typedef w_rist_ctx_t *w_rist_ctx;

#pragma region callbacks

C_EXPORT typedef int (*w_rist_log_callback)(void * /*p_arg*/,
                                            rist_log_level /*p_log_level*/,
                                            const char * /*p_msg*/);

C_EXPORT typedef int (*w_rist_auth_connect_callback)(
    void * /*p_arg*/, const char * /*p_conn_ip*/, uint16_t /*p_conn_port*/,
    const char * /*p_local_ip*/, uint16_t /*p_local_port*/,
    w_rist_peer /*p_peer*/);

C_EXPORT typedef int (*w_rist_auth_disconnect_callback)(void * /*p_arg*/,
                                                        w_rist_peer /*p_peer*/);

C_EXPORT typedef void (*w_rist_connection_status_callback)(
    void * /*p_arg*/, w_rist_peer /*p_peer*/,
    rist_connection_status /* p_peer_connection_status */);

C_EXPORT typedef int (*w_rist_out_of_band_callback)(
    void * /*p_arg*/, w_rist_oob_block /*oob_block*/);

C_EXPORT typedef int (*w_rist_stats_callback)(
    void * /*p_arg*/, w_rist_stats /*p_stats_container*/);

C_EXPORT typedef int (*w_receiver_data_callback)(
    void * /*p_arg*/, w_rist_data_block /*p_data_block*/);

#pragma endregion

#pragma region functions

C_EXPORT
auto w_rist_init(w_rist_ctx *p_rist, rist_ctx_mode p_mode,
                 rist_profile p_profile, unsigned short p_loss_percentage,
                 rist_log_level p_log_level, w_rist_log_callback p_log_callback)
    -> int {
  constexpr auto size = sizeof(w_rist_ctx_t);
  auto rist = gsl::narrow_cast<w_rist_ctx>(calloc(1, size));
  if (rist == nullptr) {
    return -1;
  }

  rist->profile = p_profile;
  rist->loss_percentage = p_loss_percentage;

  // create rist context
  rist_ctx *ctx = nullptr;

  switch (p_mode) {
  case rist_ctx_mode::RIST_SENDER_MODE: {
    // create a log
    if (rist_logging_set(&rist->log, p_log_level, p_log_callback,
                         (void *)("sender"), nullptr, stderr) != 0) {
      return -1;
    }
    // create a sender
    auto ret = rist_sender_create(&ctx, rist->profile, 0, rist->log);
    if (ret != 0) {
      rist_log(rist->log, RIST_LOG_ERROR,
               "could not create rist sender context");
      return -1;
    }
    if (p_loss_percentage > 0) {
      ctx->sender_ctx->simulate_loss = true;
      ctx->sender_ctx->loss_percentage = p_loss_percentage * 10;
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
    auto ret = rist_receiver_create(&ctx, rist->profile, rist->log);
    if (ret != 0) {
      rist_log(rist->log, RIST_LOG_ERROR,
               "could not create rist receiver context");
      return -1;
    }
    if (p_loss_percentage > 0) {
      ctx->receiver_ctx->simulate_loss = true;
      ctx->receiver_ctx->loss_percentage = p_loss_percentage * 10;
    }

    break;
  }
  };

  rist->ctx = ctx;
  *p_rist = rist;

  return 0;
}

C_EXPORT
auto w_rist_connect(w_rist_ctx p_rist, const char *p_url) -> int {

  // rely on the library to parse the url
  rist_peer_config *peer_config = nullptr;
  if (rist_parse_address2(p_url, &peer_config)) {
    rist_log(p_rist->log, RIST_LOG_ERROR,
             "could not parse peer options for receiver");
    return -1;
  }

  if (rist_peer_create(p_rist->ctx, &p_rist->peer, peer_config) == -1) {
    rist_log(p_rist->log, RIST_LOG_ERROR,
             "could not add peer connector to receiver");
    return -1;
  }
  free((void *)peer_config);

  if (rist_start(p_rist->ctx) == -1) {
    rist_log(p_rist->log, RIST_LOG_ERROR, "could not start rist receiver");
    return -1;
  }

  return 0;
}

C_EXPORT
auto w_rist_set_auth_handler(
    w_rist_ctx p_rist, void *p_arg,
    w_rist_auth_connect_callback p_on_auth_connect_cb,
    w_rist_auth_disconnect_callback p_on_auth_disconnect_cb) -> int {

  if (rist_auth_handler_set(p_rist->ctx, p_on_auth_connect_cb,
                            p_on_auth_disconnect_cb, p_arg) != 0) {

    rist_log(p_rist->log, RIST_LOG_ERROR, "could not init rist auth handler\n");
    return -1;
  }
  return 0;
}

C_EXPORT
auto w_rist_set_conn_status_callback(
    w_rist_ctx p_rist, void *p_arg,
    w_rist_connection_status_callback p_on_connect_status_cb) -> int {
  // register callback for connection status
  if (rist_connection_status_callback_set(p_rist->ctx, p_on_connect_status_cb,
                                          p_arg) == -1) {
    rist_log(p_rist->log, RIST_LOG_ERROR,
             "could not initialize rist connection status callback\n");
    return -1;
  }
  return 0;
}

C_EXPORT
auto w_rist_set_out_of_band_callback(
    w_rist_ctx p_rist, void *p_arg,
    w_rist_out_of_band_callback p_on_out_of_band_cb) -> int {
  if (p_rist->profile == RIST_PROFILE_SIMPLE) {
    return -1;
  }

  // register callback for connection status
  if (rist_oob_callback_set(p_rist->ctx, p_on_out_of_band_cb, p_arg) == -1) {
    rist_log(p_rist->log, RIST_LOG_ERROR,
             "could not add enable cout-of-band data\n");
    return -1;
  }
  return 0;
}

C_EXPORT
auto w_rist_set_stats_callback(w_rist_ctx p_rist, int p_interval, void *p_arg,
                               w_rist_stats_callback p_on_stats_cb) -> int {
  // register callback for connection status
  if (rist_stats_callback_set(p_rist->ctx, p_interval, p_on_stats_cb, p_arg) ==
      -1) {
    rist_log(p_rist->log, RIST_LOG_ERROR,
             "could not add enable stats callback\n");
    return -1;
  }
  return 0;
}

C_EXPORT
auto w_rist_write(w_rist_ctx p_rist, w_rist_data_block p_block)
    -> int {
  return rist_sender_data_write(p_rist->ctx, p_block);
}

C_EXPORT
auto w_rist_read(w_rist_ctx p_rist, w_rist_data_block *p_block,
                            int p_timeout) -> int {
  return rist_receiver_data_read2(p_rist->ctx, p_block, p_timeout);
}

C_EXPORT 
auto w_rist_set_receiver_data_callback(w_rist_ctx p_rist,
                                       w_receiver_data_callback p_on_receive_cb,
                                       void *p_arg) -> int {
  if (rist_receiver_data_callback_set2(p_rist->ctx, p_on_receive_cb, &p_arg) !=
      0) {
    rist_log(p_rist->log, RIST_LOG_ERROR,
             "could not set data_callback pointer\n");
    return -1;
  }
  return 0;
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
    free((*p_rist)->log);
  }

  free(*p_rist);
}

#pragma endregion

} // namespace wolf::stream::rist