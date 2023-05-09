#pragma once

#include "wolf.hpp"

#include "stream/quic/internal/common.hpp"

#include <msquic.h>

#include <string>

namespace wolf::stream::quic {

/**
 * @brief equivalent enum to QUIC_EXECUTION_PROFILE enum.
 *
 * Used with registration config to specify aimed execution profile.
 */
enum class w_execution_profile {
    LowLatency = QUIC_EXECUTION_PROFILE_LOW_LATENCY,
    MaxThroughput = QUIC_EXECUTION_PROFILE_TYPE_MAX_THROUGHPUT,
    Scavenger = QUIC_EXECUTION_PROFILE_TYPE_SCAVENGER,
    RealTime = QUIC_EXECUTION_PROFILE_TYPE_REAL_TIME
};

/**
 * @brief wrapper around QUIC_REGISTRATION_CONFIG.
 */
class w_registration_config {
    friend class internal::w_raw_access;

public:
    /**
     * @brief construct simple registration config.
     */
    w_registration_config(std::string p_app_name,
                          w_execution_profile p_profile = w_execution_profile::LowLatency) noexcept
        : _app_name(std::move(p_app_name))
    {
        _config.AppName = _app_name.c_str();
        _config.ExecutionProfile = static_cast<QUIC_EXECUTION_PROFILE>(p_profile);
    }

    /**
     * @brief get app name.
     */
    [[nodiscard]] const std::string& get_app_name() const noexcept
    {
        return _app_name;
    }

    /**
     * @brief get execution profile.
     */
    [[nodiscard]] w_execution_profile get_execution_profile() const noexcept
    {
        return static_cast<w_execution_profile>(_config.ExecutionProfile);
    }

    /**
     * @brief set execution profile.
     */
    void set_execution_profile(w_execution_profile p_profile) noexcept
    {
        _config.ExecutionProfile = static_cast<QUIC_EXECUTION_PROFILE>(p_profile);
    }

private:
    auto raw() noexcept { return &_config; }
    auto raw() const noexcept { return &_config; }

    QUIC_REGISTRATION_CONFIG _config = { 0 };
    const std::string _app_name;
};

}  // namespace wolf::stream::quic
