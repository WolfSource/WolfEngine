/*
        Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rist_data_block.h"

enum rist_log_level : int;
enum rist_ctx_mode : int;
enum rist_profile : int;
enum rist_connection_status : int;

typedef struct rist_peer *w_rist_peer;
typedef const struct rist_oob_block *w_rist_oob_block;
typedef const struct rist_stats *w_rist_stats;
typedef struct w_rist_ctx_t *w_rist_ctx;

// callbacks
typedef int (*w_rist_log_callback)(void * /*p_arg*/,
                                   enum rist_log_level /*p_log_level*/,
                                   const char * /*p_msg*/);

typedef int (*w_rist_auth_connect_callback)(void * /*p_arg*/,
                                            const char * /*p_conn_ip*/,
                                            uint16_t /*p_conn_port*/,
                                            const char * /*p_local_ip*/,
                                            uint16_t /*p_local_port*/,
                                            w_rist_peer /*p_peer*/);

typedef int (*w_rist_auth_disconnect_callback)(void * /*p_arg*/,
                                               w_rist_peer /*p_peer*/);

typedef void (*w_rist_connection_status_callback)(
    void * /*p_arg*/, w_rist_peer /*p_peer*/,
    enum rist_connection_status /* p_peer_connection_status */);

typedef int (*w_rist_out_of_band_callback)(void * /*p_arg*/,
                                           w_rist_oob_block /*oob_block*/);

typedef int (*w_rist_stats_callback)(void * /*p_arg*/,
                                     w_rist_stats /*p_stats_container*/);

typedef int (*w_receiver_data_callback)(void * /*p_arg*/,
                                        w_rist_data_block /*p_data_block*/);

// functions

W_EXPORT
int w_rist_init(w_rist_ctx *p_rist, enum rist_ctx_mode p_mode,
                enum rist_profile p_profile, uint16_t p_loss_percentage,
                enum rist_log_level p_log_level,
                w_rist_log_callback p_log_callback);

 W_EXPORT
 int w_rist_connect(w_rist_ctx p_rist, const char *p_url);

 W_EXPORT
 int w_rist_set_auth_handler(
     w_rist_ctx p_rist, void *p_arg,
     w_rist_auth_connect_callback p_on_auth_connect_cb,
     w_rist_auth_disconnect_callback p_on_auth_disconnect_cb);

 W_EXPORT
 int w_rist_set_conn_status_callback(
     w_rist_ctx p_rist, void *p_arg,
     w_rist_connection_status_callback p_on_connect_status_cb);

 W_EXPORT
 int w_rist_set_out_of_band_callback(
     w_rist_ctx p_rist, void *p_arg,
     w_rist_out_of_band_callback p_on_out_of_band_cb);

 W_EXPORT
 int w_rist_set_stats_callback(w_rist_ctx p_rist, int p_interval, void *p_arg,
                               w_rist_stats_callback p_on_stats_cb);

 W_EXPORT
 int w_rist_write(w_rist_ctx p_rist, w_rist_data_block p_block);

 W_EXPORT
 int w_rist_read(w_rist_ctx p_rist, w_rist_data_block *p_block, int
 p_timeout);

 W_EXPORT
 int w_rist_set_receiver_data_callback(w_rist_ctx p_rist,
                                       w_receiver_data_callback
                                       p_on_receive_cb, void *p_arg);

 W_EXPORT
 void w_rist_fini(w_rist_ctx *p_rist);

#ifdef __cplusplus
}
#endif