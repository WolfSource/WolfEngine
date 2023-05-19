#pragma once

#include "stream/quic/datatypes/w_alpn.hpp"
#include "stream/quic/datatypes/w_status.hpp"
#include "stream/quic/datatypes/w_settings.hpp"
#include "stream/quic/datatypes/w_credential_config.hpp"
#include "stream/quic/datatypes/w_registration_config.hpp"
#include "stream/quic/handles/w_configuration.hpp"
#include "stream/quic/handles/w_registration.hpp"
#include "stream/quic/handles/w_listener.hpp"

#include "wolf.hpp"

#include <string>
#include <optional>

namespace wolf::stream::quic {

/**
 * @brief server parameters/config bundle.
 */
struct w_quic_server_config {
    std::string alpn = "";
    w_settings settings{};
    w_registration_config reg_config;
    w_credential_config cred_config;
};

/**
 * @brief a simple quic server utility that holds
 *        and manages registration and configuration along with listener.
 */
class w_quic_server {
public:
    w_quic_server(const w_quic_server&) = delete;
    w_quic_server(w_quic_server&&) noexcept = default;

    w_quic_server& operator=(const w_quic_server&&) noexcept = delete;
    w_quic_server& operator=(w_quic_server&&) noexcept = default;

    ~w_quic_server() { stop(); }

    [[nodiscard]] auto& configuration() noexcept { return _configuration; }
    [[nodiscard]] const auto& configuration() const noexcept { return _configuration; }

    [[nodiscard]] auto& registration() noexcept { return _registration; }
    [[nodiscard]] const auto& registration() const noexcept { return _registration; }

    /**
     * @brief make a valid quic server instance.
     * @param p_server_config  server configuration bundle.
     * @return an instance on success.
     */
    static auto make(const w_quic_server_config& p_server_config)
        -> boost::leaf::result<w_quic_server>
    {
        auto ret = w_quic_server();

        BOOST_LEAF_ASSIGN(ret._registration, w_registration::open(p_server_config.reg_config));

        BOOST_LEAF_ASSIGN(ret._configuration, w_configuration::open(
            ret._registration,
            p_server_config.settings,
            as_alpn_view(p_server_config.alpn),
            p_server_config.cred_config
        ));

        ret._alpn = p_server_config.alpn;

        return ret;
    }

    /**
     * @brief run the server to listen on the given port with given callback.
     * @return success or failure.
     */
    auto run(std::uint16_t p_port, w_listener::callback_type p_listener_cb) -> boost::leaf::result<void>
    {
        if (_listener.is_valid()) {
            stop();
        }

        BOOST_LEAF_ASSIGN(_listener, w_listener::open(_registration, std::move(p_listener_cb)));

        auto address = w_address(w_address_family::Unspecified, p_port);

        auto status = _listener.start(address, as_alpn_view(_alpn));
        if (status.failed()) {
            return W_FAILURE(std::errc::operation_canceled,
                             wolf::format("listener didn't start: {}", status_to_str(status)));
        }

        return {};
    }

    /**
     * @brief stop the server.
     */
    void stop()
    {
        if (!_listener.is_valid()) {
            return;
        }

        _listener.stop();
        _listener.close();
    }

private:
    w_quic_server() {}

    std::string _alpn;

    w_registration _registration;
    w_configuration _configuration;
    w_listener _listener;
};

}  // namespace wolf::stream::quic
