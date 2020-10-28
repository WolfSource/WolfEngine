#include "w_log.h"
#include <concurrency/w_mutex.h>

#include "logger.hpp"
#include <unordered_map>
#include <cstdarg>

typedef struct
{
    char*           buf;//buffer 
    logger*         log;//logger
    w_mutex         mutex;//mutex
}w_logger;
static std::unordered_map<int, w_logger*> s_loggers;

int w_log_init(
    _Inout_ w_mem_pool pMemPool,
    _In_ const w_log_config* pConfig)
{
    if (!pMemPool)
    {
        return -1;
    }

    int _log_id = -1;
    const char* _trace_info = "w_log_init";

    auto _w_logger = (w_logger*)w_malloc(pMemPool, sizeof(w_logger));
    if (!_w_logger)
    {
        W_ASSERT_P(
            false,
            "could not allocate memory from pool for _w_logger. trace info: %s",
            _trace_info);
        goto out;
    }

    //create a log file
    _w_logger->log = new logger(pMemPool, pConfig);
    if (!_w_logger->log)
    {
        W_ASSERT_P(
            false,
            "could not allocate memory from pool for _w_logger->l. trace info: %s",
            _trace_info);
        goto out;
    }

    _w_logger->buf = (char*)w_malloc(pMemPool, W_MAX_BUFFER_SIZE);
    if (!_w_logger->buf)
    {
        W_ASSERT_P(
            false,
            "could not allocate memory from pool for _w_logger->buf. trace info: %s",
            _trace_info);
        goto out;
    }

    //create a mutex
    if (w_mutex_init(
        pMemPool,
        &_w_logger->mutex,
        0))
    {
        W_ASSERT_P(
            false,
            "could not create mutex for _w_logger->mutex. trace info: %s",
            _trace_info);
        goto out;
    }

    //replace or create a new log
    //remember the first log is the default one
    if (s_loggers.empty())
    {
        _log_id = 0;
    }
    else
    {
        constexpr int _min = 1, _max = INT_MAX - 1;
        for (size_t i = 0; i < 3; ++i)
        {
            srand((unsigned int)time(NULL));
            _log_id = _min + (rand() % static_cast<int>(_max - _min + 1));
            if (s_loggers.find(_log_id) == s_loggers.end())
            {
                break;
            }
        }
    }

out:
    if (_log_id == -1)
    {
        //release logger
        if (_w_logger)
        {
            if (_w_logger->mutex)
            {
                w_mutex_fini(&_w_logger->mutex);
            }
            if (_w_logger->log)
            {
                delete  _w_logger->log;
                _w_logger->log = nullptr;
            }
            _w_logger = nullptr;
        }
    }
    else
    {
        s_loggers[_log_id] = _w_logger;
    }

    return _log_id;
}

void LOG(
    _In_     w_log_type pLogType,
    _In_z_   const char* pFMT)
{
    if (!s_loggers.size())
    {
        W_ASSERT(false, "default logger could not find. trace info: w_log::LOG");
        return;
    }
    if (s_loggers[0] && s_loggers[0]->log)
    {
        s_loggers[0]->log->write(pLogType, pFMT);
    }
}

void LOG_EX(
    _In_     w_log_type pLogType,
    _In_     int pLogID,
    _In_z_   const char* pFMT)
{
    if (s_loggers.find(pLogID) == s_loggers.end())
    {
        W_ASSERT(false, "logger could not find. trace info: w_log::LOG_EX");
        return;
    }

    if (s_loggers[0] && s_loggers[0]->log)
    {
        s_loggers[0]->log->write(pLogType, pFMT);
    }
}

