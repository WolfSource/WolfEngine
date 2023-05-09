#pragma once

#include "stream/quic/internal/common.hpp"
#include "stream/quic/datatypes/w_new_connection_info.hpp"

#include <msquic.h>

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
class w_listener_event_new_connection {
    friend class internal::w_raw_access;

public:
    w_listener_event_new_connection() = delete;
    w_listener_event_new_connection(const w_listener_event_new_connection&) = delete;
    w_listener_event_new_connection(w_listener_event_new_connection&&) = delete;

    /**
     * @brief new connection's info.
     */
    [[nodiscard]] auto connection_info() const noexcept
    {
        return internal::w_raw_access::from_raw<w_new_connection_info>(*_event->NEW_CONNECTION.Info);
    }

    /**
     * @brief new connection handle.
     */
    [[nodiscard]] auto connection() const noexcept
    {
        // TODO connection handle is not implemented yet.
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
class w_listener_event_stop_complete {
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
class w_listener_event {
    friend class internal::w_raw_access;

public:
    w_listener_event() = delete;
    w_listener_event(const w_listener_event&) = delete;
    w_listener_event(w_listener_event&&) = delete;

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
