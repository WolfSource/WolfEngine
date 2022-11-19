#ifdef WOLF_SYSTEM_LOG

#include "w_log.hpp"

using w_log = wolf::system::log::w_log;
using w_log_config = wolf::system::log::w_log_config;

w_log::w_log(const w_log_config &p_config) noexcept : _config(p_config) {}

w_log::~w_log() noexcept {
  try {
    flush();
  } catch (...) {
  }
}

boost::leaf::result<int> w_log::init() {
  // first create a directory for log
  const auto _parent_path = this->_config.path.parent_path();
  const auto _is_dir = std::filesystem::is_directory(_parent_path);
  if (!_is_dir) {
    // try to create a directory for it
    const auto _dir_created = std::filesystem::create_directory(_parent_path);
    if (!_dir_created) {
      return W_ERR(std::errc::invalid_argument,
                   "could not create log directory path");
    }
  }

  // create sinks
  std::vector<spdlog::sink_ptr> _sinks;
  if (this->_config.type & w_log_sink::CONSOLE) {
    if (this->_config.multi_threaded) {
      _sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    } else {
      _sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
    }
  }

#if defined(_MSC_VER)
  if (this->_config.type & w_log_sink::VISUAL_STUDIO) {
    if (this->_config.multi_threaded) {
      _sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
    } else {
      _sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_st>());
    }
  }
#endif

  const auto _level = this->_config.level;
  const auto _flush_level = this->_config.flush_level;

  const auto _filename = this->_config.path.filename().string();
  const auto _path = this->_config.path.string();

  if (this->_config.type & w_log_sink::ASYNC_FILE) {
    // async file sink
    this->_async_file_logger =
        spdlog::create_async_nb<spdlog::sinks::rotating_file_sink_mt>(
            _filename, _path, this->_config.max_file_size_in_mb,
            this->_config.max_files, this->_config.rotate_on_open);
    // test for memory violation
    std::ignore = gsl::not_null<spdlog::logger *>(_async_file_logger.get());

    this->_async_file_logger->set_level(_level);
    this->_async_file_logger->flush_on(_flush_level);
  } else if (this->_config.type & w_log_sink::ASYNC_DAILY_FILE) {
    // async daily file sink
    this->_async_file_logger =
        spdlog::create_async_nb<spdlog::sinks::daily_file_sink_mt>(
            _filename, _path, this->_config.hour, this->_config.minute,
            false, // truncate
            gsl::narrow_cast<uint16_t>(this->_config.max_files));
    // test for memory violation
    std::ignore = gsl::not_null<spdlog::logger *>(_async_file_logger.get());

    this->_async_file_logger->set_level(_level);
    this->_async_file_logger->flush_on(_flush_level);
  }

  // create logger for other sinks
  this->_logger =
      std::make_shared<spdlog::logger>(_filename, _sinks.begin(), _sinks.end());
  // test for memory violation
  std::ignore = gsl::not_null<spdlog::logger *>(this->_logger.get());

  this->_logger->set_level(_level);
  this->_logger->flush_on(_flush_level);

  return W_SUCCESS;
}

void w_log::write(_In_ const std::string_view &p_fmt) {
  write(spdlog::level::level_enum::info, p_fmt);
}

void w_log::write(_In_ const spdlog::level::level_enum &p_level,
                  _In_ const std::string_view &p_fmt) {
  if (this->_logger != nullptr) {
    this->_logger->log(p_level, p_fmt);
  }
  if (this->_async_file_logger != nullptr) {
    this->_async_file_logger->log(p_level, p_fmt);
  }
}

//void w_log::write(_In_ const std::string_view& p_fmt) {
//  if (this->_logger != nullptr) {
//    this->_logger->log(spdlog::level::info, p_fmt);
//  }
//  if (this->_async_file_logger != nullptr) {
//    this->_async_file_logger->log(spdlog::level::info, p_fmt);
//  }
//}


//template <typename... Args>
//void w_log::write(_In_ const spdlog::level::level_enum p_log_level,
//                  _In_ const std::string_view& p_fmt, _In_ Args &&...p_args) {
//  using level_enum = spdlog::level::level_enum;
//  switch (p_log_level) {
//  default:
//  case level_enum::OFF:
//    break;
//  case level_enum::TRACE: {
//    if (this->_logger != nullptr) {
//      this->_logger->trace(p_fmt, std::forward<Args>(p_args)...);
//    }
//    if (this->_async_file_logger != nullptr) {
//      this->_async_file_logger->trace(p_fmt, std::forward<Args>(p_args)...);
//    }
//    break;
//  }
//  case level_enum::DEBUG_: {
//    if (this->_logger != nullptr) {
//      this->_logger->debug(p_fmt, std::forward<Args>(p_args)...);
//    }
//    if (this->_async_file_logger != nullptr) {
//      this->_async_file_logger->debug(p_fmt, std::forward<Args>(p_args)...);
//    }
//    break;
//  }
//  case level_enum::INFO: {
//    if (this->_logger != nullptr) {
//      this->_logger->info(p_fmt, std::forward<Args>(p_args)...);
//    }
//    if (this->_async_file_logger != nullptr) {
//      this->_async_file_logger->info(p_fmt, std::forward<Args>(p_args)...);
//    }
//    break;
//  }
//  case level_enum::WARN: {
//    if (this->_logger != nullptr) {
//      this->_logger->warn(p_fmt, std::forward<Args>(p_args)...);
//    }
//    if (this->_async_file_logger != nullptr) {
//      this->_async_file_logger->warn(p_fmt, std::forward<Args>(p_args)...);
//    }
//    break;
//  }
//  case level_enum::ERROR_: {
//    if (this->_logger != nullptr) {
//      this->_logger->error(p_fmt, std::forward<Args>(p_args)...);
//    }
//    if (this->_async_file_logger != nullptr) {
//      this->_async_file_logger->error(p_fmt, std::forward<Args>(p_args)...);
//    }
//    break;
//  }
//  case level_enum::CRITICAL: {
//    if (this->_logger != nullptr) {
//      this->_logger->critical(p_fmt, std::forward<Args>(p_args)...);
//    }
//    if (this->_async_file_logger != nullptr) {
//      this->_async_file_logger->critical(p_fmt, std::forward<Args>(p_args)...);
//    }
//    break;
//  }
//  }
//}

boost::leaf::result<int> w_log::flush() {
  const auto _logger_nn = gsl::not_null<spdlog::logger *>(this->_logger.get());
  const auto _async_file_logger_nn =
      gsl::not_null<spdlog::logger *>(this->_async_file_logger.get());

  _logger_nn->flush();
  _async_file_logger_nn->flush();

  return W_SUCCESS;
}

w_log_config w_log::get_config() const { return this->_config; }

#endif