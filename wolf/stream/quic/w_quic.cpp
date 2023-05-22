#include "wolf/stream/quic/w_quic.hpp"

#include <mutex>

auto foo() -> const QUIC_API_TABLE*
{
    static std::mutex mutex{};
    static const QUIC_API_TABLE* api_table = nullptr;

    std::lock_guard lk{mutex};

    if (!api_table) {
        if (QUIC_FAILED(MsQuicOpen2(&api_table))) {
            throw "fatal error.";
        }
    }

    return api_table;
}
