#pragma once

#include "system/w_flags.hpp"
#include "stream/quic/internal/common.hpp"
#include "stream/quic/datatypes/common_flags.hpp"
#include "stream/quic/datatypes/w_status.hpp"

#include <msquic.h>

#include <span>
#include <ranges>

namespace wolf::stream::quic {

/**
 * @brief Wrapper around START_COMPLETE variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_start_complete {
    friend class internal::w_raw_access;

public:
    w_stream_event_start_complete() = delete;
    w_stream_event_start_complete(const w_stream_event_start_complete&) = delete;
    w_stream_event_start_complete(w_stream_event_start_complete&&) = delete;

    [[nodiscard]] w_status status() const noexcept
    {
        return static_cast<w_status_code>(_event->START_COMPLETE.Status);
    }

    [[nodiscard]] auto id() const noexcept
    {
        return _event->START_COMPLETE.ID;
    }

    [[nodiscard]] bool peer_accepted() const noexcept
    {
        return _event->START_COMPLETE.PeerAccepted;
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_start_complete(internal::w_raw_tag, QUIC_STREAM_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_STREAM_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper around RECEIVE variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_receive {
    friend class internal::w_raw_access;

public:
    w_stream_event_receive() = delete;
    w_stream_event_receive(const w_stream_event_receive&) = delete;
    w_stream_event_receive(w_stream_event_receive&&) = delete;

    [[nodiscard]] auto total_buffer_length() const noexcept
    {
        return _event->RECEIVE.TotalBufferLength;
    }

    [[nodiscard]] auto buffers() const noexcept
    {
        return std::span(_event->RECEIVE.Buffers, _event->RECEIVE.BufferCount)
            | std::ranges::views::transform([](const QUIC_BUFFER& buf) {
                return std::span(buf.Buffer, buf.Length);
            });
    }

    [[nodiscard]] ::wolf::system::w_flags<w_receive_flag> flags() const noexcept
    {
        return static_cast<w_receive_flag>(_event->RECEIVE.Flags);
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_receive(internal::w_raw_tag, QUIC_STREAM_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_STREAM_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper around SEND_COMPLETE variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_send_complete {
    friend class internal::w_raw_access;

public:
    w_stream_event_send_complete() = delete;
    w_stream_event_send_complete(const w_stream_event_send_complete&) = delete;
    w_stream_event_send_complete(w_stream_event_send_complete&&) = delete;

    [[nodiscard]] bool canceled() const noexcept
    {
        return _event->SEND_COMPLETE.Canceled;
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_send_complete(internal::w_raw_tag, QUIC_STREAM_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_STREAM_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper around PEER_SEND_SHUTDOWN variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_peer_send_shutdown {
    friend class internal::w_raw_access;

public:
    w_stream_event_peer_send_shutdown() = delete;
    w_stream_event_peer_send_shutdown(const w_stream_event_peer_send_shutdown&) = delete;
    w_stream_event_peer_send_shutdown(w_stream_event_peer_send_shutdown&&) = delete;

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_peer_send_shutdown(internal::w_raw_tag, QUIC_STREAM_EVENT* /*p_event*/)
        // : _event(p_event) // unused
    {}

    // QUIC_STREAM_EVENT* _event = nullptr; // unused
};

