#pragma once

#include <msquic.h>

/**
 * @brief The equivalent enum flags for QUIC_CONNECTION_SHUTDOWN_FLAGS.
 */
enum class w_connection_shutdown_flag {
    None = QUIC_CONNECTION_SHUTDOWN_FLAG_NONE,
    Silent = QUIC_CONNECTION_SHUTDOWN_FLAG_SILENT
};
