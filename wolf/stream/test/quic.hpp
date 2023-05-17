#if defined(WOLF_TEST) && defined(WOLF_STREAM_QUIC)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <stream/quic/w_quic.hpp>

namespace quic = wolf::stream::quic;

struct quic_server_config {
    std::string alpn = "";
    quic::w_settings settings{};
    quic::w_registration_config reg_config;
    quic::w_credential_config cred_config;
};

class quic_server {
    template <typename T>
    using lazy_inited = std::optional<T>;

public:
    static auto make(const quic_server_config& p_server_config)
        -> boost::leaf::result<quic_server>
    {
        auto ret = quic_server();

        BOOST_LEAF_ASSIGN(ret._registration, quic::w_registration::open(p_server_config.reg_config));

        BOOST_LEAF_ASSIGN(ret._configuration, quic::w_configuration::open(
            *ret._registration,
            p_server_config.settings,
            quic::as_alpn_view(p_server_config.alpn),
            p_server_config.cred_config
        ));

        ret._alpn = p_server_config.alpn;

        return ret;
    }

    auto run(std::uint16_t p_port) -> boost::leaf::result<void>
    {
        _stream_cb = [this](quic::w_stream& p_stream, quic::w_stream_event& p_event) -> quic::w_status {
            std::cout << " [quic-server] stream callback: event: " << p_event.name() << std::endl;
            return p_event.visit(wolf::w_overloaded{
                [&](const quic::w_stream_event_start_complete&) {
                    if (auto status = p_stream.send("Hello Client!"); status.failed()) {
                        std::cout << " [quic-client] stream callback: couldn't send on the stream." << std::endl;
                        return quic::w_status_code::Aborted;
                    }
                    return quic::w_status_code::Success;
                },
                [&](const quic::w_stream_event_receive& ev) {
                    for (auto&& buffer : ev.buffers()) {
                        auto str = std::string_view((const char*)buffer.data(), buffer.size());
                        std::cout << " [quic-server] stream: received: " << str << std::endl;
                    }
                    return quic::w_status_code::Success;
                },
                [](const auto&) { return quic::w_status_code::Success; }
            });
        };

        _connection_cb = [this](quic::w_connection& p_conn,
                                quic::w_connection_event& p_event) -> quic::w_status {
            std::cout << " [quic-server] connection callback: event: " << p_event.name() << std::endl;
            return p_event.visit(wolf::w_overloaded{
                [&](const quic::w_connection_event_peer_stream_started& p_ev) {
                    if (p_ev.is_bidirectional()) {
                        auto stream = p_ev.accept_stream(_stream_cb);
                        if (!stream) {
                            std::cout << " [quic-server] connection callback: couldn't accept stream." << std::endl;
                            return quic::w_status_code::Aborted;
                        }
                        if (auto status = stream->send("Hello Client!"); status.failed()) {
                            std::cout << " [quic-client] stream callback: couldn't send on the bidirectional stream." << std::endl;
                            return quic::w_status_code::Aborted;
                        }
                    } else {
                        p_ev.accept_stream(_stream_cb);
                        auto sender_stream = quic::w_stream::open(p_conn,
                                                _stream_cb,
                                                quic::w_stream_open_flag::Unidirectional);
                        if (!sender_stream) {
                            std::cout << " [quic-server] connection callback: couldn't open stream." << std::endl;
                            return quic::w_status_code::Aborted;
                        }

                        if (auto status = sender_stream->start(); status.failed()) {
                            std::cout << " [quic-server] connection callback: couldn't start stream." << std::endl;
                            return quic::w_status_code::Aborted;
                        }
                    }
                    return quic::w_status_code::Success;
                },
                [&](const auto&) { return quic::w_status_code::Success; }
            });
        };

        _listener_cb = [this](quic::w_listener& p_listener,
                              quic::w_listener_event& p_event) -> quic::w_status {
            std::cout << " [quic-server] listener callback: event: " << p_event.name() << std::endl;
            return p_event.visit(wolf::w_overloaded{
                [&](quic::w_listener_event_new_connection&& p_ev) -> quic::w_status {
                    auto res = p_ev.accept_connection(*_configuration, _connection_cb);
                    return res ? quic::w_status_code::Success
                               : quic::w_status_code::Aborted;
                },
                [](const auto&) -> quic::w_status { return quic::w_status_code::Success; }
            });
        };

        BOOST_LEAF_ASSIGN(_listener, quic::w_listener::open(*_registration, _listener_cb));

        auto address = quic::w_address(quic::w_address_family::Unspecified, p_port);

        auto status = _listener->start(address, quic::as_alpn_view(_alpn));
        if (status.failed()) {
            return W_FAILURE(std::errc::operation_canceled,
                             wolf::format("listener didn't start: {}", quic::status_to_str(status)));
        }

        return {};
    }

private:
    quic_server() {}

