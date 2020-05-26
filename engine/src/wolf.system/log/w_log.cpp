#include "w_log.hpp"
#include <io/w_io.h>
#include <chrono/w_timespan.h>
#include <unordered_map>
#include <time.h>
#include <cstdarg>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#ifndef MinSizeRel
#ifdef _MSC_VER
    #include "spdlog/sinks/msvc_sink.h"
#endif
#endif

////Declaration of extern objects as shared
//#if defined(__WIN32) && !defined(__STATIC_LIB)
//#pragma data_seg (".shared")
//#endif
//
//wolf::system::w_logger wolf::logger;
//std::wstring wolf::content_path;
//wolf::system::w_inputs_manager wolf::inputs_manager;
//std::map<uint32_t, float> wolf::windows_frame_time_in_sec;
//
//void wolf::release_heap_data()
//{
//	//release all loggers
//	curl_global_cleanup();
//	spdlog::drop_all();
//    logger.release();
//	content_path.clear();
//    inputs_manager.reset();
//    windows_frame_time_in_sec.clear();
//}
//
//#if defined(__WIN32) && !defined(__STATIC_LIB)
//#pragma data_seg ()
//#pragma comment(linker,"/SECTION:.shared,RWS")
//#endif

static std::unordered_map<int, std::shared_ptr<spdlog::logger>> s_loggers;
static constexpr int s_id = 0;

int w_log_init(_In_ const w_log_config* pConfig)
{
    if(!pConfig)
    {
        return -1;
    }
    //if directory of log is not existed
    if (w_io_get_is_directory(pConfig->log_path) != W_SUCCESS)
    {
        //create the directory of log inside the root directory
        w_io_create_directory(pConfig->log_path);
    }
    auto _time = w_timespan_init_from_now();
    auto _time_str = w_timespan_to_string(_time);
    auto _log_file_path = w_string_concat(3, pConfig->log_path, _time_str, ".wLog");

    std::vector<spdlog::sink_ptr> _sinks;
#if defined(_MSC_VER) && !defined(MinSizeRel)
    _sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif
    _sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(_log_file_path, true));
    _sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
    
    auto _logger = std::make_shared<spdlog::logger>(
                pConfig->app_name,
                begin(_sinks),
                end(_sinks));
    if (!_logger)
    {
        return -1;
    }
    
    //replace or create a new log
    //remember the first log is the default one
    int _log_id = -1;
    if (s_loggers.empty())
    {
        _log_id = 0;
    }
    else
    {
        constexpr int _min = 1, _max = INT_MAX - 1;
        for (size_t i = 0; i < 3; ++i)
        {
            srand(time(NULL));
            _log_id = _min + (rand() % static_cast<int>(_max - _min + 1));
            if (s_loggers.find(_log_id) == s_loggers.end())
            {
                break;
            }
        }
    }
    
    if (_log_id == -1)
    {
        return -1;
    }
    
    s_loggers[_log_id] = _logger;
    
    _logger->info(
                "Project: \"Wolf Engine(https://WolfEngine.App). "\
                "Copyright(c) Pooya Eimandar(https://PooyaEimandar.github.io). All rights reserved.\". "\
                "Contact: \"Pooya@WolfEngine.App\" "\
                "Version: {}.{}.{}.{}",
                  WOLF_MAJOR_VERSION,
                  WOLF_MINOR_VERSION,
                  WOLF_PATCH_VERSION,
                  WOLF_DEBUG_VERSION);
    
    if (pConfig->flush_level)
    {
        _logger->set_level(spdlog::level::level_enum::warn);
        _logger->flush_on(spdlog::level::level_enum::warn);
    }
    else
    {
        _logger->flush_on(spdlog::level::level_enum::info);
    }
    
    return _log_id;
}
  
W_RESULT w_log_flush(void)
{
    //get first log
    if (s_loggers.find(s_id) == s_loggers.end()) return W_FAILURE;
    s_loggers[s_id]->flush();
    return W_SUCCESS;
}

W_RESULT w_log_flush(_In_ const int pLogID)
{
    //get the logger object based on log ID
    if (s_loggers.find(pLogID) == s_loggers.end()) return W_FAILURE;
    s_loggers[pLogID]->flush();
    return W_SUCCESS;
}

W_RESULT  w_log(_In_z_ const char* pFMT)
{
    //get first log
    if (s_loggers.find(s_id) == s_loggers.end()) return W_FAILURE;
    s_loggers[s_id]->info(pFMT);
    return W_SUCCESS;
}

W_RESULT  w_log(_In_ const w_log_type pLogType, _In_z_ const char* pFMT)
{
    //get first log
    if (s_loggers.find(s_id) == s_loggers.end()) return W_FAILURE;
    switch (pLogType)
    {
        default:
        case w_log_type::W_INFO:
            s_loggers[s_id]->info(pFMT);
            break;
        case w_log_type::W_WARNING:
            s_loggers[s_id]->warn(pFMT);
            break;
        case w_log_type::W_ERROR:
            s_loggers[s_id]->error(pFMT);
            break;
    }
    return W_SUCCESS;
}

