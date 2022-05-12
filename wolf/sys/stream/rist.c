#include "rist.h"
#include <rist-private.h>
#include <mimalloc.h>

#ifdef _MSVC
#pragma pack(push, _ALIGNMENT_)
#endif
typedef struct w_rist_t
{
    struct rist_ctx *ctx;
    w_rist_mode mode;
    int loss_percent;
} w_rist_t __attribute__((aligned(_ALIGNMENT_)));

struct rist_ctx *init_rist_receiver(
    int p_profile,
    const char *p_url,
    int p_loss_percent,
    w_buf p_trace)
{
    struct rist_ctx *ctx = NULL;
    int ret = rist_receiver_create(&ctx, p_profile, NULL);
    if (ret != 0)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not create rist receiver context");
        return NULL;
    }

    // Rely on the library to parse the url
    struct rist_peer_config *peer_config = NULL;
    if (rist_parse_address2(p_url, (void *)&peer_config))
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not parse peer options for receiver url");
        return NULL;
    }

    struct rist_peer *peer = NULL;
    if (rist_peer_create(ctx, &peer, peer_config) == -1)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not add peer connector to receiver");
        return NULL;
    }
    free((void *)peer_config);

    if (p_loss_percent > 0)
    {
        ctx->receiver_ctx->simulate_loss = true;
        ctx->receiver_ctx->loss_percentage = p_loss_percent * 10;
    }

    return ctx;
}

int w_rist_init(w_rist *p_rist,
                w_rist_mode p_mode,
                int p_profile,
                const char *p_url,
                int p_loss_percent,
                w_buf p_trace)
{
    if (is_buf_valid(p_trace) != 0)
    {
        return 1;
    }

    w_rist rist = (w_rist)mi_malloc(sizeof(w_rist_t));
    if (rist == NULL)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not allocate memory for w_rist object");
        return -1;
    }
    rist->loss_percent = p_loss_percent;

    switch (p_mode)
    {
    case w_rist_mode_receiver:
    {
        // setup rist
        rist->ctx = init_rist_receiver(p_profile, p_url, p_loss_percent, p_trace);
        break;
    }
    case w_rist_mode_sender:
    {
        break;
    }
    default:
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "undefined w_rist_mode");
        return -1;
    }
    };

    *p_rist = rist;

    return 0;
}

int w_rist_start(w_rist p_rist, w_buf p_trace)
{
    if (p_rist == NULL || p_rist->ctx == NULL || is_buf_valid(p_trace) != 0)
    {
        return 1;
    }

    // start rist
    int ret = rist_start(p_rist->ctx);
    if (ret == -1)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not start rist");
        return -1;
    }

    return 0;
}

int w_rist_stop(w_rist p_rist, w_buf p_trace)
{
    if (p_rist == NULL || p_rist->ctx == NULL || is_buf_valid(p_trace) != 0)
    {
        return 1;
    }
    return 0;
}

int w_rist_fini(w_rist *p_rist)
{
    if (p_rist == NULL || *p_rist == NULL || (*p_rist)->ctx == NULL)
    {
        return 1;
    }
    return rist_destroy((*p_rist)->ctx);
}