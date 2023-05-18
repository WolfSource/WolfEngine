#pragma once

#include "stream/quic/internal/common.hpp"
#include "stream/quic/datatypes/w_status.hpp"
#include "stream/quic/handles/w_connection.hpp"

#include <boost/leaf.hpp>
#include <msquic.h>

#include <atomic>
#include <span>
#include <tuple>

namespace wolf::stream::quic {

class w_stream_event;
class w_connection_event_peer_stream_started;

/**
 * @brief The wrapper of stream handle of msquic.
 */
class W_API w_stream {
    friend class internal::w_raw_access;
    friend class w_connection_event_peer_stream_started;

public:
    using callback_type = std::function<w_status(w_stream&, w_stream_event&)>;

private:
    using raw_callback_type = QUIC_STATUS(HQUIC, void*, QUIC_STREAM_EVENT*);

    struct context_bundle {
        callback_type callback;
        std::atomic<bool> running = false;
        std::atomic<int> refcount = 0;
        bool closed = false; // to avoid double free in reentrancy of close() in callback.
    };

    static QUIC_STATUS internal_raw_callback(HQUIC listener_raw,
                                             void* context_raw,
                                             QUIC_STREAM_EVENT* event_raw);

public:
    /**
     * @brief it's not purely constructible.
     * @note use static factory function `open`.
     */
    w_stream() = delete;

    w_stream(const w_stream&) = delete;
    w_stream(w_stream&& p_other) noexcept
        : _handle(std::exchange(p_other._handle, nullptr))
    {}

    w_stream& operator=(const w_stream&) = delete;
    w_stream& operator=(w_stream&& p_other) noexcept
    {
        std::swap(_handle, p_other._handle);
        return *this;
    }

    ~w_stream() { close(); }

    /**
     * @brief open/create a stream on a connection.
     * @param p_conn      connection to open the stream on.
     * @param p_callback  stream event handler callback.
     * @return a connection instance on success.
     */
    [[nodiscard]] static auto open(w_connection& p_conn,
                                   callback_type p_callback,
                                   wolf::w_flags<w_stream_open_flag> p_flags = w_stream_open_flag::None) noexcept
        -> boost::leaf::result<w_stream>;

    /**
     * @brief set callback.
     * @param p_callback  event handler callback.
     * @return success or failure.
     */
    auto set_callback(callback_type p_callback) -> boost::leaf::result<void>;

    /**
     * @brief start the stream.
     * @param p_flags  flags to control the behavior of starting stream.
     * @return status of success or failure.
     */
    w_status start(wolf::w_flags<w_stream_start_flag> p_flags = w_stream_start_flag::None);

    /**
     * @brief send one or more buffers into stream.
     * @param p_str    string to send.
     * @param p_flags  flags to control the behavior of send.
     * @return a status inidicating success or failure.
     */
    w_status send(std::string_view p_str,
                  wolf::w_flags<w_send_flag> p_flags = w_send_flag::None);

    /**
     * @brief send one or more buffers into stream.
     * @param p_buffer  contiguous buffer to send.
     * @param p_flags   flags to control the behavior of send.
     * @return a status inidicating success or failure.
     */
    w_status send(std::span<std::uint8_t> p_buffer,
                  wolf::w_flags<w_send_flag> p_flags = w_send_flag::None);

    /**
     * @brief shutdown the stream.
     *
     * This is a non-blocking call, completion will be notified
     * by shutdown_complete event in callback.
     */
    void shutdown(wolf::w_flags<w_stream_shutdown_flag> p_flags = w_stream_shutdown_flag::Graceful,
                  std::size_t p_error_code = 0);
private:
    /**
     * @brief close the connection handle.
     */
    void close();

    /**
     * @brief setup the new raw stream created by msquic.
     * @param p_conn_raw  raw stream handle.
     * @param p_config    configuration to associate with.
     * @param p_callback  event handler callback.
     * @return status of whether succeeded or not.
     */
    [[nodiscard]] static auto setup_new_raw_stream(HQUIC p_stream_raw, callback_type p_callback) noexcept
        -> boost::leaf::result<w_stream>;

    template <typename HandlerF>
    [[nodiscard]] static auto make_context_callback_ptrs(HandlerF&& p_callback)
        -> std::pair<context_bundle*, QUIC_STREAM_CALLBACK_HANDLER>
    {
        return {
            new context_bundle{ .callback = std::forward<HandlerF>(p_callback) },
            internal_raw_callback
        };
    }

    auto raw() noexcept { return _handle; }
    auto raw() const noexcept { return _handle; }

    explicit w_stream(internal::w_raw_tag, HQUIC p_handle) noexcept;

    HQUIC _handle = nullptr;
};

}  // namespace wolf::stream::quic
