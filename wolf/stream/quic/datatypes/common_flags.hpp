#pragma once

#include <msquic.h>

namespace wolf::stream::quic {

/**
 * @brief The equivalent enum flags for QUIC_CONNECTION_SHUTDOWN_FLAGS.
 */
enum class w_connection_shutdown_flag {
    None = QUIC_CONNECTION_SHUTDOWN_FLAG_NONE,
    Silent = QUIC_CONNECTION_SHUTDOWN_FLAG_SILENT
};

/**
 * @brief The equivalent enum flags for QUIC_STREAM_OPEN_FLAGS.
 */
enum class w_stream_open_flag {
    None = QUIC_STREAM_OPEN_FLAG_NONE,
    ZeroRTT = QUIC_STREAM_OPEN_FLAG_0_RTT,
    Unidirectional = QUIC_STREAM_OPEN_FLAG_UNIDIRECTIONAL
};

/**
 * @brief The equivalent enum flags for QUIC_STREAM_START_FLAGS.
 */
enum class w_stream_start_flag {
    None = QUIC_STREAM_START_FLAG_NONE,
    Immediate = QUIC_STREAM_START_FLAG_IMMEDIATE,
    FailBlocked = QUIC_STREAM_START_FLAG_FAIL_BLOCKED,
    ShutdownOnFail = QUIC_STREAM_START_FLAG_SHUTDOWN_ON_FAIL,
    IndicatePeerAccept = QUIC_STREAM_START_FLAG_INDICATE_PEER_ACCEPT,
};

/**
 * @brief The equivalent enum flags for QUIC_STREAM_SHUTDOWN_FLAGS.
 */
enum class w_stream_shutdown_flag {
    Graceful = QUIC_STREAM_SHUTDOWN_FLAG_GRACEFUL,
    AbortSend = QUIC_STREAM_SHUTDOWN_FLAG_ABORT_SEND,
    AbortReceive = QUIC_STREAM_SHUTDOWN_FLAG_ABORT_RECEIVE,
    Immediate = QUIC_STREAM_SHUTDOWN_FLAG_IMMEDIATE,

    Abort = QUIC_STREAM_SHUTDOWN_FLAG_ABORT,  // abort both (ABORT_SEND | ABORT_RECEIVE)
};

/**
 * @brief The equivalent enum flags for QUIC_RECEIVE_FLAGS.
 */
enum class w_receive_flag {
    None = QUIC_RECEIVE_FLAG_NONE,
    ZeroRTT = QUIC_RECEIVE_FLAG_0_RTT,
    Fin = QUIC_RECEIVE_FLAG_FIN
};

/**
 * @brief The equivalent enum flags for QUIC_SEND_FLAGS.
 */
enum class w_send_flag {
    None = QUIC_SEND_FLAG_NONE,
    AllowZeroRTT = QUIC_SEND_FLAG_ALLOW_0_RTT,
    Start = QUIC_SEND_FLAG_START,
    Fin = QUIC_SEND_FLAG_FIN,
    DataGramPriority = QUIC_SEND_FLAG_DGRAM_PRIORITY,
    DelaySend = QUIC_SEND_FLAG_DELAY_SEND,
};

}  // namespace wolf::stream::quic
