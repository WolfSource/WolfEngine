#if defined(WOLF_TEST) && defined(WOLF_STREAM_QUIC)

#pragma once

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <stream/quic/w_quic.hpp>

namespace quic = wolf::stream::quic;

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

            BOOST_LEAF_AUTO(server, quic::w_quic_server::make(quic::w_quic_server_config{
                .alpn = "sample",
                .settings = quic::w_settings{
                    .idle_timeout_ms = 2000,
                    .peer_bidi_stream_count = 1,
                    .peer_unidi_stream_count = 1
                },
                .reg_config = quic::w_registration_config("server"),
                .cred_config = std::move(cred_conf)
            }));

            bool server_received = false;

            auto server_stream_cb = [&](quic::w_stream& p_stream,
                                        quic::w_stream_event& p_event) -> quic::w_status {
                std::cout << " [quic-server] stream callback: event: " << p_event.name() << std::endl;
                return p_event.visit(wolf::w_overloaded{
                    [&](const quic::w_stream_event_start_complete&) {
                        auto status = p_stream.send("Hello Client!");
                        BOOST_REQUIRE(status.succeeded());
                        return quic::w_status_code::Success;
                    },
                    [&](const quic::w_stream_event_receive& p_ev) {
                        BOOST_CHECK(p_ev.buffers().size() == 1);
                        for (auto&& buffer : p_ev.buffers()) {
                            auto str = std::string_view((const char*)buffer.data(), buffer.size());
                            std::cout << " [quic-server] stream: received: " << str << std::endl;
                            BOOST_CHECK(str == "Hello Server!");
                        }
                        server_received = true;
                        return quic::w_status_code::Success;
                    },
                    [](const auto&) { return quic::w_status_code::Success; }
                });
            };

            auto server_connection_cb = [&](quic::w_connection& p_conn,
                                            quic::w_connection_event& p_event) -> quic::w_status {
                std::cout << " [quic-server] connection callback: event: " << p_event.name() << std::endl;
                return p_event.visit(wolf::w_overloaded{
                    [&](const quic::w_connection_event_peer_stream_started& p_ev) {
                        if (p_ev.is_bidirectional()) {
                            auto stream = p_ev.accept_stream(server_stream_cb);
                            BOOST_REQUIRE(stream);
                            auto status = stream->send("Hello Client!");
                            BOOST_REQUIRE(status.succeeded());
                        } else {
                            p_ev.accept_stream(server_stream_cb);
                            auto sender_stream = quic::w_stream::open(p_conn,
                                                    server_stream_cb,
                                                    quic::w_stream_open_flag::Unidirectional);
                            BOOST_REQUIRE(sender_stream);
                            auto status = sender_stream->start();
                            BOOST_REQUIRE(status.succeeded());
                        }
                        return quic::w_status_code::Success;
                    },
                    [&](const auto&) { return quic::w_status_code::Success; }
                });
            };

            auto server_listener_cb = [&](quic::w_listener& p_listener,
                                          quic::w_listener_event& p_event) -> quic::w_status {
                std::cout << " [quic-server] listener callback: event: " << p_event.name() << std::endl;
                return p_event.visit(wolf::w_overloaded{
                    [&](quic::w_listener_event_new_connection&& p_ev) -> quic::w_status {
                        auto res = p_ev.accept_connection(server.configuration(), server_connection_cb);
                        BOOST_REQUIRE(res);
                        return res ? quic::w_status_code::Success
                                   : quic::w_status_code::Aborted;
                    },
                    [](const auto&) -> quic::w_status { return quic::w_status_code::Success; }
                });
            };

            BOOST_LEAF_CHECK(server.run(1234, server_listener_cb));

            std::this_thread::sleep_for(std::chrono::seconds(1));

            BOOST_LEAF_AUTO(client, quic::w_quic_client::make(quic::w_quic_client_config{
                .alpn = "sample",
                .settings = quic::w_settings{
                    .idle_timeout_ms = 2000,
                    .peer_bidi_stream_count = 1,
                    .peer_unidi_stream_count = 1
                },
                .reg_config = quic::w_registration_config("client")
            }));

            bool client_received = false;

            auto client_stream_cb = [&](quic::w_stream& p_stream,
                                        quic::w_stream_event& p_event) -> quic::w_status {
                std::cout << " [quic-client] stream callback: event: " << p_event.name() << std::endl;
                return p_event.visit(wolf::w_overloaded{
                    [&](const quic::w_stream_event_start_complete&) {
                        auto status = p_stream.send("Hello Server!");
                        BOOST_REQUIRE(status.succeeded());
                        return quic::w_status_code::Success;
                    },
                    [&](const quic::w_stream_event_receive& p_ev) {
                        BOOST_CHECK(p_ev.buffers().size() == 1);
                        for (auto&& buffer : p_ev.buffers()) {
                            auto str = std::string_view((const char*)buffer.data(), buffer.size());
                            std::cout << " [quic-client] stream: received: " << str << std::endl;
                            BOOST_CHECK(str == "Hello Client!");
                        }
                        client_received = true;
                        return quic::w_status_code::Success;
                    },
                    [](const auto&) { return quic::w_status_code::Success; }
                });
            };

            auto client_connection_cb = [&](quic::w_connection& p_conn,
                                            quic::w_connection_event& p_event) -> quic::w_status {
                std::cout << " [quic-client] connection callback: event: " << p_event.name() << std::endl;
                return p_event.visit(wolf::w_overloaded{
                    [&](const quic::w_connection_event_connected&) {
                        auto stream = quic::w_stream::open(p_conn,
                                             client_stream_cb,
                                             quic::w_stream_open_flag::Unidirectional);
                        BOOST_REQUIRE(stream);

                        auto status = stream->start();
                        BOOST_REQUIRE(status.succeeded());

                        return quic::w_status_code::Success;
                    },
                    [&](const quic::w_connection_event_peer_stream_started& p_ev) {
                        auto stream = p_ev.accept_stream(client_stream_cb);
                        BOOST_REQUIRE(stream);
                        return stream ? quic::w_status_code::Success
                                      : quic::w_status_code::Aborted;
                    },
                    [](const auto&) { return quic::w_status_code::Success; }
                });
            };

            BOOST_LEAF_CHECK(client.run("127.0.0.1", 1234, client_connection_cb));

            std::this_thread::sleep_for(std::chrono::seconds(3));

            BOOST_CHECK(server_received);
            BOOST_CHECK(client_received);

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
