/*
        Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/
#include "rist.h"
#include <gsl/gsl> //NO-LINT
#include <librist/librist.h>
#include <rist-private.h>

struct w_rist_ctx_t
{
    rist_ctx* ctx;
    rist_peer* peer;
    rist_profile profile;
    uint16_t loss_percentage;
    rist_logging_settings* log;
};

int w_rist_init(w_rist_ctx* p_rist, enum rist_ctx_mode p_mode,
    enum rist_profile p_profile, uint16_t p_loss_percentage,
    enum rist_log_level p_log_level,
    w_rist_log_callback p_log_callback)
{
    constexpr auto size = sizeof(w_rist_ctx_t);
    auto* rist = gsl::owner<w_rist_ctx_t*>(calloc(1, size));
    if (rist == nullptr) 
    {
        return W_FAILURE;
    }

    rist->profile = p_profile;
    rist->loss_percentage = p_loss_percentage;

    // create rist context
    rist_ctx* ctx = nullptr;

    switch (p_mode) 
    {
    case rist_ctx_mode::RIST_SENDER_MODE: 
    {
        // create a log
        if (rist_logging_set(&rist->log, p_log_level, p_log_callback,
            (void*)("sender"), nullptr, stderr) != 0) 
        {
            return W_FAILURE;
        }
        // create a sender
        auto ret = rist_sender_create(&ctx, rist->profile, 0, rist->log);
        if (ret != 0) 
        {
            rist_log(rist->log, RIST_LOG_ERROR,
                "could not create rist sender context");
            return W_FAILURE;
        }
        if (p_loss_percentage > 0) 
        {
            ctx->sender_ctx->simulate_loss = true;
            ctx->sender_ctx->loss_percentage = p_loss_percentage * 10;
        }
        break;
    }
    case rist_ctx_mode::RIST_RECEIVER_MODE: 
    {
        // create a log
        if (rist_logging_set(&rist->log, p_log_level, p_log_callback,
            (void*)("receiver"), nullptr, stderr) != 0)
        {
            return W_FAILURE;
        }
        // create a receiver
        auto ret = rist_receiver_create(&ctx, rist->profile, rist->log);
        if (ret != 0)
        {
            rist_log(rist->log, rist_log_level::RIST_LOG_ERROR,
                "could not create rist receiver context");
            return W_FAILURE;
        }
        if (p_loss_percentage > 0)
        {
            ctx->receiver_ctx->simulate_loss = true;
            ctx->receiver_ctx->loss_percentage = p_loss_percentage * 10;
        }

        break;
    }
    };

    rist->ctx = ctx;
    *p_rist = rist;

    return W_SUCCESS;
}

int w_rist_connect(w_rist_ctx p_rist, const char* p_url)
{
    // rely on the library to parse the url
    rist_peer_config* peer_config = nullptr;
    if (rist_parse_address2(p_url, &peer_config)) {
        rist_log(p_rist->log, rist_log_level::RIST_LOG_ERROR,
            "could not parse peer options for receiver");
        return W_FAILURE;
    }

    if (rist_peer_create(p_rist->ctx, &p_rist->peer, peer_config) == -1) {
        rist_log(p_rist->log, rist_log_level::RIST_LOG_ERROR,
            "could not add peer connector to receiver");
        return W_FAILURE;
    }
    free((void*)peer_config);

    if (rist_start(p_rist->ctx) == -1) {
        rist_log(p_rist->log, rist_log_level::RIST_LOG_ERROR,
            "could not start rist receiver");
        return W_FAILURE;
    }

    return W_SUCCESS;
}

int w_rist_set_auth_handler(
    w_rist_ctx p_rist, void* p_arg,
    w_rist_auth_connect_callback p_on_auth_connect_cb,
    w_rist_auth_disconnect_callback p_on_auth_disconnect_cb)
{
    if (rist_auth_handler_set(p_rist->ctx, p_on_auth_connect_cb,
        p_on_auth_disconnect_cb, p_arg) != 0) 
    {
        rist_log(p_rist->log, rist_log_level::RIST_LOG_ERROR,
            "could not init rist auth handler\n");
        return W_FAILURE;
    }
    return W_SUCCESS;
}

int w_rist_set_conn_status_callback(
    w_rist_ctx p_rist, void* p_arg,
    w_rist_connection_status_callback p_on_connect_status_cb)
{
    // register callback for connection status
    if (rist_connection_status_callback_set(p_rist->ctx, p_on_connect_status_cb, p_arg) == -1)
    {
        rist_log(p_rist->log, RIST_LOG_ERROR,
            "could not initialize rist connection status callback\n");
        return W_FAILURE;
    }
    return W_SUCCESS;
}

int w_rist_set_out_of_band_callback(
    w_rist_ctx p_rist, void* p_arg,
    w_rist_out_of_band_callback p_on_out_of_band_cb)
{
    if (p_rist->profile == RIST_PROFILE_SIMPLE)
    {
        return W_FAILURE;
    }

    // register callback for connection status
    if (rist_oob_callback_set(p_rist->ctx, p_on_out_of_band_cb, p_arg) == -1)
    {
        rist_log(p_rist->log, RIST_LOG_ERROR,
            "could not add enable cout-of-band data\n");
        return W_FAILURE;
    }
    return W_SUCCESS;
}

int w_rist_set_stats_callback(w_rist_ctx p_rist, int p_interval, void* p_arg,
    w_rist_stats_callback p_on_stats_cb)
{
    // register callback for connection status
    if (rist_stats_callback_set(p_rist->ctx, p_interval, p_on_stats_cb, p_arg) == -1)
    {
        rist_log(p_rist->log, RIST_LOG_ERROR,
            "could not add enable stats callback\n");
        return W_FAILURE;
    }
    return W_SUCCESS;
}

int w_rist_write(w_rist_ctx p_rist, w_rist_data_block p_block)
{
    return rist_sender_data_write(p_rist->ctx, p_block);
}

int w_rist_read(w_rist_ctx p_rist, w_rist_data_block* p_block, int p_timeout) 
{
    return rist_receiver_data_read2(p_rist->ctx, p_block, p_timeout);
}

int w_rist_set_receiver_data_callback(w_rist_ctx p_rist,
    w_receiver_data_callback p_on_receive_cb,
    void* p_arg)
{
    if (rist_receiver_data_callback_set2(p_rist->ctx, p_on_receive_cb, &p_arg) != 0)
    {
        rist_log(p_rist->log, RIST_LOG_ERROR,
            "could not set data_callback pointer\n");
        return W_FAILURE;
    }
    return W_SUCCESS;
}

void w_rist_fini(w_rist_ctx* p_rist)
{
    if (p_rist == nullptr || (*p_rist) == nullptr)
    {
        return;
    }

    if ((*p_rist)->ctx)
    {
        rist_destroy((*p_rist)->ctx);
    }

    // use free instead of mi_free
    // there is a bug with mi_free on windows
    if ((*p_rist)->log)
    {
        free((*p_rist)->log);
    }

    free(*p_rist);
}