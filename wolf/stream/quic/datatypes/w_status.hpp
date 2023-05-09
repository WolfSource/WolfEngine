#pragma once

#include "wolf.hpp"

#include <msquic.h>

#include <string_view>

namespace wolf::stream::quic {

/**
 * @brief equivalent of QUIC_STATUS.
 */
enum class w_status_code {
    Success                   = QUIC_STATUS_SUCCESS,
    Pending                   = QUIC_STATUS_PENDING,
    Continue                  = QUIC_STATUS_CONTINUE,
    Aborted                   = QUIC_STATUS_ABORTED,
    Unreachable               = QUIC_STATUS_UNREACHABLE,
    OutOfMemory               = QUIC_STATUS_OUT_OF_MEMORY,
    NotSupported              = QUIC_STATUS_NOT_SUPPORTED,
    NotFound                  = QUIC_STATUS_NOT_FOUND,
    BufferTooSmall            = QUIC_STATUS_BUFFER_TOO_SMALL,
    UserCancelled             = QUIC_STATUS_USER_CANCELED,
    InvalidParameter          = QUIC_STATUS_INVALID_PARAMETER,
    InvalidState              = QUIC_STATUS_INVALID_STATE,
    InternalError             = QUIC_STATUS_INTERNAL_ERROR,
    HandshakeFailure          = QUIC_STATUS_HANDSHAKE_FAILURE,
    InvalidAddress            = QUIC_STATUS_INVALID_ADDRESS,
    AddressInUse              = QUIC_STATUS_ADDRESS_IN_USE,
    ConnectionTimeout         = QUIC_STATUS_CONNECTION_TIMEOUT,
    ConnectionIdle            = QUIC_STATUS_CONNECTION_IDLE,
    ConnectionRefused         = QUIC_STATUS_CONNECTION_REFUSED,
    ProtocolError             = QUIC_STATUS_PROTOCOL_ERROR,
    VersionNegotiationError   = QUIC_STATUS_VER_NEG_ERROR,
    AlpnNegotiationFailure    = QUIC_STATUS_ALPN_NEG_FAILURE,
    StreamLimitReached        = QUIC_STATUS_STREAM_LIMIT_REACHED,
    TlsError                  = QUIC_STATUS_TLS_ERROR,
    BadCertificate            = QUIC_STATUS_BAD_CERTIFICATE,
    RequiredCertificate       = QUIC_STATUS_REQUIRED_CERTIFICATE,
    RevokedCertificate        = QUIC_STATUS_REVOKED_CERTIFICATE,
    UnknonwCertificate        = QUIC_STATUS_UNKNOWN_CERTIFICATE,
    ExpiredCertificate        = QUIC_STATUS_EXPIRED_CERTIFICATE,
    AlpnInUse                 = QUIC_STATUS_ALPN_IN_USE,
    CertExpired               = QUIC_STATUS_CERT_EXPIRED,
    CertNoCert                = QUIC_STATUS_CERT_NO_CERT,
    CertUntrustedRoot         = QUIC_STATUS_CERT_UNTRUSTED_ROOT
};

/**
 * @brief A helper utility providing characteristics of QUIC_STATUS.
 *
 * Providing helpers such as its QUIC_SUCCEEDED/QUIC_FAILED through
 * succeeded/failed methods, along with implicit bool conversion
 * equivalent to QUIC_SUCCEEDED macro.
 */
class w_status {
public:
    constexpr w_status(QUIC_STATUS p_code) noexcept
        : _status_code(static_cast<w_status_code>(p_code))
    {}

    constexpr w_status(w_status_code p_code) noexcept
        : _status_code(p_code)
    {}

    /**
     * @brief whether status indicates succession or not.
     */
    [[nodiscard]] constexpr bool succeeded() const noexcept
    {
        return QUIC_SUCCEEDED(static_cast<QUIC_STATUS>(_status_code));
    }

    /**
     * @brief whether status indicates failure or not.
     */
    [[nodiscard]] constexpr bool failed() const noexcept
    {
        return QUIC_FAILED(static_cast<QUIC_STATUS>(_status_code));
    }

    constexpr operator bool() const noexcept
    {
        return succeeded();
    }

    constexpr operator w_status_code() const noexcept
    {
        return _status_code;
    }

private:
    w_status_code _status_code = w_status_code::Success;
};

/**
 * @brief convert given status to an equivalent short string.
 */
W_API std::string_view status_to_str(w_status p_status);

}  // namespace wolf::stream::quic
