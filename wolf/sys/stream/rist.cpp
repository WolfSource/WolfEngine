#include "rist.h"
#include <rist-private.h>
#include <mimalloc.h>
#include <stdatomic.h>

#ifdef _MSVC
#pragma pack(push, _ALIGN_32_)
#endif
typedef struct w_rist_t
{
    struct rist_ctx *ctx;
    w_rist_mode mode;
    int loss_percent;
    int receive_timeout;
    // set 1 to stop loop
    atomic_bool stop;
} w_rist_t _ALIGNMENT_32_;

struct rist_ctx *init_rist_sender(int p_profile, const char *p_url, w_buf p_trace)
{
    struct rist_ctx *ctx = NULL;
    if (rist_sender_create(&ctx, p_profile, 0, NULL) != 0)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not create rist sender context\n");
        return NULL;
    }

    const struct rist_peer_config *peer_config_link = NULL;
    if (rist_parse_address2(p_url, (void *)&peer_config_link))
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not parse peer options for sender\n");
        return NULL;
    }

    struct rist_peer *peer = NULL;
    if (rist_peer_create(ctx, &peer, peer_config_link) == -1)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not add peer connector to sender\n");
        return NULL;
    }
    mi_free((void *)peer_config_link);

    return ctx;
}

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
                       "could not create rist receiver context\n");
        return NULL;
    }

    // Rely on the library to parse the url
    struct rist_peer_config *peer_config = NULL;
    if (rist_parse_address2(p_url, (void *)&peer_config))
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not parse peer options for receiver url\n");
        return NULL;
    }

    struct rist_peer *peer = NULL;
    if (rist_peer_create(ctx, &peer, peer_config) == -1)
    {
        (void)snprintf((char *)p_trace->data,
                       p_trace->len,
                       "could not add peer connector to receiver\n");
        return NULL;
    }
    mi_free((void *)peer_config);

    if (p_loss_percent > 0)
    {
        ctx->receiver_ctx->simulate_loss = true;
        const int rate = 10;
        ctx->receiver_ctx->loss_percentage = p_loss_percent * rate;
    }

    return ctx;
}

void start_receiving(w_rist p_rist)
{
    bool got_first = false;
    int receive_count = 1;
    struct rist_data_block *block = NULL;
#pragma unroll
    while (true)
    {
        if (atomic_load(&p_rist->stop))
        {
            break;
        }

        int queue_length = rist_receiver_data_read2(p_rist->ctx, &block, p_rist->receive_timeout);
        if (queue_length > 0)
        {
            if (!got_first)
            {
                receive_count = (int)block->seq;
                got_first = true;
            }

            receive_count++;
            rist_receiver_data_block_free2((struct rist_data_block * *const)&block);
        }
    }
}

void start_sending(w_rist p_rist)
{
    const int64_t one_miliseconds = 1000;
    const int max_size = 1316;

    int send_counter = 0;
    char buffer[max_size] = {0};
    struct rist_data_block data = {0};
    // we just try to send some string at ~20mbs for ~8 seconds
#pragma unroll
    while (true)
    {
        if (atomic_load(&p_rist->stop))
        {
            break;
        }
        sprintf(buffer, "DEADBEAF TEST PACKET #%i", send_counter);
        data.payload = &buffer;
        data.payload_len = max_size;
        int ret = rist_sender_data_write(p_rist->ctx, &data);
        if (ret < 0)
        {
            // fprintf(stderr, "Failed to send test packet with error code %d!\n", ret);
            // atomic_store(&failed, 1);
            atomic_store(&p_rist->stop, 1);
            break;
        }
        if (ret != (int)data.payload_len)
        {
            // fprintf(stderr, "Failed to send test packet %d != %d !\n", ret, (int)data.payload_len);
            // atomic_store(&failed, 1);
            atomic_store(&p_rist->stop, 1);
            break;
        }
        send_counter++;

        w_sleep(one_miliseconds);
    }

    w_sleep(one_miliseconds);
}

int w_rist_init(w_rist *p_rist,
                const char *p_url,
                w_rist_config p_config,
                w_buf p_trace)
{
    if (is_buf_valid(p_trace) != 0 || p_config == NULL)
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
    rist->loss_percent = p_config->loss_percent;
    rist->receive_timeout = p_config->timeout;
    atomic_store(&rist->stop, 0);

    switch (p_config->mode)
    {
    case w_rist_mode_receiver:
    {
        // setup rist
        rist->ctx = init_rist_receiver(p_config->profile, p_url, p_config->loss_percent, p_trace);
        break;
    }
    case w_rist_mode_sender:
    {
        rist->ctx = init_rist_sender(p_config->profile, p_url, p_trace);
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

    switch (p_rist->mode)
    {
    case w_rist_mode_receiver:
    {
        start_receiving(p_rist);
        break;
    }
    case w_rist_mode_sender:
    {
        start_sending(p_rist);
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

    // rest state
    atomic_store(&p_rist->stop, 0);
    return 0;
}

int w_rist_stop(w_rist p_rist, w_buf p_trace)
{
    if (p_rist == NULL || p_rist->ctx == NULL || is_buf_valid(p_trace) != 0)
    {
        return 1;
    }
    atomic_store(&p_rist->stop, 1);

    return 0;
}

bool w_rist_is_stopped(w_rist p_rist)
{
    return (atomic_load(&p_rist->stop) == 0);
}

int w_rist_fini(w_rist *p_rist)
{
    if (p_rist == NULL || *p_rist == NULL || (*p_rist)->ctx == NULL)
    {
        return 1;
    }
    return rist_destroy((*p_rist)->ctx);
}