void LOG_P(
    _In_     w_log_type pLogType,
    _In_z_   const char* pFMT,
    _In_     ...)
{
    if (!s_loggers.size())
    {
        W_ASSERT(false, "default logger could not find. trace info: w_log::LOG_P");
        return;
    }

    if (s_loggers[0] &&
        s_loggers[0]->log &&
        s_loggers[0]->buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(s_loggers[0]->buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[0]->log->write(pLogType, s_loggers[0]->buf);
    }
}

void LOG_P_EX(
    _In_     w_log_type pLogType,
    _In_     int pLogID,
    _In_z_   const char* pFMT,
    _In_     ...)
{
    if (s_loggers.find(pLogID) == s_loggers.end())
    {
        W_ASSERT(false, "logger could not find. trace info: w_log::LOG_P_EX");
        return;
    }

    if (s_loggers[pLogID] &&
        s_loggers[pLogID]->log &&
        s_loggers[pLogID]->buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(s_loggers[pLogID]->buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[pLogID]->log->write(pLogType, s_loggers[pLogID]->buf);
    }
}

void V(
    _In_        W_RESULT pResult,
    _In_        w_log_type pLogType,
    _In_z_      const char* pFMT)
{
    if (pResult == W_SUCCESS) return;
    if (!s_loggers.size())
    {
        W_ASSERT(false, "default logger could not find. trace info: w_log::V");
        return;
    }

    if (s_loggers[0] && s_loggers[0]->log)
    {
        s_loggers[0]->log->write(pLogType, pFMT);
    }
}

void VA(
    _In_        W_RESULT pResult,
    _In_        w_log_type pLogType,
    _In_z_      const char* pFMT,
    _In_        ...)
{
    if (pResult == W_SUCCESS) return;
    if (!s_loggers.size())
    {
        W_ASSERT(false, "default logger could not find. trace info: w_log::VA");
        return;
    }

    if (s_loggers[0] &&
        s_loggers[0]->log &&
        s_loggers[0]->buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(s_loggers[0]->buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[0]->log->write(pLogType, s_loggers[0]->buf);
    }
}

void VALIDATE(
    _In_        W_RESULT pResult,
    _In_        w_log_type pLogType,
    _In_        bool pTerminateProgram,
    _In_z_      const char* pFMT,
    _In_        ...)
{
    if (pResult == W_SUCCESS) return;
    if (!s_loggers.size())
    {
        W_ASSERT(false, "default logger could not find. trace info: w_log::VALIDATE");
        return;
    }
    
    if (s_loggers[0] &&
        s_loggers[0]->log &&
        s_loggers[0]->buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(s_loggers[0]->buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[0]->log->write(pLogType, s_loggers[0]->buf);
    }
    
    if (pTerminateProgram)
    {
        wolf_fini();
        std::exit(EXIT_FAILURE);
    }
}

void VALIDATE_EX(
    _In_        W_RESULT pResult,
    _In_        int pLogID,
    _In_        w_log_type pLogType,
    _In_        bool pTerminateProgram,
    _In_z_      const char* pFMT,
    _In_        ...)
{
    if (pResult == W_SUCCESS) return;
    
    if (s_loggers.find(pLogID) == s_loggers.end())
    {
        W_ASSERT(false, "logger could not find. trace info: w_log::VALIDATE_EX");
        return;
    }
    
    if (s_loggers[pLogID] &&
        s_loggers[pLogID]->log &&
        s_loggers[pLogID]->buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(s_loggers[pLogID]->buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[pLogID]->log->write(pLogType, s_loggers[pLogID]->buf);
    }
    
    if (pTerminateProgram)
    {
        wolf_fini();
        std::exit(EXIT_FAILURE);
    }
}

W_RESULT  w_log_flush(void)
{
    //get first log
    if (!s_loggers.size()) return W_FAILURE;
   
    if (s_loggers[0] && s_loggers[0]->log)
    {
        return s_loggers[0]->log->flush();
    }
    return W_FAILURE;
}

W_RESULT  w_log_flush_ex(_In_ int pLogID)
{
    //get the logger object based on log ID
    if (s_loggers.find(pLogID) == s_loggers.end()) return W_FAILURE;
    
    if (s_loggers[pLogID] && s_loggers[pLogID]->log)
    {
        return s_loggers[pLogID]->log->flush();
    }
    return W_FAILURE;
}

W_RESULT  w_log_fini()
{
    for (auto i = 0; i < s_loggers.size(); ++i)
    {
        auto _ptr = s_loggers[i];
        if (_ptr)
        {
            if (_ptr->mutex)
            {
                //release mutex
                w_mutex_fini(&_ptr->mutex);
            }
            if (_ptr->log)
            {
                //release logger
                delete  _ptr->log;
                _ptr->log = nullptr;
            }
        }
    }
    s_loggers.clear();
    return W_SUCCESS;
}

W_RESULT  w_log_fini_ex(_In_ int pLogID)
{
    //get the logger object based on log ID
    auto _iter = s_loggers.find(pLogID);
    if (_iter == s_loggers.end()) return W_FAILURE;

    auto _ptr = s_loggers[pLogID];
    if (_ptr)
    {
        if (_ptr->mutex)
        {
            //release mutex
            w_mutex_fini(&_ptr->mutex);
        }

        if (_ptr->log)
        {
            //release logger
            delete  _ptr->log;
            _ptr->log = nullptr;
        }
    }

    s_loggers.erase(_iter);

    return W_SUCCESS;
}
