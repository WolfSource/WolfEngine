#ifndef WOLF_STREAM_QUIC
#error "WOLF_STREAM_QUIC feature is not enabled."
#endif

#pragma once

#include "system/w_flags.hpp"

#include "stream/quic/datatypes/common_flags.hpp"
#include "stream/quic/datatypes/w_status.hpp"
#include "stream/quic/datatypes/w_settings.hpp"
#include "stream/quic/datatypes/w_address.hpp"
#include "stream/quic/datatypes/w_credential_config.hpp"
#include "stream/quic/datatypes/w_registration_config.hpp"
#include "stream/quic/datatypes/w_new_connection_info.hpp"
#include "stream/quic/datatypes/w_alpn.hpp"

#include "stream/quic/events/w_connection_event.hpp"
#include "stream/quic/events/w_listener_event.hpp"
#include "stream/quic/events/w_stream_event.hpp"

#include "stream/quic/handles/w_configuration.hpp"
#include "stream/quic/handles/w_connection.hpp"
#include "stream/quic/handles/w_listener.hpp"
#include "stream/quic/handles/w_registration.hpp"
#include "stream/quic/handles/w_stream.hpp"

#include "stream/quic/w_quic_server.hpp"
