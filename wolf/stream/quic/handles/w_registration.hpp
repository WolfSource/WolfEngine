#pragma once

#include "stream/quic/internal/common.hpp"
#include "stream/quic/datatypes/common_flags.hpp"
#include "stream/quic/datatypes/w_registration_config.hpp"
#include "system/w_flags.hpp"

#include <boost/leaf.hpp>
#include <msquic.h>

namespace wolf::stream::quic {

/**
 * @brief Wrapper of registration handle of msquic.
 *
 * registration is the execution context of a quic application.
 * all connections, worker threads, internal allocations, etc
 * are controlled by registration.
 *
 * one likely won't need more than one registration in their application.
 *
 * @note make sure that the configuration handle has
 *       a shorter lifetime and closes before this registration,
 *       or there might be deadlock.
 */
class W_API w_registration {
    friend internal::w_raw_access;

public:
    /**
     * @brief use static factory function `open`.
     */
    w_registration() = delete;

    w_registration(const w_registration&) = delete;
    w_registration(w_registration&& p_other) noexcept
        : _handle(std::exchange(p_other._handle, nullptr))
    {}

    w_registration& operator=(const w_registration&) = delete;
    w_registration& operator=(w_registration&& p_other) noexcept
    {
        std::swap(_handle, p_other._handle);
        return *this;
    }

    ~w_registration() { close(); }

    /**
     * @brief open/create a regisration with default registration config.
     */
    [[nodiscard]] static auto open() noexcept -> boost::leaf::result<w_registration>;

    /**
     * @brief open/create a registration with given registration config.
     * @param p_config registration configuration.
     */
    [[nodiscard]] static auto open(const w_registration_config& p_config) noexcept
        -> boost::leaf::result<w_registration>;

    /**
     * @brief shutdown the execution context along with its connections.
     * @param p_flags connection shutdown flags.
     * @param p_error_code error code to send to peers.
     */
    void shutdown(wolf::w_flags<w_connection_shutdown_flag> p_flags = w_connection_shutdown_flag::None,
                  std::size_t p_error_code = 0);

    /**
     * @brief close the registration and free up its memory.
     * @note the associated configuration handle
     *       must be closed before closing the registration.
     */
    void close();

private:
    auto raw() noexcept { return _handle; }
    auto raw() const noexcept { return _handle; }

    explicit w_registration(internal::w_raw_tag, HQUIC p_handle) noexcept
        : _handle(p_handle)
    {}

    HQUIC _handle = nullptr;
};

}  // namespace wolf::stream::quic
