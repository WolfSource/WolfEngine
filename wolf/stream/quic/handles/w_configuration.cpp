#include "stream/quic/handles/w_configuration.hpp"

#include "stream/quic/internal/w_msquic_api.hpp"

#include "wolf.hpp"

namespace wolf::stream::quic {

auto w_configuration::open(w_registration& p_reg,
                           const w_settings& p_settings,
                           w_alpn_view p_alpn) noexcept
    -> boost::leaf::result<w_configuration>
{
    HQUIC handle = nullptr;
    auto api = internal::w_msquic_api::api();

    auto reg_raw = internal::w_raw_access::raw(p_reg);
    if (!reg_raw) {
        return W_FAILURE(std::errc::invalid_argument, "given registration is invalid.");
    }

    auto settings = internal::w_raw_access::raw(p_settings);

    auto alpn = QUIC_BUFFER{
        .Length = static_cast<unsigned int>(p_alpn.size()),
        .Buffer = p_alpn.data()
    };

    w_status status = api->ConfigurationOpen(reg_raw,
                                             &alpn, 1,
                                             &settings, sizeof(settings),
                                             nullptr,
                                             &handle);
    if (status.failed()) {
        return W_FAILURE(
            std::errc::operation_canceled,
            wolf::format("couldn't open configuration: {}", status_to_str(status)));
    }

    return w_configuration(internal::w_raw_tag{}, handle);
}

auto w_configuration::open(w_registration& p_reg,
                           const w_settings& p_settings,
                           w_alpn_view p_alpn,
                           const w_credential_config& p_cred_conf) noexcept
    -> boost::leaf::result<w_configuration>
{
    BOOST_LEAF_AUTO(ret, open(p_reg, p_settings, p_alpn));
    auto status = ret.load_credential(p_cred_conf);
    if (status.failed()) {
        return W_FAILURE(std::errc::operation_canceled,
                         wolf::format("couldn't load config credentials: {}", status_to_str(status)));
    }
    return ret;
}

w_status wolf::stream::quic::w_configuration::load_credential(const w_credential_config& p_creds_config)
{
    auto api = internal::w_msquic_api::api();
    auto creds_config_raw = internal::w_raw_access::raw(p_creds_config);
    return api->ConfigurationLoadCredential(_handle, creds_config_raw);
}

void w_configuration::close()
{
    if (!_handle) {
        return;
    }

    auto api = internal::w_msquic_api::api();
    api->ConfigurationClose(_handle);
    _handle = nullptr;
}

}  // namespace wolf::stream::quic
