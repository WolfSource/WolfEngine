//#ifdef WOLF_SYSTEM_LOG
//
//#include "w_log.hpp"
//
//#include <DISABLE_ANALYSIS_BEGIN>
//
//#include <spdlog/spdlog.h>
//#include <spdlog/async.h>
//#include <spdlog/sinks/basic_file_sink.h>
//#include <spdlog/sinks/stdout_color_sinks.h>
//#include <spdlog/sinks/rotating_file_sink.h>
//#include <spdlog/sinks/daily_file_sink.h>
//
//#ifdef _MSC_VER
//#include <spdlog/sinks/msvc_sink.h>
//#endif
//
//#include <DISABLE_ANALYSIS_END>
//
//using w_log = wolf::system::log::w_log;
//using w_log_config = wolf::system::log::w_log_config;
//
//struct w_log::priv
//{
//    std::shared_ptr<spdlog::logger> logger = nullptr;
//    std::shared_ptr<spdlog::logger> async_file_logger = nullptr;
//};
//
//w_log::w_log(_In_ w_log_config p_log_config) noexcept :
//    _config(p_log_config),
//    _priv(std::make_unique<priv>())
//{
//}
//
//w_log::~w_log()
//{
//    try
//    {
//        flush();
//    }
//    catch (...)
//    {
//
//    }
//}
//
//tl::expected<void, W_RESULT> w_log::init()
//{
//    const auto TRACE = "w_log::init";
//
//    const auto _parent_path = this->_config.path.parent_path();
//    const auto _is_dir = std::filesystem::is_directory(_parent_path);
//    if (!_is_dir)
//    {
//        // try to create a directory for it
//        const auto _dir_created = std::filesystem::create_directory(_parent_path);
//        if (!_dir_created)
//        {
//            return tl::unexpected(W_RESULT::BAD_CONFIG);
//        }
//    }
//
//    std::vector<spdlog::sink_ptr> _sinks;
//    if (this->_config.type & w_log_sink::CONSOLE)
//    {
//        if (this->_config.multi_threaded)
//        {
//            _sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
//        }
//        else
//        {
//            _sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
//        }
//    }
//
//#if defined(_MSC_VER)
//    if (this->_config.type & w_log_sink::VISUAL_STUDIO)
//    {
//        if (this->_config.multi_threaded)
//        {
//            _sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
//        }
//        else
//        {
//            _sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_st>());
//        }
//    }
//#endif
//
//    auto _level = gsl::narrow_cast<spdlog::level::level_enum>(this->_config.level);
//    auto _flush_level = gsl::narrow_cast<spdlog::level::level_enum>(this->_config.flush_level);
//
//    const auto _name = this->_config.path.filename();
//    const auto _name_string = _name.string();
//    if (this->_config.type & w_log_sink::ASYNC_FILE)
//    {
//        // async file sink
//        this->_priv->async_file_logger = spdlog::create_async_nb<spdlog::sinks::rotating_file_sink_mt>(
//            _name_string,
//            this->_config.path,
//            this->_config.max_file_size_in_mb,
//            this->_config.max_files,
//            this->_config.rotate_on_open);
//        if (this->_priv->async_file_logger == nullptr)
//        {
//            return tl::unexpected(W_RESULT::FAILURE);
//        }
//
//        this->_priv->async_file_logger->set_level(_level);
//        this->_priv->async_file_logger->flush_on(_flush_level);
//    }
//    else if (this->_config.type & w_log_sink::ASYNC_DAILY_FILE)
//    {
//        // async daily file sink
//        this->_priv->async_file_logger = spdlog::create_async_nb<spdlog::sinks::daily_file_sink_mt>(
//            _name_string,
//            this->_config.path,
//            this->_config.hour,
//            this->_config.minute,
//            false, // truncate
//            gsl::narrow_cast<uint16_t>(this->_config.max_files));
//        if (this->_priv->async_file_logger == nullptr)
//        {
//            return tl::unexpected(W_RESULT::FAILURE);
//        }
//
//        this->_priv->async_file_logger->set_level(_level);
//        this->_priv->async_file_logger->flush_on(_flush_level);
//    }
//
//    // other sinks 
//    this->_priv->logger = std::make_shared<spdlog::logger>(
//        _name_string,
//        _sinks.begin(),
//        _sinks.end());
//    if (this->_priv->logger == nullptr)
//    {
//        return tl::unexpected(W_RESULT::FAILURE);
//    }
//
//    this->_priv->logger->set_level(_level);
//    this->_priv->logger->flush_on(_flush_level);
//
//    W_EXPECTED_SUCCESS
//}
//
//template<typename... Args>
//tl::expected<void, W_RESULT> w_log::write(
//    _In_z_ std::string_view p_fmt,
//    _In_ Args &&... p_args)
//{
//    if (this->_priv == nullptr)
//    {
//        return tl::unexpected(W_RESULT::BAD_ARG);
//    }
//    
//    if (this->_priv->logger != nullptr)
//    {
//        this->_priv->logger->debug(spdlog::level::info, p_fmt, std::forward<Args>(p_args)...);
//    }
//    if (this->_priv->async_file_logger != nullptr)
//    {
//        this->_priv->async_file_logger->debug(spdlog::level::info, p_fmt, std::forward<Args>(p_args)...);
//    }
//
//    W_EXPECTED_SUCCESS
//}
//
//template<typename... Args>
//tl::expected<void, W_RESULT> w_log::write(
//    _In_ const w_log_level p_log_level,
//    _In_z_ std::string_view p_fmt,
//    _In_ Args &&... p_args)
//{
//    if (this->_priv == nullptr)
//    {
//        return tl::unexpected(W_RESULT::BAD_ARG);
//    }
//
//    switch (p_log_level)
//    {
//    default:
//    case w_log_level::OFF:
//        break;
//    case w_log_level::TRACE:
//    {
//        if (this->_priv->logger != nullptr)
//        {
//            this->_priv->logger->trace(p_fmt, std::forward<Args>(p_args)...);
//        }
//        if (this->_priv->async_file_logger != nullptr)
//        {
//            this->_priv->async_file_logger->trace(p_fmt, std::forward<Args>(p_args)...);
//        }
//        break;
//    }
//    case w_log_level::DEBUG_:
//    {
//        if (this->_priv->logger != nullptr)
//        {
//            this->_priv->logger->debug(p_fmt, std::forward<Args>(p_args)...);
//        }
//        if (this->_priv->async_file_logger != nullptr)
//        {
//            this->_priv->async_file_logger->debug(p_fmt, std::forward<Args>(p_args)...);
//        }
//        break;
//    }
//    case w_log_level::INFO:
//    {
//        if (this->_priv->logger != nullptr)
//        {
//            this->_priv->logger->info(p_fmt, std::forward<Args>(p_args)...);
//        }
//        if (this->_priv->async_file_logger != nullptr)
//        {
//            this->_priv->async_file_logger->info(p_fmt, std::forward<Args>(p_args)...);
//        }
//        break;
//    }
//    case w_log_level::WARN:
//    {
//        if (this->_priv->logger != nullptr)
//        {
//            this->_priv->logger->warn(p_fmt, std::forward<Args>(p_args)...);
//        }
//        if (this->_priv->async_file_logger != nullptr)
//        {
//            this->_priv->async_file_logger->warn(p_fmt, std::forward<Args>(p_args)...);
//        }
//        break;
//    }
//    case w_log_level::ERROR_:
//    {
//        if (this->_priv->logger != nullptr)
//        {
//            this->_priv->logger->error(p_fmt, std::forward<Args>(p_args)...);
//        }
//        if (this->_priv->async_file_logger != nullptr)
//        {
//            this->_priv->async_file_logger->error(p_fmt, std::forward<Args>(p_args)...);
//        }
//        break;
//    }
//    case w_log_level::CRITICAL:
//    {
//        if (this->_priv->logger != nullptr)
//        {
//            this->_priv->logger->critical(p_fmt, std::forward<Args>(p_args)...);
//        }
//        if (this->_priv->async_file_logger != nullptr)
//        {
//            this->_priv->async_file_logger->critical(p_fmt, std::forward<Args>(p_args)...);
//        }
//        break;
//    }
//    }
//
//    W_EXPECTED_SUCCESS
//}
//
//tl::expected<void, W_RESULT> w_log::flush()
//{
//    if (this->_priv == nullptr)
//    {
//        return tl::unexpected(W_RESULT::BAD_ARG);
//    }
//
//    if (this->_priv->logger != nullptr)
//    {
//        this->_priv->logger->flush();
//    }
//    if (this->_priv->async_file_logger != nullptr)
//    {
//        this->_priv->async_file_logger->flush();
//    }
//
//    W_EXPECTED_SUCCESS
//}
//
//w_log_config w_log::get_config() const
//{
//    return this->_config;
//}
//
//#endif