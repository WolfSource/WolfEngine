#pragma once

#include "stream/quic/internal/common.hpp"
#include "stream/quic/datatypes/w_status.hpp"
#include "stream/quic/datatypes/w_alpn.hpp"
#include "stream/quic/events/w_listener_event.hpp"
#include "stream/quic/handles/w_registration.hpp"

#include <boost/leaf.hpp>
#include <msquic.h>

#include <atomic>
#include <span>
#include <tuple>

namespace wolf::stream::quic {

/**
 * @brief The wrapper of listener handle of msquic.
 */
class W_API w_listener {
    friend internal::w_raw_access;

public:
    using callback_type = std::function<w_status(w_listener&, w_listener_event&)>;

private:
    using raw_callback_type = QUIC_STATUS(HQUIC, void*, QUIC_LISTENER_EVENT*);

    struct context_bundle {
        callback_type callback;
        std::atomic<bool> running = false;
        std::atomic<int> refcount = 0;
        bool closed = false; // to avoid double free in reentrancy of close() in callback.
    };

    static QUIC_STATUS internal_raw_callback(HQUIC p_listener_raw,
                                             void* p_context_raw,
                                             QUIC_LISTENER_EVENT* p_event_raw);

public:
    /**
     * @brief it's not purely constructible.
     * @note use static factory function `open`.
     */
    w_listener() = delete;

    w_listener(const w_listener&) = delete;
    w_listener(w_listener&& p_other) noexcept
        : _handle(std::exchange(p_other._handle, nullptr))
    {}

    w_listener& operator=(const w_listener&) = delete;
    w_listener& operator=(w_listener&& p_other) noexcept
    {
        std::swap(_handle, p_other._handle);
        return *this;
    }

    ~w_listener() { close(); }

    /**
     * @brief open/create a listener.
     * @param p_reg       registration (execution context).
     * @param p_callback  listener callback.
     * @return a listener instance on success.
     */
    [[nodiscard]] static auto open(w_registration& p_reg, callback_type p_callback) noexcept
        -> boost::leaf::result<w_listener>;

    /**
     * @brief set callback.
     * @param p_callback  listener callback.
     * @return success or failure.
     */
    auto set_callback(callback_type p_callback) -> boost::leaf::result<void>;

    /**
     * @brief start to listen.
     * @param p_address  address and port to listen on.
     * @param p_alpn     application-layer protocol negotiation.
     * @return
     */
    w_status start(const w_address& p_address, w_alpn_view p_alpn);

    /**
     * @brief stop the listener.
     *
     * This is a non-blocking call, completion will be notified
     * by stop_complete event in callback.
     */
    void stop();

private:
    /**
     * @brief close the listener handle.
     */
    void close();

    auto raw() noexcept { return _handle; }
    auto raw() const noexcept { return _handle; }

    explicit w_listener(internal::w_raw_tag, HQUIC p_handle) noexcept;

    HQUIC _handle = nullptr;
};

}  // namespace wolf::stream::quic
