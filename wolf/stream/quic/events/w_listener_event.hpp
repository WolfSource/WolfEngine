#pragma once

#include "stream/quic/internal/common.hpp"
#include "stream/quic/internal/w_msquic_api.hpp"
#include "stream/quic/datatypes/w_new_connection_info.hpp"
#include "stream/quic/handles/w_connection.hpp"

#include <msquic.h>

// NOTE windows.h must be included after msquic.h
//      due to some declaration issues.
#include "wolf.hpp"

namespace wolf::stream::quic {

/**
 * @brief Wrapper of NEW_CONNECTION variation of QUIC_LISTENER_EVENT.
 *
 * There are access/proxy methods to access each field.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class W_API w_listener_event_new_connection {
    friend class internal::w_raw_access;

public:
    w_listener_event_new_connection() = delete;
    w_listener_event_new_connection(const w_listener_event_new_connection&) = delete;
    w_listener_event_new_connection(w_listener_event_new_connection&&) = delete;

    ~w_listener_event_new_connection()
    {
        // make sure to close connection if still afloating
        // to avoid leak/deadlock/crash.
        if (_event->NEW_CONNECTION.Connection) {
            reject_connection();
        }
    }

    /**
     * @brief get the new connection's info.
     */
    [[nodiscard]] w_new_connection_info connection_info() const noexcept
    {
        return internal::w_raw_access::from_raw<w_new_connection_info>(*_event->NEW_CONNECTION.Info);
    }

    /**
     * @brief reject and close the incoming connection handle.
     */
    void reject_connection() noexcept
    {
        if (!_event->NEW_CONNECTION.Connection) {
            return;
        }

        internal::w_msquic_api::api()->ConnectionClose(_event->NEW_CONNECTION.Connection);
        _event->NEW_CONNECTION.Connection = nullptr;  // to avoid Use-After-Free.
    }

    /**
     * @brief accept and setup the new connection handle.
     *
     * @param p_config    configuration to set.
     * @param p_callback  event handler callback.
     * @return success or error.
     */
    auto accept_connection(w_configuration& p_config, w_connection::callback_type p_callback) noexcept
        -> boost::leaf::result<w_connection>
    {
        if (!_event->NEW_CONNECTION.Connection) {
            return W_FAILURE(std::errc::operation_canceled,
                             "connection is already accepted or rejected.");
        }

        BOOST_LEAF_AUTO(conn, w_connection::setup_new_raw_connection(
            _event->NEW_CONNECTION.Connection,
            p_config,
            std::move(p_callback)
        ));

        _event->NEW_CONNECTION.Connection = nullptr;

        return conn;
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_listener_event_new_connection(internal::w_raw_tag, QUIC_LISTENER_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_LISTENER_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper of STOP_COMPLETE variation of QUIC_LISTENER_EVENT.
 *
 * There are access/proxy methods to access each field.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class W_API w_listener_event_stop_complete {
    friend class internal::w_raw_access;

public:
    w_listener_event_stop_complete() = delete;
    w_listener_event_stop_complete(const w_listener_event_stop_complete&) = delete;
    w_listener_event_stop_complete(w_listener_event_stop_complete&&) = delete;

    /**
     * @brief whether the app close is in progress.
     */
    [[nodiscard]] bool app_close_in_progress() const noexcept
    {
        return _event->STOP_COMPLETE.AppCloseInProgress;
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_listener_event_stop_complete(internal::w_raw_tag, QUIC_LISTENER_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_LISTENER_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper around QUIC_LISTENER_EVENT variant.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class W_API w_listener_event {
    friend class internal::w_raw_access;

public:
    w_listener_event() = delete;
    w_listener_event(const w_listener_event&) = delete;
    w_listener_event(w_listener_event&&) = delete;

    /**
     * @brief get string name of the underlying event.
     */
    [[nodiscard]] auto name() const noexcept -> std::string_view
    {
        switch (_event->Type) {
        case QUIC_LISTENER_EVENT_NEW_CONNECTION:
            return "new-connection";

        case QUIC_LISTENER_EVENT_STOP_COMPLETE:
            return "stop-complete";
        }

        return "unknown";
    }

    /**
     * @brief visit the variant based on event type.
     * @param p_visitor  overload to handle different variations of the event instnace.
     */
    template <typename F>
    auto visit(F&& p_visitor)
    {
        switch (_event->Type) {
        case QUIC_LISTENER_EVENT_NEW_CONNECTION:
            return p_visitor(
                internal::w_raw_access::from_raw<w_listener_event_new_connection>(_event)
            );
        case QUIC_LISTENER_EVENT_STOP_COMPLETE:
            return p_visitor(
                internal::w_raw_access::from_raw<w_listener_event_stop_complete>(_event)
            );
        }

        return p_visitor(std::monostate{});
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_listener_event(internal::w_raw_tag, QUIC_LISTENER_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_LISTENER_EVENT* _event = nullptr;
};

}  // namespace wolf::stream::quic
