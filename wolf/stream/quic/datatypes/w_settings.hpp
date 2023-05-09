#pragma once

#include "wolf.hpp"

#include "stream/quic/internal/common.hpp"

#include <msquic.h>

#include <optional>
#include <memory>

namespace wolf::stream::quic {

/**
 * @brief Equivalent enum of QUIC_SERVER_RESUMPTION_LEVEL.
 *
 * Its values are defined in QUIC_SETTINGS document in msquic.
 */
enum class w_server_resumption_level {
    NoResume = QUIC_SERVER_NO_RESUME,
    ResumeOnly = QUIC_SERVER_RESUME_ONLY,
    ResumeAndZeroRTT = QUIC_SERVER_RESUME_AND_ZERORTT
};

/**
 * @brief Equivalent enum of QUIC_CONGESTION_CONTROL_ALGORITHM.
 */
enum class w_congestion_control_algorithm {
    Cubic = QUIC_CONGESTION_CONTROL_ALGORITHM_CUBIC,
    Max = QUIC_CONGESTION_CONTROL_ALGORITHM_MAX
};

/**
 * @brief Wrapper of QUIC_SETTINGS.
 *
 * QUIC_SETTINGS has a set of fields to tweak
 * application's protocol behavior.
 *
 * A use-case of it to setup the configuration handle
 * for registration execution context.
 *
 * For each field in QUIC_SETTINGS, an equivalent
 * std::optional wrapped typed field has been defined.
 *
 * This type has and should be kept with support of aggregate initialization.
 */
class w_settings {
    friend class internal::w_raw_access;

public:
    std::optional<std::uint64_t>                  max_bytes_per_key;
    std::optional<std::uint64_t>                  handshake_idle_timeout_ms;
    std::optional<std::uint64_t>                  idle_timeout_ms;
    std::optional<std::uint64_t>                  mtu_discovery_search_complete_timeout_us;
    std::optional<std::uint32_t>                  tls_client_max_send_buffer;
    std::optional<std::uint32_t>                  tls_server_max_send_buffer;
    std::optional<std::uint32_t>                  stream_recv_window_default;
    std::optional<std::uint32_t>                  stream_recv_buffer_default;
    std::optional<std::uint32_t>                  conn_flow_control_window;
    std::optional<std::uint32_t>                  max_worker_queue_delay_us;
    std::optional<std::uint32_t>                  max_stateless_operations;
    std::optional<std::uint32_t>                  initial_window_packets;
    std::optional<std::uint32_t>                  send_idle_timeout_ms;
    std::optional<std::uint32_t>                  initial_rtt_ms;
    std::optional<std::uint32_t>                  max_ack_delay_ms;
    std::optional<std::uint32_t>                  disconnect_timeout_ms;
    std::optional<std::uint32_t>                  keep_alive_interval_ms;
    std::optional<w_congestion_control_algorithm> congestion_control_algorithm;
    std::optional<std::uint16_t>                  peer_bidi_stream_count;
    std::optional<std::uint16_t>                  peer_unidi_stream_count;
    std::optional<std::uint16_t>                  max_binding_stateless_operations;
    std::optional<std::uint16_t>                  stateless_operation_expiration_ms;
    std::optional<std::uint16_t>                  minimum_mtu;
    std::optional<std::uint16_t>                  maximum_mtu;
    std::optional<bool>                           send_buffering_enabled;
    std::optional<bool>                           pacing_enabled;
    std::optional<bool>                           migration_enabled;
    std::optional<bool>                           datagram_receive_enabled;
    std::optional<w_server_resumption_level>      server_resumption_level;
    std::optional<bool>                           grease_quic_bit_enabled;
    std::optional<bool>                           ecn_enabled;
    std::optional<std::uint8_t>                   max_operations_per_drain;
    std::optional<std::uint8_t>                   mtu_discovery_missing_probe_count;
    std::optional<std::uint32_t>                  dest_cid_update_idle_timeout_ms;

private:
    auto raw() const noexcept
    {
        QUIC_SETTINGS ret;
        std::memset(&ret, 0, sizeof(ret));

        // NOTE the default value used by `value_or` is irrelevant to settings
        //      as if there isn't a value, then the `IsSet` correspondant flag will be false too.

        ret.MaxBytesPerKey = max_bytes_per_key.value_or(0);
        ret.IsSet.MaxBytesPerKey = max_bytes_per_key.has_value();

        ret.HandshakeIdleTimeoutMs = handshake_idle_timeout_ms.value_or(0);
        ret.IsSet.HandshakeIdleTimeoutMs = handshake_idle_timeout_ms.has_value();

        ret.IdleTimeoutMs = idle_timeout_ms.value_or(0);
        ret.IsSet.IdleTimeoutMs = idle_timeout_ms.has_value();

        ret.MtuDiscoverySearchCompleteTimeoutUs =
                mtu_discovery_search_complete_timeout_us.value_or(0);
        ret.IsSet.MtuDiscoverySearchCompleteTimeoutUs =
                mtu_discovery_search_complete_timeout_us.has_value();

        ret.TlsClientMaxSendBuffer = tls_client_max_send_buffer.value_or(0);
        ret.IsSet.TlsClientMaxSendBuffer = tls_client_max_send_buffer.has_value();

        ret.TlsServerMaxSendBuffer = tls_server_max_send_buffer.value_or(0);
        ret.IsSet.TlsServerMaxSendBuffer = tls_server_max_send_buffer.has_value();

        ret.StreamRecvWindowDefault = stream_recv_window_default.value_or(0);
        ret.IsSet.StreamRecvWindowDefault = stream_recv_window_default.has_value();

        ret.StreamRecvBufferDefault = stream_recv_buffer_default.value_or(0);
        ret.IsSet.StreamRecvBufferDefault = stream_recv_buffer_default.has_value();

        ret.ConnFlowControlWindow = conn_flow_control_window.value_or(0);
        ret.IsSet.ConnFlowControlWindow = conn_flow_control_window.has_value();

        ret.MaxWorkerQueueDelayUs = max_worker_queue_delay_us.value_or(0);
        ret.IsSet.MaxWorkerQueueDelayUs = max_worker_queue_delay_us.has_value();

        ret.MaxStatelessOperations = max_stateless_operations.value_or(0);
        ret.IsSet.MaxStatelessOperations = max_stateless_operations.has_value();

        ret.InitialWindowPackets = initial_window_packets.value_or(0);
        ret.IsSet.InitialWindowPackets = initial_window_packets.has_value();

        ret.SendIdleTimeoutMs = send_idle_timeout_ms.value_or(0);
        ret.IsSet.SendIdleTimeoutMs = send_idle_timeout_ms.has_value();

        ret.InitialRttMs = initial_rtt_ms.value_or(0);
        ret.IsSet.InitialRttMs = initial_rtt_ms.has_value();

        ret.MaxAckDelayMs = max_ack_delay_ms.value_or(0);
        ret.IsSet.MaxAckDelayMs = max_ack_delay_ms.has_value();

        ret.DisconnectTimeoutMs = disconnect_timeout_ms.value_or(0);
        ret.IsSet.DisconnectTimeoutMs = disconnect_timeout_ms.has_value();

        ret.KeepAliveIntervalMs = keep_alive_interval_ms.value_or(0);
        ret.IsSet.KeepAliveIntervalMs = keep_alive_interval_ms.has_value();

        ret.CongestionControlAlgorithm = static_cast<QUIC_CONGESTION_CONTROL_ALGORITHM>(
            congestion_control_algorithm.value_or(w_congestion_control_algorithm::Cubic)
        );
        ret.IsSet.CongestionControlAlgorithm = congestion_control_algorithm.has_value();

        ret.PeerBidiStreamCount = peer_bidi_stream_count.value_or(0);
        ret.IsSet.PeerBidiStreamCount = peer_bidi_stream_count.has_value();

        ret.PeerUnidiStreamCount = peer_unidi_stream_count.value_or(0);
        ret.IsSet.PeerUnidiStreamCount = peer_unidi_stream_count.has_value();

        ret.MaxBindingStatelessOperations = max_binding_stateless_operations.value_or(0);
        ret.IsSet.MaxBindingStatelessOperations = max_binding_stateless_operations.has_value();

        ret.StatelessOperationExpirationMs = stateless_operation_expiration_ms.value_or(0);
        ret.IsSet.StatelessOperationExpirationMs = stateless_operation_expiration_ms.has_value();

        ret.MinimumMtu = minimum_mtu.value_or(0);
        ret.IsSet.MinimumMtu = minimum_mtu.has_value();

        ret.MaximumMtu = maximum_mtu.value_or(0);
        ret.IsSet.MaximumMtu = maximum_mtu.has_value();

        ret.SendBufferingEnabled = send_buffering_enabled.value_or(0);
        ret.IsSet.SendBufferingEnabled = send_buffering_enabled.has_value();

        ret.PacingEnabled = pacing_enabled.value_or(0);
        ret.IsSet.PacingEnabled = pacing_enabled.has_value();

        ret.MigrationEnabled = migration_enabled.value_or(0);
        ret.IsSet.MigrationEnabled = migration_enabled.has_value();

        ret.DatagramReceiveEnabled = datagram_receive_enabled.value_or(0);
        ret.IsSet.DatagramReceiveEnabled = datagram_receive_enabled.has_value();

        ret.ServerResumptionLevel = static_cast<QUIC_SERVER_RESUMPTION_LEVEL>(
            server_resumption_level.value_or(w_server_resumption_level::NoResume)
        );
        ret.IsSet.ServerResumptionLevel = server_resumption_level.has_value();

        ret.GreaseQuicBitEnabled = grease_quic_bit_enabled.value_or(0);
        ret.IsSet.GreaseQuicBitEnabled = grease_quic_bit_enabled.has_value();

        ret.EcnEnabled = ecn_enabled.value_or(0);
        ret.IsSet.EcnEnabled = ecn_enabled.has_value();

        ret.MaxOperationsPerDrain = max_operations_per_drain.value_or(0);
        ret.IsSet.MaxOperationsPerDrain = max_operations_per_drain.has_value();

        ret.MtuDiscoveryMissingProbeCount = mtu_discovery_missing_probe_count.value_or(0);
        ret.IsSet.MtuDiscoveryMissingProbeCount = mtu_discovery_missing_probe_count.has_value();

        ret.DestCidUpdateIdleTimeoutMs = dest_cid_update_idle_timeout_ms.value_or(0);
        ret.IsSet.DestCidUpdateIdleTimeoutMs = dest_cid_update_idle_timeout_ms.has_value();

        return ret;
    }
};

}  // namespace wolf::stream::quic
