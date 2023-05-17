#include "stream/quic/handles/w_connection.hpp"
#include "stream/quic/events/w_connection_event.hpp"
#include "stream/quic/internal/w_msquic_api.hpp"

// NOTE: this header must be included after msquic.h
//       or there would be a redefinition error.
#include "wolf.hpp"

namespace wolf::stream::quic {

QUIC_STATUS w_connection::internal_raw_callback(HQUIC p_connection_raw,
                                                void* p_context_raw,
                                                QUIC_CONNECTION_EVENT* p_event_raw)
{
    if (!p_context_raw) {
        return QUIC_STATUS_INTERNAL_ERROR;
    }

    auto context = static_cast<context_bundle*>(p_context_raw);

    auto connection = w_connection(internal::w_raw_tag{}, p_connection_raw);
    auto event = internal::w_raw_access::from_raw<w_connection_event>(p_event_raw);

    w_status status = context->callback(connection, event);

    if (p_event_raw->Type == QUIC_CONNECTION_EVENT_SHUTDOWN_COMPLETE) {
        context->running = false;
        // not running anymore, thus no longer requires to be held for callback.
        --context->refcount;
    }

    return status;
}

auto w_connection::open(w_registration& p_reg, callback_type p_callback) noexcept
    -> boost::leaf::result<w_connection>
{
    HQUIC handle = nullptr;

    auto api = internal::w_msquic_api::api();

    auto reg_raw = internal::w_raw_access::raw(p_reg);
    if (!reg_raw) {
        return W_FAILURE(std::errc::invalid_argument,
                         "given registration is not valid.");
    }

    auto [context_ptr, callback_ptr] = make_context_callback_ptrs(std::move(p_callback));
    if (!context_ptr || !callback_ptr) {
        return W_FAILURE(std::errc::not_enough_memory,
                         "couldn't allocate for callback context.");
    }

    w_status status = api->ConnectionOpen(
        reg_raw,
        (QUIC_CONNECTION_CALLBACK_HANDLER)callback_ptr,
        (void*)context_ptr,
        &handle
    );
    if (status.failed()) {
        delete context_ptr;
        return W_FAILURE(std::errc::operation_canceled,
                         wolf::format("couldn't open/create connection: {}", status_to_str(status)));
    }

    return w_connection(internal::w_raw_tag{}, handle);
}

auto w_connection::set_callback(callback_type p_callback) -> boost::leaf::result<void>
{
    if (!_handle) {
        return W_FAILURE(std::errc::operation_canceled,
                         "connection is closed/destroyed.");
    }

    auto api = internal::w_msquic_api::api();

    auto context = static_cast<context_bundle*>(api->GetContext(_handle));

    if (!context) {
        return W_FAILURE(std::errc::operation_canceled,
                         "connection is in invalid state.");
    }

    if (context->running) {
        return W_FAILURE(std::errc::operation_canceled,
                         "can't set callback when connection is active/running.");
    }

    context->callback = std::move(p_callback);

    return {};
}

w_status w_connection::start(w_configuration& p_config, const char* p_host, std::uint16_t p_port)
{
    if (!_handle) {
        return w_status_code::InvalidState;
    }

    auto api = internal::w_msquic_api::api();

    context_bundle* context = static_cast<context_bundle*>(api->GetContext(_handle));
    if (!context || context->closing || context->running) {
        return w_status_code::InvalidState;
    }

    auto config_raw = internal::w_raw_access::raw(p_config);

    w_status status = api->ConnectionStart(
        _handle,
        config_raw,
        QUIC_ADDRESS_FAMILY_UNSPEC,
        p_host,
        p_port
    );

    if (status.failed()) {
        return status;
    }

    context->running = true;
    // account one more for running connections to
    // be passed into the callback.
    ++context->refcount;

    return status;
}

void w_connection::shutdown(wolf::w_flags<w_connection_shutdown_flag> p_flags,
                            std::size_t p_error_code)
{
    if (!_handle) {
        return;
    }

    auto api = internal::w_msquic_api::api();

    auto context = static_cast<context_bundle*>(api->GetContext(_handle));
    if (!context || !context->running) {
        return;
    }

    const auto flags = static_cast<QUIC_CONNECTION_SHUTDOWN_FLAGS>(p_flags.to_underlying());

    api->ConnectionShutdown(_handle, flags, p_error_code);
}

void w_connection::close()
{
    if (!_handle) {
        return;
    }

    auto api = internal::w_msquic_api::api();

    auto context = static_cast<context_bundle*>(api->GetContext(_handle));
    if (!context || context->closing) {
        return;
    }

    if (--context->refcount > 0) {
        return;
    }

    // makes sure if callback calls close,
    // it wouldn't result in reentrancy in msquic api.
    context->closing = true;

    api->ConnectionClose(_handle);
    _handle = nullptr;
    delete context;
}

auto w_connection::setup_new_raw_connection(HQUIC p_conn_raw,
                                            w_configuration& p_config,
                                            callback_type p_callback) noexcept
    -> boost::leaf::result<w_connection>
{
    auto api = internal::w_msquic_api::api();
    auto config_raw = internal::w_raw_access::raw(p_config);
    if (!config_raw) {
        return W_FAILURE(std::errc::invalid_argument, "given config is closed or invalid.");
    }

    w_status status = api->ConnectionSetConfiguration(p_conn_raw, config_raw);
    if (status.failed()) {
        auto err_str = wolf::format("couldn't set the configuration on the connection: {}",
                                    status_to_str(status));
        return W_FAILURE(std::errc::operation_canceled, err_str);
    }

    auto [context_ptr, callback_ptr] = make_context_callback_ptrs(std::move(p_callback));
    if (!context_ptr) {
        return W_FAILURE(std::errc::not_enough_memory,
                         "couldn't allocate for connection callback context.");
    }

    api->SetCallbackHandler(
        p_conn_raw,
        (QUIC_CONNECTION_CALLBACK_HANDLER)callback_ptr,
        context_ptr
    );

    context_ptr->running = true;
    ++context_ptr->refcount;

    return w_connection(internal::w_raw_tag{}, p_conn_raw);
}

w_connection::w_connection(internal::w_raw_tag, HQUIC p_handle) noexcept
    : _handle(p_handle)
{
    auto api = internal::w_msquic_api::api();
    auto context = static_cast<context_bundle*>(api->GetContext(p_handle));
    ++context->refcount;
}

}  // namespace wolf::stream::quic
