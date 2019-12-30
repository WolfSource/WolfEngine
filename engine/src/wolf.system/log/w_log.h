/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_logger.h
    Description      : logger based on zlog https://github.com/HardySimpson/zlog
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

typedef struct
{
    char*   app_name;
    char*   log_path;
    bool    log_to_console;
} w_log_config;

/**
 * initialize log file
 * @return return
 */
W_RESULT w_log_init(_In_ const w_log_config* pConfig);

/**
 * write an info to log file
*/
void w_log_write(_In_ const char* fmt, ...);

/**
 * write a warning to log file
*/
void w_log_warning(_In_ const char* fmt, ...);

/**
 * write an error to log file
*/
void w_log_error(_In_ const char* fmt, ...);

/**
 * terminate log file
*/
void w_log_terminate(void);

#ifdef __cplusplus
}
#endif
