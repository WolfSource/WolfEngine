#pragma once

#include <msquic.h>

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
 * @brief The equivalent enum flags for QUIC_RECEIVE_FLAGS.
 */
enum class w_receive_flag {
    None = QUIC_RECEIVE_FLAG_NONE,
    ZeroRTT = QUIC_RECEIVE_FLAG_0_RTT,
    Fin = QUIC_RECEIVE_FLAG_FIN
};
