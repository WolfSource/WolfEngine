#include "w_log.h"
#include <apr-1/apr_general.h>

#include "logger.hpp"
#include <unordered_map>
#include <cstdarg>

static std::unordered_map<int, logger*> s_loggers;

int w_log_init(_In_ const w_log_config* pConfig)
{
    auto _l = new logger(pConfig);
    if (!_l)
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

    s_loggers[_log_id] = _l;

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
    s_loggers[0]->write(pLogType, pFMT);
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
    s_loggers[pLogID]->write(pLogType, pFMT);
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

    char* _buf = (char*)w_malloc(W_MAX_BUFFER_SIZE, "LOG_P");
    if (_buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(_buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[0]->write(pLogType, _buf);

        w_free(_buf);
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

    char* _buf = (char*)w_malloc(W_MAX_BUFFER_SIZE, "LOG_P_EX");
    if (_buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(_buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[pLogID]->write(pLogType, _buf);

        w_free(_buf);
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

    s_loggers[0]->write(pLogType, pFMT);
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
    
    char* _buf = (char* )w_malloc(W_MAX_BUFFER_SIZE, "VA");
    if (_buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(_buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[0]->write(pLogType, _buf);
     
        w_free(_buf);
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
    
    char* _buf = (char* )w_malloc(W_MAX_BUFFER_SIZE, "VALIDATE");
    if (_buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(_buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[0]->write(pLogType, _buf);
        
        w_free(_buf);
    }
    
    if (pTerminateProgram)
    {
        wolf_terminate();
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
    
    char* _buf = (char* )w_malloc(W_MAX_BUFFER_SIZE, "VALIDATE_EX");
    if (_buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(_buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[pLogID]->write(pLogType, _buf);
    
        w_free(_buf);
    }
    
    if (pTerminateProgram)
    {
        wolf_terminate();
        std::exit(EXIT_FAILURE);
    }
}

W_RESULT  w_log_flush(void)
{
    //get first log
    if (!s_loggers.size()) return W_FAILURE;
    return s_loggers[0]->flush();
}

W_RESULT  w_log_flush_ex(_In_ int pLogID)
{
    //get the logger object based on log ID
    if (s_loggers.find(pLogID) == s_loggers.end()) return W_FAILURE;
    return s_loggers[pLogID]->flush();
}

W_RESULT  w_log_terminate()
{
    for (auto i = 0; i < s_loggers.size(); ++i)
    {
        auto _ptr = s_loggers[i];
        if (_ptr)
        {
            delete _ptr;
            _ptr = nullptr;
        }
    }
    s_loggers.clear();
    return W_SUCCESS;
}

W_RESULT  w_log_terminate_ex(_In_ int pLogID)
{
    //get the logger object based on log ID
    auto _iter = s_loggers.find(pLogID);
    if (_iter == s_loggers.end()) return W_FAILURE;

    auto _ptr = s_loggers[pLogID];
    if (_ptr)
    {
        delete _ptr;
        _ptr = nullptr;
    }
    s_loggers.erase(_iter);

    return W_SUCCESS;
}