    std::string _alpn;

    quic::w_listener::callback_type _listener_cb;
    quic::w_connection::callback_type _connection_cb;
    quic::w_stream::callback_type _stream_cb;

    lazy_inited<quic::w_registration> _registration;
    lazy_inited<quic::w_configuration> _configuration;
    lazy_inited<quic::w_listener> _listener;
};

struct quic_client_config {
    std::string alpn = "";
    quic::w_settings settings{};
    quic::w_registration_config reg_config;
    quic::w_credential_config cred_config = quic::w_credential_config(
        quic::w_certificate_none(),
        quic::w_credential_flag::Client
    );
};



class quic_client {
    template <typename T>
    using lazy_inited = std::optional<T>;

public:
    static auto make(const quic_client_config& p_client_config) -> boost::leaf::result<quic_client>
    {
        quic_client ret;

        BOOST_LEAF_ASSIGN(ret._registration, quic::w_registration::open(p_client_config.reg_config));

        auto cred_config = quic::w_credential_config(
            quic::w_certificate_none(),
            wolf::w_flags(quic::w_credential_flag::Client)
            | quic::w_credential_flag::NoCertificationValidation
        );

        BOOST_LEAF_ASSIGN(ret._configuration, quic::w_configuration::open(
            *ret._registration,
            p_client_config.settings,
            quic::as_alpn_view(p_client_config.alpn),
            cred_config
        ));

        ret._alpn = p_client_config.alpn;

        return ret;
    }

    auto run(const char* p_host, std::uint16_t p_port) -> boost::leaf::result<void>
    {
        if (_connection) {
            return W_FAILURE(std::errc::operation_in_progress, "already running.");
        }

        _stream_cb = [](quic::w_stream& p_stream,
                        quic::w_stream_event& p_event) -> quic::w_status {
            std::cout << " [quic-client] stream callback: event: " << p_event.name() << std::endl;
            return p_event.visit(wolf::w_overloaded{
                [&](const quic::w_stream_event_start_complete&) {
                    if (auto status = p_stream.send("Hello Server!"); status.failed()) {
                        std::cout << " [quic-client] stream callback: couldn't send on the stream." << std::endl;
                        return quic::w_status_code::Aborted;
                    }
                    return quic::w_status_code::Success;
                },
                [&](const quic::w_stream_event_receive& ev) {
                    for (auto&& buffer : ev.buffers()) {
                        auto str = std::string_view((const char*)buffer.data(), buffer.size());
                        std::cout << " [quic-client] stream: received: " << str << std::endl;
                    }
                    return quic::w_status_code::Success;
                },
                [](const auto&) { return quic::w_status_code::Success; }
            });
        };

        _connection_cb = [this](quic::w_connection& p_conn,
                                quic::w_connection_event& p_event) -> quic::w_status {
            std::cout << " [quic-client] connection callback: event: " << p_event.name() << std::endl;
            return p_event.visit(wolf::w_overloaded{
                [&](const quic::w_connection_event_connected&) {
                    auto stream = quic::w_stream::open(p_conn,
                                         _stream_cb,
                                         quic::w_stream_open_flag::Unidirectional);
                    if (!stream) {
                        std::cout << " [quic-client] connection callback: couldn't open stream." << std::endl;
                        return quic::w_status_code::Aborted;
                    }

                    if (auto status = stream->start(); status.failed()) {
                        std::cout << " [quic-client] connection callback: couldn't start stream." << std::endl;
                        return quic::w_status_code::Aborted;
                    }

                    return quic::w_status_code::Success;
                },
                [&](const auto&) { return quic::w_status_code::Success; }
            });
        };

        BOOST_LEAF_ASSIGN(_connection, quic::w_connection::open(*_registration, _connection_cb));

        quic::w_status status = _connection->start(*_configuration, p_host, p_port);
        if (status.failed()) {
            auto err_str = wolf::format("couldn't start connection: {}", quic::status_to_str(status));
            return W_FAILURE(std::errc::operation_canceled, std::move(err_str));
        }

        return {};
    }

private:
    quic_client() {}

