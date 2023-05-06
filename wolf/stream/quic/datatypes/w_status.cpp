#include "stream/quic/datatypes/w_status.hpp"

namespace wolf::stream::quic {

std::string_view status_to_str(w_status p_status)
{
    auto status = static_cast<QUIC_STATUS>(w_status_code(p_status));

    switch (status) {
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

// namespace internal

}  // namespace wolf::stream::quic
