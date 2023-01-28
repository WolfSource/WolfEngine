/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef WOLF_SYSTEM_LOG

#include "w_log_config.hpp"
#include <wolf/wolf.hpp>

#include <DISABLE_ANALYSIS_BEGIN>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef _MSC_VER
#include <spdlog/sinks/msvc_sink.h>
#endif

#include <DISABLE_ANALYSIS_END>

namespace wolf::system::log {
class w_log {
public:
  // constructor
  W_API explicit w_log(const w_log_config &p_config) noexcept;

  // move constructor.
  W_API w_log(w_log &&p_other) = default;
  // move assignment operator.
  W_API w_log &operator=(w_log &&p_other) = default;

  // destructor
  W_API virtual ~w_log() noexcept;

  W_API boost::leaf::result<int> init();

  W_API void write(_In_ const std::string_view &p_fmt);

  W_API void write(_In_ const spdlog::level::level_enum &p_level,
                   _In_ const std::string_view &p_fmt);

#ifdef __clang__
  template <class... Args>
  W_API void write(_In_ const fmt::v9::format_string<Args...> p_fmt,
                   _In_ Args &&...p_args) {
    const auto _str =
        fmt::v9::vformat(p_fmt, fmt::v9::make_format_args(p_args...));
    write(_str);
  }

  template <class... Args>
  W_API void write(_In_ const spdlog::level::level_enum &p_level,
                   _In_ const fmt::v9::format_string<Args...> p_fmt,
                   _In_ Args &&...p_args) {
    const auto _str =
        fmt::v9::vformat(p_fmt, fmt::v9::make_format_args(p_args...));
    write(p_level, _str);
  }

#else

  template <class... Args>
  W_API void write(_In_ const std::string_view p_fmt, _In_ Args &&...p_args) {
    const auto _str =
        std::vformat(p_fmt, std::make_format_args(p_args...));
    write(_str);
  }

  template <class... Args>
  W_API void write(_In_ const spdlog::level::level_enum &p_level,
                   _In_ const std::string_view p_fmt, _In_ Args &&...p_args) {
    const auto _str = std::vformat(p_fmt, std::make_format_args(p_args...));
    write(p_level, _str);
  }

#endif // __clang__

  W_API boost::leaf::result<int> flush();

  W_API w_log_config get_config() const;

private:
  // disable copy constructor
  w_log(const w_log &) = delete;
  // disable copy operator
  w_log &operator=(const w_log &) = delete;

  w_log_config _config = {};
  std::shared_ptr<spdlog::logger> _logger = nullptr;
  std::shared_ptr<spdlog::logger> _async_file_logger = nullptr;
};
} // namespace wolf::system::log

#endif