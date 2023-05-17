#pragma once

#include "wolf.hpp"

#include <msquic.h>

#include <mutex>

namespace wolf::stream::quic::internal {

/**
 * @brief singleton msquic api function table provider.
 */
class W_API w_msquic_api {
public:
    w_msquic_api() = delete;

    /**
     * @brief make a valid QUIC_API_TABLE pointer instance.
     */
    static auto api() -> const QUIC_API_TABLE*;
};

}  // namespace wolf::stream::quic::internal
