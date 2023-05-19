#include "stream/quic/handles/w_stream.hpp"

#include "stream/quic/internal/w_msquic_api.hpp"
#include "stream/quic/events/w_stream_event.hpp"

#include "wolf.hpp"

namespace wolf::stream::quic {

QUIC_STATUS w_stream::internal_raw_callback(HQUIC stream_raw,
                                            void* context_raw,
                                            QUIC_STREAM_EVENT* event_raw)
{
    if (!context_raw) {
        return QUIC_STATUS_INTERNAL_ERROR;
    }

    auto context = static_cast<context_bundle*>(context_raw);

    auto stream = w_stream(internal::w_raw_tag{}, stream_raw);
    auto event = internal::w_raw_access::from_raw<w_stream_event>(event_raw);

    w_status status = context->callback(stream, event);

    // if stream is not closed.
    if (stream.raw()) {
        if (event_raw->Type == QUIC_STREAM_EVENT_SEND_COMPLETE) {
            delete static_cast<QUIC_BUFFER*>(event_raw->SEND_COMPLETE.ClientContext);
        }
        if (event_raw->Type == QUIC_STREAM_EVENT_SHUTDOWN_COMPLETE) {
            context->running = false;
            // not running anymore, thus no longer requires to be held for callback.
            --context->refcount;
        }
    }

    return status;
}

bool w_stream::is_running() const noexcept
{
    if (!_handle) {
        return false;
    }

    auto api = internal::w_msquic_api::api();
    auto context = static_cast<context_bundle*>(api->GetContext(_handle));
    return context->running;
}

auto w_stream::open(w_connection& p_conn,
                    callback_type p_callback,
                    wolf::w_flags<w_stream_open_flag> p_flags) noexcept
    -> boost::leaf::result<w_stream>
{
    HQUIC handle = nullptr;

    auto api = internal::w_msquic_api::api();

    auto conn_raw = internal::w_raw_access::raw(p_conn);
    if (!conn_raw) {
        return W_FAILURE(std::errc::invalid_argument, "given connection is not valid.");
    }

    auto [context_ptr, callback_ptr] = make_context_callback_ptrs(std::move(p_callback));
    if (!context_ptr || !callback_ptr) {
        return W_FAILURE(std::errc::not_enough_memory, "couldn't allocate for callback context.");
    }

    w_status status = api->StreamOpen(
        conn_raw,
        static_cast<QUIC_STREAM_OPEN_FLAGS>(p_flags.to_underlying()),
        (QUIC_STREAM_CALLBACK_HANDLER)callback_ptr,
        (void*)context_ptr,
        &handle
    );
    if (status.failed()) {
        delete context_ptr;
        return W_FAILURE(std::errc::operation_canceled,
                         wolf::format("couldn't open/create stream: {}", status_to_str(status)));
    }

    return w_stream(internal::w_raw_tag{}, handle);
}

auto w_stream::set_callback(callback_type p_callback) -> boost::leaf::result<void>
{
    if (!_handle) {
        return W_FAILURE(std::errc::operation_canceled, "stream is closed/destroyed.");
    }

    auto api = internal::w_msquic_api::api();

    auto context = static_cast<context_bundle*>(api->GetContext(_handle));

    if (!context) {
        return W_FAILURE(std::errc::operation_canceled, "stream is in invalid state.");
    }

    if (context->running) {
        return W_FAILURE(std::errc::operation_canceled,
                         "can't set callback when stream is active/running.");
    }

    context->callback = std::move(p_callback);

    return {};
}

w_status w_stream::start(wolf::w_flags<w_stream_start_flag> p_flags)
{
    if (!_handle) {
        return w_status_code::InvalidState;
    }

    auto api = internal::w_msquic_api::api();

    auto context = static_cast<context_bundle*>(api->GetContext(_handle));
    if (!context) {
        return w_status_code::InternalError;
    }

    if (context->running) {
        return w_status_code::InvalidState;
    }

    w_status status = api->StreamStart(
        _handle,
        static_cast<QUIC_STREAM_START_FLAGS>(p_flags.to_underlying())
    );
    if (status.failed()) {
        return status;
    }

    context->running = true;
    // account one more for running stream to
    // be passed into the callback.
    ++context->refcount;

    return status;
}

w_status w_stream::send(std::string_view p_str, wolf::w_flags<w_send_flag> p_flags)
{
    return send(std::span<uint8_t>((uint8_t*)p_str.data(), p_str.size()));
}

w_status w_stream::send(std::span<uint8_t> p_buffer, wolf::w_flags<w_send_flag> p_flags)
{
    if (!_handle) {
        return w_status_code::InvalidState;
    }

    auto api = internal::w_msquic_api::api();

    // on successful send, the allocated buffer must be
    // dealloacted in event handler callback by context pointer.
    auto buf_ptr = new QUIC_BUFFER{
        .Length = (unsigned int)p_buffer.size(),
        .Buffer = p_buffer.data()
    };

    const auto flags = static_cast<QUIC_SEND_FLAGS>(p_flags.to_underlying());

    w_status status = api->StreamSend(_handle, buf_ptr, 1, flags, buf_ptr);
    if (status.failed()) {
        delete buf_ptr;
        return status;
    }

    return status;
}

void w_stream::shutdown(wolf::w_flags<w_stream_shutdown_flag> p_flags, size_t p_error_code)
{
    if (!_handle) {
        return;
    }

    auto api = internal::w_msquic_api::api();

    auto context = static_cast<context_bundle*>(api->GetContext(_handle));
    if (!context || !context->running) {
        return;
    }

    const auto flags = static_cast<QUIC_STREAM_SHUTDOWN_FLAGS>(p_flags.to_underlying());

    api->StreamShutdown(_handle, flags, p_error_code);
}

void w_stream::close()
{
    if (!_handle) {
        return;
    }

    auto api = internal::w_msquic_api::api();

    auto context = static_cast<context_bundle*>(api->GetContext(_handle));
    if (!context || context->closed) {
        return;
    }

    if (--context->refcount > 0) {
        return;
    }

    // making sure if callback calls close,
    // it wouldn't result in reentrancy in msquic api.
    context->closed = true;

    api->StreamClose(_handle);
    _handle = nullptr;
    delete context;
}

auto w_stream::setup_new_raw_stream(HQUIC p_stream_raw, callback_type p_callback) noexcept
    -> boost::leaf::result<w_stream>
{
    auto api = internal::w_msquic_api::api();

    auto [context_ptr, callback_ptr] = make_context_callback_ptrs(std::move(p_callback));
    if (!context_ptr) {
        return W_FAILURE(std::errc::operation_canceled,
                         "couldn't allocate for context of stream callback.");
    }

    api->SetCallbackHandler(
        p_stream_raw,
        (QUIC_STREAM_CALLBACK_HANDLER)callback_ptr,
        context_ptr
    );

    // since it's already running, sets it as running
    // and also adds a refcount for callback.
    context_ptr->running = true;
    ++context_ptr->refcount;

    return w_stream(internal::w_raw_tag{}, p_stream_raw);
}

w_stream::w_stream(internal::w_raw_tag, HQUIC p_handle) noexcept
    : _handle(p_handle)
{
    auto api = internal::w_msquic_api::api();
    auto context = static_cast<context_bundle*>(api->GetContext(p_handle));
    ++context->refcount;
}

}  // namespace wolf::stream::quic
