#pragma once

#include "stream/quic/internal/common.hpp"
#include "stream/quic/datatypes/w_status.hpp"
#include "stream/quic/handles/w_configuration.hpp"
#include "stream/quic/handles/w_registration.hpp"

#include <boost/leaf.hpp>
#include <msquic.h>

#include <atomic>
#include <span>
#include <tuple>
#include <mutex>

namespace wolf::stream::quic {

class w_connection_event;
class w_listener_event_new_connection;

/**
 * @brief The wrapper of connection handle of msquic.
 */
class W_API w_connection {
    friend class internal::w_raw_access;
    friend class w_listener_event_new_connection;

public:
    using callback_type = std::function<w_status(w_connection&, w_connection_event&)>;

private:
    using raw_callback_type = QUIC_STATUS(HQUIC, void*, QUIC_CONNECTION_EVENT*);

    struct context_bundle {
        callback_type callback;
        std::atomic<bool> running = false;
        std::atomic<int> refcount = 0;
        bool closing = false; // to avoid double free in reentrancy of close() in callback.
    };

    static QUIC_STATUS internal_raw_callback(HQUIC p_connection_raw,
                                             void* p_context_raw,
                                             QUIC_CONNECTION_EVENT* p_event_raw);

public:
    /**
     * @brief it's not purely constructible.
     * @note use static factory function `open`.
     */
    w_connection() = delete;

    w_connection(const w_connection&) = delete;
    w_connection(w_connection&& p_other) noexcept
        : _handle(std::exchange(p_other._handle, nullptr))
    {}

    w_connection& operator=(const w_connection&) = delete;
    w_connection& operator=(w_connection&& p_other) noexcept
    {
        std::swap(_handle, p_other._handle);
        return *this;
    }

    ~w_connection() { close(); }

    /**
     * @brief open/create a connection.
     * @param p_reg       registration (execution context).
     * @param p_callback  connection callback.
     * @return a connection instance on success.
     */
    [[nodiscard]] static auto open(w_registration& p_reg, callback_type p_callback) noexcept
        -> boost::leaf::result<w_connection>;

    /**
     * @brief set callback.
     * @param p_callback  connection callback.
     * @return success or failure.
     */
    auto set_callback(callback_type p_callback) -> boost::leaf::result<void>;

    /**
     * @brief start the connection to connect.
     * @param p_config  the configuration to use for connection.
     * @param p_host    the host to connect to.
     * @param p_alpn    the port to connect to.
     * @return status of success or failure.
     */
    w_status start(w_configuration& p_config, const char* p_host, std::uint16_t p_port);

    /**
     * @brief shutdown the connection.
     *
     * This is a non-blocking call, completion will be notified
     * by shutdown_complete event in callback.
     */
    void shutdown(wolf::w_flags<w_connection_shutdown_flag> p_flags = w_connection_shutdown_flag::None,
                  std::size_t p_error_code = 0);

private:
    /**
     * @brief close the connection handle.
     */
    void close();

    /**
     * @brief setup the new raw connection created by msquic.
     * @param p_conn_raw  raw connection handle.
     * @param p_config    configuration to associate with.
     * @param p_callback  event handler callback.
     * @return status of whether succeeded or not.
     */
    [[nodiscard]] static auto setup_new_raw_connection(HQUIC p_conn_raw,
                                                       w_configuration& p_config,
                                                       callback_type p_callback) noexcept
        -> boost::leaf::result<w_connection>;

    template <typename HandlerF>
    [[nodiscard]] static auto make_context_callback_ptrs(HandlerF&& p_callback)
        -> std::pair<context_bundle*, QUIC_CONNECTION_CALLBACK_HANDLER>
    {
        return {
            new context_bundle{ .callback = std::forward<HandlerF>(p_callback) },
            internal_raw_callback
        };
    }

    auto raw() noexcept { return _handle; }
    auto raw() const noexcept { return _handle; }

    explicit w_connection(internal::w_raw_tag, HQUIC p_handle) noexcept;

    HQUIC _handle = nullptr;
};

}  // namespace wolf::stream::quic
