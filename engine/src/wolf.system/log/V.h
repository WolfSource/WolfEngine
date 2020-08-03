/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
    Name			 : w_log.h
    Description		 : a logger class which is shared over all dlls
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>
#include "w_log_config.h"

#ifdef W_PLATFORM_ANDROID

#include <errno.h>
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Wolf_Engine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Wolf_Engine", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Wolf_Engine", __VA_ARGS__))

#endif

/**
 * initialize the log file and create a log file inside a Log folder
 * @param pConfig is the configuration of log
 * @return log stream ID
*/
W_SYSTEM_EXPORT
int  V_INIT(_In_ const w_log_config* pConfig);

/**
 * Validate W_RESULT and write a message in to the default log file if the W_RESULT was not equal to W_SUCCESS
 * @param pResult result
 * @param pLogType type of the log
 * @param pFMT fmt
*/
W_SYSTEM_EXPORT
void V(_In_     const W_RESULT pResult,
       _In_     w_log_type pLogType,
       _In_z_   const char* pFMT);

/**
 * Validate W_RESULT and write a message in to the default log file if the W_RESULT was not equal to W_SUCCESS
 * @param pResult result
 * @param pLogType type of the log
 * @param pFMT fmt
*/
W_SYSTEM_EXPORT
void VA(
	_In_	    W_RESULT pResult,
    _In_        w_log_type pLogType,
	_In_z_	    const char* pFMT,
    _In_        ...);

/**
 * Validate W_RESULT and write a message in to the default log file if the W_RESULT was not equal to W_SUCCESS
 * @param pResult result
 * @param pLogType type of the log
 * @param pTerminateProgram terminate the program
 * @param pFMT fmt
*/
W_SYSTEM_EXPORT
void VALIDATE(
    _In_        W_RESULT pResult,
    _In_        w_log_type pLogType,
    _In_        bool pTerminateProgram,
    _In_z_      const char* pFMT,
    _In_        ...);

/**
 * flush the log file
*/
W_SYSTEM_EXPORT
W_RESULT  V_FLUSH(void);

/**
 * flush the specific log file
*/
W_SYSTEM_EXPORT
W_RESULT  V_FLUSH_EX(_In_ int pLogID);

#ifdef __cplusplus
}
#endif
