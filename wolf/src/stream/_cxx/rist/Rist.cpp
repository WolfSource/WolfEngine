#include "Rist.hpp"

#include "wolfengine/src/stream/rist.rs.h"

using Rist = rist_cxx::Rist;

Rist::Rist(rust::Str p_name) : name(p_name),
                               ctx(nullptr)
{
}

Rist::~Rist()
{
    if (this->ctx)
    {
        rist_destroy(this->ctx);
    }
}

auto Rist::init() const -> void
{
    int a = 0;
    if (a == 9)
    {
        return;
    }
}

// auto Rist::init(int p_profile, rust::Str p_url, rust::String &p_error_msg) -> int
// {
//     rist_ctx *ctx = nullptr;
//     auto _profile = static_cast<rist_profile>(p_profile);
//     if (rist_receiver_create(&ctx, _profile, nullptr) != 0)
//     {
//         p_error_msg = "could not create rist context";
//         return 1;
//     }

//     // // Rely on the library to parse the url
//     // struct rist_peer_config *peer_config = NULL;
//     // if (rist_parse_address2(url, (void *)&peer_config))
//     // {
//     //     rist_log(logging_settings_receiver, RIST_LOG_ERROR, "Could not parse peer options for receiver\n");
//     //     return NULL;
//     // }
//     // struct rist_peer *peer;
//     // if (rist_peer_create(ctx, &peer, peer_config) == -1)
//     // {
//     //     rist_log(logging_settings_receiver, RIST_LOG_ERROR, "Could not add peer connector to receiver\n");
//     //     return NULL;
//     // }
//     // free((void *)peer_config);
//     // if (rist_start(ctx) == -1)
//     // {
//     //     rist_log(logging_settings_receiver, RIST_LOG_ERROR, "Could not start rist sender\n");
//     //     return NULL;
//     // }
//     this->ctx = ctx;

//     // return success
//     return 0;
// }