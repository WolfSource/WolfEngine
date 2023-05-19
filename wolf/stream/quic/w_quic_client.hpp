#pragma once

#include "stream/quic/datatypes/w_alpn.hpp"
#include "stream/quic/datatypes/w_status.hpp"
#include "stream/quic/datatypes/w_settings.hpp"
#include "stream/quic/datatypes/w_credential_config.hpp"
#include "stream/quic/datatypes/w_registration_config.hpp"
#include "stream/quic/handles/w_configuration.hpp"
#include "stream/quic/handles/w_registration.hpp"
#include "stream/quic/handles/w_connection.hpp"

#include "wolf.hpp"

#include <string>

namespace wolf::stream::quic {

/**
 * @brief client parameters/config bundle.
 */
struct w_quic_client_config {
    std::string alpn = "";
    quic::w_settings settings = quic::w_settings{
        .peer_bidi_stream_count = 1,
        .peer_unidi_stream_count = 1
    };
    quic::w_registration_config reg_config;
    quic::w_credential_config cred_config = quic::w_credential_config(
        quic::w_certificate_none(),
        quic::w_credential_flag::Client
    );
};

/**
 * @brief a simple quic client utility that holds
 *        and manages registration and configuration along with connection.
 */
class w_quic_client {
public:
    w_quic_client(const w_quic_client&) = delete;
    w_quic_client(w_quic_client&&) noexcept = default;

    w_quic_client& operator=(const w_quic_client&&) noexcept = delete;
    w_quic_client& operator=(w_quic_client&&) noexcept = default;

    ~w_quic_client() { stop(); }

    [[nodiscard]] auto& configuration() noexcept { return _configuration; }
    [[nodiscard]] const auto& configuration() const noexcept { return _configuration; }

    [[nodiscard]] auto& registration() noexcept { return _registration; }
    [[nodiscard]] const auto& registration() const noexcept { return _registration; }

    /**
     * @brief make a valid quic client instance.
     * @param p_server_config  client configuration bundle.
     * @return an instance on success.
     */
    [[nodiscard]] static auto make(const w_quic_client_config& p_client_config)
        -> boost::leaf::result<w_quic_client>
    {
        w_quic_client ret;

        BOOST_LEAF_ASSIGN(ret._registration, quic::w_registration::open(p_client_config.reg_config));

        auto cred_config = quic::w_credential_config(
            quic::w_certificate_none(),
            wolf::w_flags(quic::w_credential_flag::Client)
            | quic::w_credential_flag::NoCertificationValidation
        );

        BOOST_LEAF_ASSIGN(ret._configuration, quic::w_configuration::open(
            ret._registration,
            p_client_config.settings,
            quic::as_alpn_view(p_client_config.alpn),
            cred_config
        ));

        ret._alpn = p_client_config.alpn;

        return ret;
    }

    /**
     * @brief run the client to connect on the given host and port with given callback.
     * @return success or failure.
     */
    auto run(const char* p_host, std::uint16_t p_port, w_connection::callback_type p_connection_cb)
        -> boost::leaf::result<void>
    {
        if (_connection.is_running()) {
            return W_FAILURE(std::errc::operation_in_progress, "already running.");
        }

        BOOST_LEAF_ASSIGN(_connection, quic::w_connection::open(_registration, std::move(p_connection_cb)));

        quic::w_status status = _connection.start(_configuration, p_host, p_port);
        if (status.failed()) {
            auto err_str = wolf::format("couldn't start connection: {}", quic::status_to_str(status));
            return W_FAILURE(std::errc::operation_canceled, std::move(err_str));
        }

        return {};
    }

    /**
     * @brief stop the server.
     */
    void stop()
    {
        if (!_connection.is_running()) {
            return;
        }

        _connection.shutdown();
        _connection.close();
    }

private:
    w_quic_client() {}

    std::string _alpn;

    w_registration _registration;
    w_configuration _configuration;
    w_connection _connection;
};

}  // namespace wolf::stream::quic
