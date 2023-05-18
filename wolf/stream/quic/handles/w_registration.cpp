#include "stream/quic/handles/w_registration.hpp"

#include "stream/quic/internal/w_msquic_api.hpp"
#include "stream/quic/datatypes/w_status.hpp"

#include "wolf.hpp"

namespace wolf::stream::quic {

auto w_registration::open() noexcept -> boost::leaf::result<w_registration>
{
    HQUIC handle;
    auto api = internal::w_msquic_api::api();

    w_status status = api->RegistrationOpen(nullptr, &handle);
    if (status.failed()) {
        return W_FAILURE(std::errc::operation_canceled,
                         wolf::format("couldn't open/create registration: {}", status_to_str(status)));
    }

    return w_registration(internal::w_raw_tag{}, handle);
}

auto w_registration::open(const w_registration_config& p_config) noexcept
    -> boost::leaf::result<w_registration>
{
    HQUIC handle;
    auto api = internal::w_msquic_api::api();

    auto config_raw = internal::w_raw_access::raw(p_config);

    w_status status = api->RegistrationOpen(config_raw, &handle);
    if (status.failed()) {
        return W_FAILURE(std::errc::operation_canceled,
                         wolf::format("couldn't open/create registration: {}", status_to_str(status)));
    }

    return w_registration(internal::w_raw_tag{}, handle);
}

void w_registration::shutdown(wolf::w_flags<w_connection_shutdown_flag> p_flags,
                              std::size_t p_error_code)
{
    if (!_handle) {
        return;
    }

    auto api = internal::w_msquic_api::api();
    auto flags = static_cast<QUIC_CONNECTION_SHUTDOWN_FLAGS>(p_flags.to_underlying());
    api->RegistrationShutdown(_handle, flags, p_error_code);
}

void w_registration::close()
{
    if (!_handle) {
        return;
    }

    auto api = internal::w_msquic_api::api();
    api->RegistrationClose(_handle);
    _handle = nullptr;
}

}  // namespace wolf::stream::quic
