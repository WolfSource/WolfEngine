#pragma once

#include "wolf.hpp"

#include "stream/quic/internal/common.hpp"
#include "stream/quic/datatypes/w_address.hpp"

#include <msquic.h>

#include <cstdint>
#include <string_view>

namespace wolf::stream::quic {

/**
 * @brief Accessor wrapper of QUIC_NEW_CONNECTION_INFO type.
 *
 * This class wraps the raw C type, and provides simple accessor methods.
 */
class w_new_connection_info {
    friend class internal::w_raw_access;

public:
    w_new_connection_info() = delete;

    /**
     * @brief negotiated quic version.
     */
    [[nodiscard]] auto quic_version() const
    {
        return _conn_info.QuicVersion;
    }

    /**
     * @brief the local address of the new incoming connection.
     */
    [[nodiscard]] w_address local_address() const
    {
        return internal::w_raw_access::from_raw<w_address>(*_conn_info.LocalAddress);
    }

    /**
     * @brief the remote address of the new incoming connection.
     */
    [[nodiscard]] w_address remote_address() const
    {
        return internal::w_raw_access::from_raw<w_address>(*_conn_info.RemoteAddress);
    }

    /**
     * @brief the server name (SNI) of the new incoming connection.
     */
    [[nodiscard]] std::string_view server_name() const
    {
        return std::string_view(_conn_info.ServerName, _conn_info.ServerNameLength);
    }

    /**
     * @brief the negotiated ALPN between server and client.
     */
    [[nodiscard]] std::string_view negotiated_alpn() const
    {
        auto c_str = (const char*)(_conn_info.NegotiatedAlpn);
        return std::string_view(c_str, _conn_info.NegotiatedAlpnLength);
    }

private:
    w_new_connection_info(internal::w_raw_tag, const QUIC_NEW_CONNECTION_INFO& p_conn_info)
        : _conn_info(p_conn_info)
    {}

    QUIC_NEW_CONNECTION_INFO _conn_info = { 0 };
};

}  // namespace wolf::stream::quic
