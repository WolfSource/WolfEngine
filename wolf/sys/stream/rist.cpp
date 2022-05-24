// #include "rist.h"
// #include "common.hpp"
// #include <atomic>
// #include <rist-private.h>
// #include <thread>

// #ifdef _MSVC
// #pragma pack(push, _ALIGN_32_)
// #endif
// struct w_rist_t
// {
//     rist_ctx *ctx;
//     w_rist_mode mode;
//     rist_profile profile;
//     int loss_percent;
//     int receive_timeout;
//     std::atomic<bool> stop;
// } _ALIGNMENT_32_;

// auto init_rist_sender(rist_profile p_profile, const char *p_url, w_buf p_trace) -> rist_ctx *
// {
//     auto *trace_buf = reinterpret_cast<char *>(p_trace->data);
//     auto trace_buf_len = p_trace->len;

//     rist_ctx *ctx = nullptr;
//     if (rist_sender_create(&ctx, p_profile, 0, nullptr) != 0)
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "could not create rist sender context\n");
//         return nullptr;
//     }

//     rist_peer_config *peer_config_link = nullptr;
//     if (rist_parse_address2(p_url, &peer_config_link) != 0)
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "could not parse peer options for sender\n");
//         return nullptr;
//     }

//     rist_peer *peer = nullptr;
//     if (rist_peer_create(ctx, &peer, peer_config_link) != 0)
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "could not add peer connector to sender\n");
//         return nullptr;
//     }
//     mi_free(gsl::narrow_cast<void *>(peer_config_link));

//     return ctx;
// }

// auto init_rist_receiver(
//     rist_profile p_profile,
//     const char *p_url,
//     int p_loss_percent,
//     w_buf p_trace) -> rist_ctx *
// {
//     auto *trace_buf = reinterpret_cast<char *>(p_trace->data);
//     auto trace_buf_len = p_trace->len;

//     rist_ctx *ctx = nullptr;
//     int ret = rist_receiver_create(&ctx, p_profile, nullptr);
//     if (ret != 0)
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "could not create rist receiver context\n");
//         return nullptr;
//     }

//     // Rely on the library to parse the url
//     struct rist_peer_config *peer_config = nullptr;
//     if (rist_parse_address2(p_url, &peer_config) != 0)
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "could not parse peer options for receiver url\n");
//         return nullptr;
//     }

//     struct rist_peer *peer = nullptr;
//     if (rist_peer_create(ctx, &peer, peer_config) != 0)
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "could not add peer connector to receiver\n");
//         return nullptr;
//     }
//     mi_free(gsl::narrow_cast<void *>(peer_config));

//     if (p_loss_percent > 0)
//     {
//         ctx->receiver_ctx->simulate_loss = true;
//         const int rate = 10;
//         ctx->receiver_ctx->loss_percentage = p_loss_percent * rate;
//     }

//     return ctx;
// }

// void start_receiving(w_rist p_rist)
// {
//     const auto max_size = 1316;
//     bool got_first = false;
//     std::array<char, max_size> rcompare = {0};
//     int64_t receive_count = 1;
//     rist_data_block *block = nullptr;
// #pragma unroll
//     while (true)
//     {
//         if (p_rist->stop)
//         {
//             break;
//         }

//         int queue_length = rist_receiver_data_read2(p_rist->ctx, &block, p_rist->receive_timeout);
//         if (queue_length > 0)
//         {
//             if (!got_first)
//             {
//                 receive_count = gsl::narrow_cast<int64_t>(block->seq);
//                 got_first = true;
//             }

//             (void)sprintf(rcompare.data(), "DEADBEAF TEST PACKET #%" PRId64 "\n", receive_count);
//             if (strcmp(rcompare.data(), gsl::narrow_cast<const char *>(block->payload)) != 0)
//             {
//                 p_rist->stop = true;
//                 break;
//             }

//             receive_count++;
//             rist_receiver_data_block_free2(&block);
//         }
//     }
// }

// void start_sending(w_rist p_rist)
// {
//     const int max_size = 1316;