W_RESULT  w_log(_In_ const w_log_type pLogType,
                _In_ const int pLogID,
                _In_z_ const char* pFMT)
{
    if (s_loggers.find(pLogID) == s_loggers.end()) return W_FAILURE;
    switch (pLogType)
    {
        default:
        case w_log_type::W_INFO:
            s_loggers[pLogID]->info(pFMT);
            break;
        case w_log_type::W_WARNING:
            s_loggers[pLogID]->warn(pFMT);
            break;
        case w_log_type::W_ERROR:
            s_loggers[pLogID]->error(pFMT);
            break;
    }
    return W_SUCCESS;
}

template<typename... w_args>
W_RESULT w_log(_In_ const w_log_type pLogType, _In_z_ const char* pFMT, _In_ const w_args&... pArgs)
{
    //get first log
    if (s_loggers.find(s_id) == s_loggers.end()) return W_FAILURE;
    switch (pLogType)
    {
        default:
        case w_log_type::W_INFO:
            s_loggers[s_id]->info(pFMT, pArgs...);
            break;
        case w_log_type::W_WARNING:
            s_loggers[s_id]->warn(pFMT, pArgs...);
            break;
        case w_log_type::W_ERROR:
            s_loggers[s_id]->error(pFMT, pArgs...);
            break;
    }
    return W_SUCCESS;
}

template<typename... w_args>
W_RESULT  w_log(_In_ const w_log_type pLogType,
                _In_ const int pLogID,
                _In_z_ const char* pFMT,
                _In_ const w_args&... pArgs)
{
    if (s_loggers.find(pLogID) == s_loggers.end()) return W_FAILURE;
    switch (pLogType)
    {
        default:
        case w_log_type::W_INFO:
            s_loggers[pLogID]->info(pFMT, pArgs...);
            break;
        case w_log_type::W_WARNING:
            s_loggers[pLogID]->warn(pFMT, pArgs...);
            break;
        case w_log_type::W_ERROR:
            s_loggers[pLogID]->error(pFMT, pArgs...);
            break;
    }
    return W_SUCCESS;
}

void V(_In_ const W_RESULT pResult, _In_z_ const char* pFMT)
{
    if (pResult == W_SUCCESS) return;
    if (s_loggers.find(s_id) == s_loggers.end()) return;
    s_loggers[s_id]->info(pFMT);
}

template<typename... w_args>
void V(
    _In_    W_RESULT pResult,
    _In_z_    const char* pFMT,
    _In_    const w_args&... pArgs)
{
    if (pResult == W_SUCCESS) return;
    if (s_loggers.find(s_id) == s_loggers.end()) return;
    s_loggers[s_id]->info(pFMT, pArgs...);
}

void V(
    _In_    W_RESULT pResult,
    _In_    w_log_type pLogType,
    _In_z_  const char* pFMT)
{
    if (pResult == W_SUCCESS) return;
    if (s_loggers.find(s_id) == s_loggers.end()) return;

    switch (pLogType)
    {
    default:
    case w_log_type::W_INFO:
        s_loggers[s_id]->info(pFMT);
        break;
    case w_log_type::W_WARNING:
        s_loggers[s_id]->warn(pFMT);
        break;
    case w_log_type::W_ERROR:
        s_loggers[s_id]->error(pFMT);
        break;
    }
}

template<typename... w_args>
void V(
    _In_    W_RESULT pResult,
    _In_    w_log_type pLogType,
    _In_z_    const char* pFMT,
    _In_    const w_args&... pArgs)
{
    if (pResult == W_SUCCESS) return;
    if (s_loggers.find(s_id) == s_loggers.end()) return;

    switch (pLogType)
    {
    default:
    case w_log_type::W_INFO:
        s_loggers[s_id]->info(pFMT, pArgs...);
        break;
    case w_log_type::W_WARNING:
        s_loggers[s_id]->warn(pFMT, pArgs...);
        break;
    case w_log_type::W_ERROR:
        s_loggers[s_id]->error(pFMT, pArgs...);
        break;
    }
}

void V(
    _In_    W_RESULT pResult,
    _In_    w_log_type pLogType,
    _In_    bool pTerminateProgram,
    _In_z_  const char* pFMT)
{
    if (pResult == W_SUCCESS) return;
    if (s_loggers.find(s_id) == s_loggers.end()) return;

    switch (pLogType)
    {
    default:
    case w_log_type::W_INFO:
        s_loggers[s_id]->info(pFMT);
        break;
    case w_log_type::W_WARNING:
        s_loggers[s_id]->warn(pFMT);
        break;
    case w_log_type::W_ERROR:
        s_loggers[s_id]->error(pFMT);
        break;
    }

    if (pTerminateProgram)
    {
        wolf_terminate();
        std::exit(EXIT_FAILURE);
    }
}

template<typename... w_args>
void V(
    _In_    W_RESULT pResult,
    _In_    w_log_type pLogType,
    _In_    bool pTerminateProgram,
    _In_z_  const char* pFMT,
    _In_    const w_args&... pArgs)
{
    if (pResult == W_SUCCESS) return;
    if (s_loggers.find(s_id) == s_loggers.end()) return;

    switch (pLogType)
    {
    default:
    case w_log_type::W_INFO:
        s_loggers[s_id]->info(pFMT, pArgs...);
        break;
    case w_log_type::W_WARNING:
        s_loggers[s_id]->warn(pFMT, pArgs...);
        break;
    case w_log_type::W_ERROR:
        s_loggers[s_id]->error(pFMT, pArgs...);
        break;
    }

    if (pTerminateProgram)
    {
        wolf_terminate();
        std::exit(EXIT_FAILURE);
    }
}
