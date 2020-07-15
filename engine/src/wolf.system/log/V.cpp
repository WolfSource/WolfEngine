#include "V.h"
#include <apr-1/apr_general.h>

#include "w_logger.hpp"
#include <unordered_map>
#include <cstdarg>

static std::unordered_map<int, w_logger*> s_loggers;

int V_INIT(_In_ const w_log_config* pConfig)
{
    auto _l = (w_logger*)w_malloc(sizeof(w_logger), "w_log_init");
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

void V(_In_     const W_RESULT pResult,
       _In_     w_log_type pLogType,
       _In_z_   const char* pFMT)
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
        W_ASSERT(false, "default logger could not find. trace info: w_log::V");
        return;
    }
    
    char* _buf = (char* )w_malloc(W_MAX_BUFFER_SIZE, "VA");
    if (_buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(_buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[0]->write(_buf);
     
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
        W_ASSERT(false, "default logger could not find. trace info: w_log::V");
        return;
    }
    
    char* _buf = (char* )w_malloc(W_MAX_BUFFER_SIZE, "VALIDATE");
    if (_buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(_buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[0]->write(_buf);
        
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
    _In_        const int pLogID,
    _In_        w_log_type pLogType,
    _In_        bool pTerminateProgram,
    _In_z_      const char* pFMT,
    _In_        ...)
{
    if (pResult == W_SUCCESS) return;
    
    if (s_loggers.find(pLogID) == s_loggers.end())
    {
        W_ASSERT(false, "logger could not find. trace info: w_log::V");
        return;
    }
    
    char* _buf = (char* )w_malloc(W_MAX_BUFFER_SIZE, "VALIDATE_EX");
    if (_buf)
    {
        va_list _arg_ptr;
        va_start(_arg_ptr, pFMT);
        vsnprintf(_buf, W_MAX_BUFFER_SIZE - 1, pFMT, _arg_ptr);
        va_end(_arg_ptr);

        s_loggers[pLogID]->write(_buf);
    
        w_free(_buf);
    }
    
    if (pTerminateProgram)
    {
        wolf_terminate();
        std::exit(EXIT_FAILURE);
    }
}

W_RESULT  V_FLUSH(void)
{
    //get first log
    if (!s_loggers.size()) return W_FAILURE;
    return s_loggers[0]->flush();
}

W_RESULT  V_FLUSH_EX(_In_ int pLogID)
{
    //get the logger object based on log ID
    if (s_loggers.find(pLogID) == s_loggers.end()) return W_FAILURE;
    return s_loggers[pLogID]->flush();
}
