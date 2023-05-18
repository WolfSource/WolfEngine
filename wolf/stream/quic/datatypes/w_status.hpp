#pragma once

#include <msquic.h>

#include "wolf.hpp"

#include <string_view>

namespace wolf::stream::quic {

namespace internal {

inline const char* status_to_str(QUIC_STATUS p_status)
{
    switch (p_status) {
    case QUIC_STATUS_SUCCESS:                 return "success";
    case QUIC_STATUS_PENDING:                 return "pending";
    case QUIC_STATUS_CONTINUE:                return "continue";
    case QUIC_STATUS_ABORTED:                 return "aborted";
    case QUIC_STATUS_UNREACHABLE:             return "unreachable";
    case QUIC_STATUS_OUT_OF_MEMORY:           return "out-of-memory";
    case QUIC_STATUS_NOT_SUPPORTED:           return "not-supported";
    case QUIC_STATUS_NOT_FOUND:               return "not-found";
    case QUIC_STATUS_BUFFER_TOO_SMALL:        return "buffer-too-small";
    case QUIC_STATUS_USER_CANCELED:           return "user-canceled";
    case QUIC_STATUS_INVALID_PARAMETER:       return "invalid-parameter";
    case QUIC_STATUS_INVALID_STATE:           return "invalid-state";
    case QUIC_STATUS_INTERNAL_ERROR:          return "internal-error";
    case QUIC_STATUS_HANDSHAKE_FAILURE:       return "handshake-failure";
    case QUIC_STATUS_INVALID_ADDRESS:         return "invalid-address";
    case QUIC_STATUS_ADDRESS_IN_USE:          return "address-in-use";
    case QUIC_STATUS_CONNECTION_TIMEOUT:      return "connection-timeout";
    case QUIC_STATUS_CONNECTION_IDLE:         return "connection-idle";
    case QUIC_STATUS_CONNECTION_REFUSED:      return "connection-refused";
    case QUIC_STATUS_PROTOCOL_ERROR:          return "protocol-error";
    case QUIC_STATUS_VER_NEG_ERROR:           return "version-negotiation-error";
    case QUIC_STATUS_ALPN_NEG_FAILURE:        return "alpn-negotiation-error";
    case QUIC_STATUS_STREAM_LIMIT_REACHED:    return "stream-limit-reached";
    case QUIC_STATUS_TLS_ERROR:               return "tls-error";
    case QUIC_STATUS_BAD_CERTIFICATE:         return "bad-certificate";
    case QUIC_STATUS_REQUIRED_CERTIFICATE:    return "required-certificate";
    case QUIC_STATUS_REVOKED_CERTIFICATE:     return "revoked-certificate";
    case QUIC_STATUS_UNKNOWN_CERTIFICATE:     return "unkown-certificate";
    case QUIC_STATUS_EXPIRED_CERTIFICATE:     return "expired-certificate";
    case QUIC_STATUS_ALPN_IN_USE:             return "alpn-in-use";
    case QUIC_STATUS_CERT_EXPIRED:            return "cert-expired";
    case QUIC_STATUS_CERT_NO_CERT:            return "no-cert";
    case QUIC_STATUS_CERT_UNTRUSTED_ROOT:     return "cert-untrusted-root";
    default:
        return "unknown";
    }
}

}  // namespace internal

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
class W_API w_status {
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
 * @brief convert given status code to an equivalent short string.
 */
inline std::string_view status_to_str(w_status_code p_status_code)
{
    return internal::status_to_str(static_cast<QUIC_STATUS>(p_status_code));
}

/**
 * @brief convert given status to an equivalent short string.
 */
inline std::string_view status_to_str(w_status p_status)
{
    return internal::status_to_str(static_cast<QUIC_STATUS>(w_status_code(p_status)));
}

}  // namespace wolf::stream::quic