    std::string _alpn;

    quic::w_connection::callback_type _connection_cb;
    quic::w_stream::callback_type _stream_cb;

    lazy_inited<quic::w_registration> _registration;
    lazy_inited<quic::w_configuration> _configuration;
    lazy_inited<quic::w_connection> _connection;
};

BOOST_AUTO_TEST_CASE(msquic_base_types) {
    BOOST_REQUIRE(quic::w_status(quic::w_status_code::Success).succeeded());
    BOOST_REQUIRE(quic::w_status(quic::w_status_code::Aborted).failed());

    {
        auto status_code = quic::w_status_code::Success;
        auto status = quic::w_status(status_code);
        BOOST_REQUIRE(quic::status_to_str(status_code) == quic::status_to_str(status));
    }
}

BOOST_AUTO_TEST_CASE(msquic_server_client) {
    boost::leaf::try_handle_all(
        []() -> boost::leaf::result<void> {
            std::uint8_t cert_hash[20] = {
                // 5C39C7B31216288A00F74CC11DB5E9D7B3AA421D
                0x5C, 0x39, 0xC7, 0xB3,
                0x12, 0x16, 0x28, 0x8A,
                0x00, 0xF7, 0x4C, 0xC1,
                0x1D, 0xB5, 0xE9, 0xD7,
                0xB3, 0xAA, 0x42, 0x1D
            };
            auto cert = quic::w_certificate_hash(cert_hash);
            auto cred_conf = quic::w_credential_config(cert);

            BOOST_LEAF_AUTO(server, quic_server::make(quic_server_config{
                .alpn = "sample",
                .settings = quic::w_settings{
                    .idle_timeout_ms = 2000,
                    .peer_bidi_stream_count = 1,
                    .peer_unidi_stream_count = 1
                },
                .reg_config = quic::w_registration_config("server"),
                .cred_config = std::move(cred_conf)
            }));

            BOOST_LEAF_CHECK(server.run(1234));

            std::this_thread::sleep_for(std::chrono::seconds(1));

            BOOST_LEAF_AUTO(client, quic_client::make(quic_client_config{
                .alpn = "sample",
                .settings = quic::w_settings{
                    .idle_timeout_ms = 2000,
                    .peer_bidi_stream_count = 1,
                    .peer_unidi_stream_count = 1
                },
                .reg_config = quic::w_registration_config("client")
            }));

            BOOST_LEAF_CHECK(client.run("127.0.0.1", 1234));

            std::this_thread::sleep_for(std::chrono::seconds(1));

            return {};
        },
        [](const w_trace& p_trace) {
            BOOST_ERROR(p_trace.to_string());
        },
        []() {
            BOOST_ERROR("unknown error occured.");
        }
    );
}

#endif  // defined(WOLF_TEST) && defined(WOLF_STREAM_QUIC)
