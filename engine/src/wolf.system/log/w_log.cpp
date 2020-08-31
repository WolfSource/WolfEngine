#include "w_log.h"
#include <concurrency/w_thread.h>

#include "logger.hpp"
#include <unordered_map>
#include <cstdarg>

typedef struct
{
    char*           buf;//buffer 
    logger*         log;//logger
    w_mutex         mutex;//mutex
    w_mem_pool      mem_pool;//memory pool
}w_logger;
static std::unordered_map<int, w_logger*> s_loggers;

int w_log_init(
    _In_ const w_log_config* pConfig)
{
    int _log_id = -1;
    const char* _trace_info = "w_log_init";

    //create a w_mem_pool
    w_mem_pool _mem_pool;
    w_mem_pool_init(&_mem_pool, W_MEM_POOL_FAST_EXTEND);
    if (!_mem_pool)
    {
        W_ASSERT_P(false, "could not create memory pool for w_logger. trace info: %s", _trace_info);
        return -1;
    }

    auto _w_logger = (w_logger*)w_malloc(_mem_pool, sizeof(w_logger));
    if (!_w_logger)
    {
        W_ASSERT_P(
            false,
            "could not allocate memory from pool for _w_logger. trace info: %s",
            _trace_info);
        goto out;
    }
    _w_logger->mem_pool = _mem_pool;

    //create a log file
    _w_logger->log = (logger*)w_malloc(_mem_pool, sizeof(logger));
    if (!_w_logger->log)
    {
        W_ASSERT_P(
            false,
            "could not allocate memory from pool for _w_logger->l. trace info: %s",
            _trace_info);
        goto out;
    }
    //initialize the log file
    if (_w_logger->log->init(_mem_pool, pConfig) != W_SUCCESS)
    {
        W_ASSERT_P(
            false,
            "could not initialize _w_logger->log. trace info: %s",
            _trace_info);
        goto out;
    }

    _w_logger->buf = (char*)w_malloc(_mem_pool, W_MAX_BUFFER_SIZE);
    if (_w_logger->buf)
    {
        W_ASSERT_P(
            false,
            "could not allocate memory from pool for _w_logger->buf. trace info: %s",
            _trace_info);
        goto out;
    }

    //create a mutex
    w_thread_mutex_init(
        &_w_logger->mutex,
        0x0,
        _mem_pool);
    if (!_w_logger->mutex)
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
                w_thread_mutex_fini(_w_logger->mutex);
                _w_logger->mutex = nullptr;
            }
            if (_w_logger->mem_pool)
            {
                w_mem_pool_fini(&_w_logger->mem_pool);
                _w_logger->mem_pool = nullptr;
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

W_RESULT  w_log_fini_all()
{
    for (auto i = 0; i < s_loggers.size(); ++i)
    {
        auto _ptr = s_loggers[i];
        if (_ptr)
        {
            if (_ptr->mutex)
            {
                //release mutex
                w_thread_mutex_fini(_ptr->mutex);
                _ptr->mutex = nullptr;
            }

            if (_ptr->mem_pool)
            {
                //release memory pool
                w_mem_pool_fini(&_ptr->mem_pool);
                _ptr->mem_pool = nullptr;
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
            w_thread_mutex_fini(_ptr->mutex);
            _ptr->mutex = nullptr;
        }

        if (_ptr->mem_pool)
        {
            //release memory pool
            w_mem_pool_fini(&_ptr->mem_pool);
            _ptr->mem_pool = nullptr;
        }
    }

    s_loggers.erase(_iter);

    return W_SUCCESS;
}