/**
 * @brief Wrapper around PEER_SEND_ABORTED variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_peer_send_aborted {
    friend class internal::w_raw_access;

public:
    w_stream_event_peer_send_aborted() = delete;
    w_stream_event_peer_send_aborted(const w_stream_event_peer_send_aborted&) = delete;
    w_stream_event_peer_send_aborted(w_stream_event_peer_send_aborted&&) = delete;

    [[nodiscard]] auto error_code() const noexcept
    {
        return _event->PEER_SEND_ABORTED.ErrorCode;
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_peer_send_aborted(internal::w_raw_tag, QUIC_STREAM_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_STREAM_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper around PEER_RECEIVE_ABORTED variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_peer_receive_aborted {
    friend class internal::w_raw_access;

public:
    w_stream_event_peer_receive_aborted() = delete;
    w_stream_event_peer_receive_aborted(const w_stream_event_peer_receive_aborted&) = delete;
    w_stream_event_peer_receive_aborted(w_stream_event_peer_receive_aborted&&) = delete;

    [[nodiscard]] auto error_code() const noexcept
    {
        return _event->PEER_RECEIVE_ABORTED.ErrorCode;
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_peer_receive_aborted(internal::w_raw_tag, QUIC_STREAM_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_STREAM_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper around SEND_SHUTDOWN_COMPLETE variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_send_shutdown_complete {
    friend class internal::w_raw_access;

public:
    w_stream_event_send_shutdown_complete() = delete;
    w_stream_event_send_shutdown_complete(const w_stream_event_send_shutdown_complete&) = delete;
    w_stream_event_send_shutdown_complete(w_stream_event_send_shutdown_complete&&) = delete;

    [[nodiscard]] bool graceful() const noexcept
    {
        return _event->SEND_SHUTDOWN_COMPLETE.Graceful;
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_send_shutdown_complete(internal::w_raw_tag, QUIC_STREAM_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_STREAM_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper around SHUTDOWN_COMPLETE variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_shutdown_complete {
    friend class internal::w_raw_access;

public:
    w_stream_event_shutdown_complete() = delete;
    w_stream_event_shutdown_complete(const w_stream_event_shutdown_complete&) = delete;
    w_stream_event_shutdown_complete(w_stream_event_shutdown_complete&&) = delete;

    [[nodiscard]] bool connection_shutdown() const noexcept
    {
        return _event->SHUTDOWN_COMPLETE.ConnectionShutdown;
    }

    [[nodiscard]] bool app_close_in_progress() const noexcept
    {
        return _event->SHUTDOWN_COMPLETE.AppCloseInProgress;
    }

    [[nodiscard]] bool connection_shutdown_by_app() const noexcept
    {
        return _event->SHUTDOWN_COMPLETE.ConnectionShutdownByApp;
    }

    [[nodiscard]] bool connection_closed_remotely() const noexcept
    {
        return _event->SHUTDOWN_COMPLETE.ConnectionClosedRemotely;
    }

    [[nodiscard]] auto connection_error_code() const noexcept
    {
        return _event->SHUTDOWN_COMPLETE.ConnectionErrorCode;
    }

    [[nodiscard]] w_status connection_close_status() const noexcept
    {
        return static_cast<w_status_code>(_event->SHUTDOWN_COMPLETE.ConnectionCloseStatus);
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_shutdown_complete(internal::w_raw_tag, QUIC_STREAM_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_STREAM_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper around IDEAL_SEND_BUFFER_SIZE variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_ideal_send_buffer_size {
    friend class internal::w_raw_access;

public:
    w_stream_event_ideal_send_buffer_size() = delete;
    w_stream_event_ideal_send_buffer_size(const w_stream_event_ideal_send_buffer_size&) = delete;
    w_stream_event_ideal_send_buffer_size(w_stream_event_ideal_send_buffer_size&&) = delete;

    [[nodiscard]] std::size_t byte_count() const noexcept
    {
        return _event->IDEAL_SEND_BUFFER_SIZE.ByteCount;
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_ideal_send_buffer_size(internal::w_raw_tag, QUIC_STREAM_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_STREAM_EVENT* _event = nullptr;
};

/**
 * @brief Wrapper around PEER_ACCEPTED variation of QUIC_STREAM_EVENT.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event_peer_accepted {
    friend class internal::w_raw_access;

public:
    w_stream_event_peer_accepted() = delete;
    w_stream_event_peer_accepted(const w_stream_event_peer_accepted&) = delete;
    w_stream_event_peer_accepted(w_stream_event_peer_accepted&&) = delete;

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event_peer_accepted(internal::w_raw_tag, QUIC_STREAM_EVENT* /*p_event*/)
        // : _event(p_event) // unused
    {}

    // QUIC_STREAM_EVENT* _event = nullptr; // unused
};

/**
 * @brief Wrapper around QUIC_STREAM_EVENT variant.
 *
 * @note This class is non-owning and also is not move or copy-able,
 *       but only referencible.
 *       please refer to its provider for lifetime guarantees.
 */
class w_stream_event {
public:
    w_stream_event() = delete;
    w_stream_event(const w_stream_event&) = delete;
    w_stream_event(w_stream_event&&) = delete;

    /**
     * @brief visit the variant based on event type.
     * @param p_visitor  overload to handle different variations of the event instnace.
     */
    template <typename F>
    auto visit(F&& p_visitor)
    {
        switch (_event->Type) {
        case QUIC_STREAM_EVENT_START_COMPLETE:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_start_complete>(_event)
            );

        case QUIC_STREAM_EVENT_RECEIVE:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_receive>(_event)
            );

        case QUIC_STREAM_EVENT_SEND_COMPLETE:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_send_complete>(_event)
            );

        case QUIC_STREAM_EVENT_PEER_SEND_SHUTDOWN:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_peer_send_shutdown>(_event)
            );

        case QUIC_STREAM_EVENT_PEER_SEND_ABORTED:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_peer_send_shutdown>(_event)
            );

        case QUIC_STREAM_EVENT_PEER_RECEIVE_ABORTED:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_peer_receive_aborted>(_event)
            );

        case QUIC_STREAM_EVENT_SEND_SHUTDOWN_COMPLETE:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_send_shutdown_complete>(_event)
            );

        case QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_shutdown_complete>(_event)
            );

        case QUIC_STREAM_EVENT_IDEAL_SEND_BUFFER_SIZE:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_ideal_send_buffer_size>(_event)
            );

        case QUIC_STREAM_EVENT_PEER_ACCEPTED:
            return p_visitor(
                internal::w_raw_access::from_raw<w_stream_event_peer_accepted>(_event)
            );
            break;
        }
    }

private:
    /**
     * @brief wrap the given raw event as non-owning.
     * @param p_event  pointer to event.
     */
    w_stream_event(internal::w_raw_tag, QUIC_STREAM_EVENT* p_event)
        : _event(p_event)
    {}

    QUIC_STREAM_EVENT* _event = nullptr;
};

}  // namespace wolf::stream::quic
