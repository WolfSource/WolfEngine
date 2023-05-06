#pragma once

#include "wolf.hpp"

#include "stream/quic/internal/common.hpp"

#include <msquic.h>

#include <cstdint>
#include <memory>

namespace wolf::stream::quic {

/**
 * @brief equivalent of QUIC_ADDRESS_FAMILY enum.
 */
enum class w_address_family {
    Unspecified = QUIC_ADDRESS_FAMILY_UNSPEC,
    INET = QUIC_ADDRESS_FAMILY_INET,
    INET6 = QUIC_ADDRESS_FAMILY_INET6
};

/**
 * @brief Wrapping QUIC_ADDRESS.
 *
 * Commonly used to open a listener.
 */
class w_address {
    friend class internal::w_raw_access;

public:
    w_address(w_address_family p_address_family, std::uint16_t p_port) noexcept
    {
        set_address_family(p_address_family);
        set_port(p_port);
    }

    /**
     * @brief get address family.
     */
    [[nodiscard]] w_address_family get_address_family() const noexcept
    {
        return static_cast<w_address_family>(QuicAddrGetFamily(&_address));
    }

    /**
     * @brief set address family.
     */
    void set_address_family(w_address_family p_address_family) noexcept
    {
        QuicAddrSetFamily(&_address, static_cast<QUIC_ADDRESS_FAMILY>(p_address_family));
    }

    /**
     * @brief get port.
     */
    [[nodiscard]] std::uint16_t get_port() const noexcept
    {
        return QuicAddrGetPort(&_address);
    }

    /**
     * @brief set port.
     */
    void set_port(std::uint16_t p_port) noexcept
    {
        QuicAddrSetPort(&_address, p_port);
    }

private:
    auto raw() noexcept { return &_address; }
    auto raw() const noexcept { return &_address; }

    w_address(internal::w_raw_tag, const QUIC_ADDR& p_raw_addr)
    {
        std::memcpy(&_address, &p_raw_addr, sizeof(QUIC_ADDR));
    }

    QUIC_ADDR _address;
};

}  // namespace wolf::stream::quic
