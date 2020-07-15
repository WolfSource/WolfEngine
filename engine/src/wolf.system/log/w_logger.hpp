/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
    Name			 : w_log.h
    Description		 : a logger class which is shared over all dlls
    Comment          :
*/

#pragma once

#include <wolf.h>
#include <memory>
#include "w_log_config.h"
#include "spdlog/spdlog.h"

class w_logger
{
public:
    W_SYSTEM_EXPORT
    explicit w_logger(_In_ const w_log_config* pConfig);

    W_SYSTEM_EXPORT
    ~w_logger(void) {};
    
    W_SYSTEM_EXPORT
    W_RESULT  write(_In_z_ const char* pFMT);
    
    W_SYSTEM_EXPORT
    W_RESULT  write(_In_ const w_log_type pLogType,
                    _In_z_ const char* pFMT);
    
    W_SYSTEM_EXPORT
    W_RESULT  write(_In_ const w_log_type pLogType,
                    _In_ const int pLogID,
                    _In_z_ const char* pFMT);
    
    template<typename... w_args>
    W_SYSTEM_EXPORT
    W_RESULT  write(_In_ const w_log_type pLogType,
                    _In_z_ const char* pFMT,
                    _In_ const w_args&... pArgs);

    template<typename... w_args>
    W_SYSTEM_EXPORT
    W_RESULT  write(_In_ const w_log_type pLogType,
                    _In_ const int pLogID,
                    _In_z_ const char* pFMT,
                    _In_ const w_args&... pArgs);
    
    W_SYSTEM_EXPORT
    W_RESULT  flush(void);
    
    W_SYSTEM_EXPORT
    W_RESULT  flush(_In_ int pLogID);
    
private:
    std::shared_ptr<spdlog::logger> _l;
};