//     int send_counter = 0;
//     std::array<char, max_size> buffer = {0};
//     rist_data_block data = {};
//     // we just try to send some string at ~20mbs for ~8 seconds
// #pragma unroll
//     while (true)
//     {
//         if (p_rist->stop)
//         {
//             break;
//         }
//         (void)sprintf(buffer.data(), "DEADBEAF TEST PACKET #%i", send_counter);
//         data.payload = &buffer;
//         data.payload_len = max_size;
//         int len = rist_sender_data_write(p_rist->ctx, &data);
//         if (len < 0)
//         {
//             // fprintf(stderr, "Failed to send test packet with error code %d!\n", ret);
//             p_rist->stop = true;
//             break;
//         }
//         if (len != gsl::narrow_cast<int>(data.payload_len))
//         {
//             // fprintf(stderr, "Failed to send test packet %d != %d !\n", ret, (int)data.payload_len);
//             p_rist->stop = true;
//             break;
//         }
//         send_counter++;

//         std::this_thread::sleep_for(std::chrono::milliseconds(1));
//     }

//     std::this_thread::sleep_for(std::chrono::milliseconds(1));
// }

// int w_rist_bind(w_rist *p_rist,
//                 const char *p_url,
//                 w_rist_config p_config,
//                 w_buf p_trace)
// {
//     if (p_config == nullptr || !is_valid_buffer(p_trace))
//     {
//         return 1;
//     }

//     auto *trace_buf = reinterpret_cast<char *>(p_trace->data);
//     auto trace_buf_len = p_trace->len;

//     auto *rist = gsl::narrow_cast<w_rist>(mi_malloc(sizeof(w_rist_t)));
//     if (rist == nullptr)
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "could not allocate memory for w_rist object");
//         return -1;
//     }
//     rist->mode = p_config->mode;
//     rist->profile = gsl::narrow_cast<rist_profile>(p_config->profile);
//     rist->loss_percent = p_config->loss_percent;
//     rist->receive_timeout = p_config->timeout;
//     rist->stop = false;

//     switch (rist->mode)
//     {
//     case w_rist_mode::W_RIST_MODE_RECEIVER:
//     {
//         // setup rist
//         rist->ctx = init_rist_receiver(
//             rist->profile,
//             p_url, p_config->loss_percent,
//             p_trace);
//         break;
//     }
//     case w_rist_mode::W_RIST_MODE_SENDER:
//     {
//         rist->ctx = init_rist_sender(
//             rist->profile,
//             p_url,
//             p_trace);
//         break;
//     }
//     default:
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "undefined w_rist_mode");
//         return -1;
//     }
//     };

//     *p_rist = rist;

//     return 0;
// }

// int w_rist_start(w_rist p_rist, w_buf p_trace)
// {
//     if (p_rist == nullptr || p_rist->ctx == nullptr || !is_valid_buffer(p_trace))
//     {
//         return 1;
//     }

//     auto *trace_buf = reinterpret_cast<char *>(p_trace->data);
//     auto trace_buf_len = p_trace->len;

//     // start rist
//     int ret = rist_start(p_rist->ctx);
//     if (ret == -1)
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "could not start rist");
//         return -1;
//     }

//     switch (p_rist->mode)
//     {
//     case w_rist_mode::W_RIST_MODE_RECEIVER:
//     {
//         start_receiving(p_rist);
//         break;
//     }
//     case w_rist_mode::W_RIST_MODE_SENDER:
//     {
//         start_sending(p_rist);
//         break;
//     }
//     default:
//     {
//         (void)snprintf(trace_buf,
//                        trace_buf_len,
//                        "undefined w_rist_mode");
//         return -1;
//     }
//     };

//     // reset state
//     p_rist->stop = false;

//     return 0;
// }

// int w_rist_stop(w_rist p_rist)
// {
//     if (p_rist == nullptr || p_rist->ctx == nullptr)
//     {
//         return 1;
//     }
//     p_rist->stop = true;
//     return 0;
// }

// bool w_rist_is_stopped(w_rist p_rist)
// {
//     return p_rist->stop;
// }

// int w_rist_fini(w_rist *p_rist)
// {
//     if (p_rist == nullptr || *p_rist == nullptr || (*p_rist)->ctx == nullptr)
//     {
//         return 1;
//     }
//     return rist_destroy((*p_rist)->ctx);
// }