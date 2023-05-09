#include "stream/quic/internal/w_msquic_api.hpp"

#include <exception>

namespace wolf::stream::quic::internal {

std::mutex w_msquic_api::_s_mutex{};
const QUIC_API_TABLE* w_msquic_api::_s_instance = nullptr;

auto w_msquic_api::api() -> const QUIC_API_TABLE*
{
    std::unique_lock lk{_s_mutex};

    if (!_s_instance) {
        QUIC_STATUS status;
        if (QUIC_FAILED(status = MsQuicOpen2(&_s_instance))) {
            throw std::exception("error occured when initializing msquic library.");
        }
    }

    return _s_instance;
}

}  // namespace wolf::stream::quic::internal
