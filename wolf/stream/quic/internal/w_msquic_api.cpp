#include "stream/quic/internal/w_msquic_api.hpp"

#include <exception>

namespace wolf::stream::quic::internal {

static std::mutex g_msquic_api_table_mutex{};
static const QUIC_API_TABLE* g_msquic_api_table_instance = nullptr;

auto w_msquic_api::api() -> const QUIC_API_TABLE*
{
    auto lk = std::unique_lock(g_msquic_api_table_mutex);

    if (!g_msquic_api_table_instance) {
        QUIC_STATUS status;
        if (QUIC_FAILED(status = MsQuicOpen2(&g_msquic_api_table_instance))) {
            throw std::exception("error occured when initializing msquic library.");
        }
    }

    return g_msquic_api_table_instance;
}

}  // namespace wolf::stream::quic::internal
