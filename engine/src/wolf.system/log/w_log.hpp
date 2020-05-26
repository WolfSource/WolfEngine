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

#ifdef W_PLATFORM_ANDROID

#include <errno.h>
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Wolf_Engine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Wolf_Engine", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Wolf_Engine", __VA_ARGS__))

#endif

typedef enum w_log_type
{
    W_INFO = 0,
    W_WARNING,
    W_ERROR
} w_log_type;

typedef struct w_log_config
{
	char*	            app_name = nullptr;
	char*	            log_path = nullptr;
	enum w_log_type	    flush_level = w_log_type::W_INFO;
	bool		        log_to_std_out = true;
} w_log_config;

/**
 * initialize the log file and create a log file inside a Log folder
 * @param pConfig is the configuration of log
 * @return log stream ID
*/
int  w_log_init(_In_ const w_log_config* pConfig);

/**
 * flush the specific log file
 * @return result
*/
W_RESULT  w_log_flush(void);


/**
 * flush the specific log file
 * @param pLogID log stream ID
 * @return result
*/
W_RESULT  w_log_flush(_In_ const int pLogID);

/**
 * write to default log file
 * @param pFMT fmt
 * @return result
*/
W_RESULT  w_log(_In_z_ const char* pFMT);


/**
 * write to default log file
 * @param pLogType type of log
        W_INFO,
        W_WARNING,
        W_ERROR
 * @param pFMT fmt
 * @return result
*/
W_RESULT  w_log(_In_ const w_log_type pLogType, _In_z_ const char* pFMT);

/**
 * write to specific log file
 * @param pLogType type of log
        W_INFO,
        W_WARNING,
        W_ERROR
 * @param pLogID log id
 * @param pFMT fmt
 * @return result
*/
W_RESULT  w_log(_In_ const w_log_type pLogType,
                _In_ const int pLogID,
                _In_z_ const char* pFMT);

/**
 * write to default log file
 * @param pLogType type of log
        W_INFO,
        W_WARNING,
        W_ERROR
 * @param pFMT fmt
 * @param pArgs are the argumans of fmt
 * @return result
*/
template<typename... w_args>
W_RESULT  w_log(_In_ const w_log_type pLogType, _In_z_ const char* pFMT, _In_ const w_args&... pArgs);

/**
 * write to default log file
 * @param pLogType type of log
        W_INFO,
        W_WARNING,
        W_ERROR
 * @param pLogID log id
 * @param pFMT fmt
 * @param pArgs are the argumans of fmt
 * @return result
*/
template<typename... w_args>
W_RESULT  w_log(_In_ const w_log_type pLogType,
                _In_ const int pLogID,
                _In_z_ const char* pFMT,
                _In_ const w_args&... pArgs);

#pragma region VALIDATE

/**
 * Validate W_RESULT and write in to the default log file
 * @param pResult result
 * @param pFMT fmt
*/
void V(_In_ const W_RESULT pResult, _In_z_ const char* pFMT);

/**
 * Validate W_RESULT and write in to the default log file
 * @param pResult result
 * @param pFMT fmt
 * @param pArgs argumans
*/
template<typename... w_args>
void V(
	_In_	W_RESULT pResult,
	_In_z_	const char* pFMT,
    _In_	const w_args&... pArgs);

/**
 * Validate W_RESULT and write in to the default log file
 * @param pResult result
 * @param pLogType type of log
 * @param pFMT fmt
*/
void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
    _In_z_	const char* pFMT);

/**
 * Validate W_RESULT and write in to the default log file
 * @param pResult result
 * @param pLogType type of log
 * @param pFMT fmt
 * @param pArgs args
*/
template<typename... w_args>
void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_z_	const char* pFMT,
    _In_	const w_args&... pArgs);

/**
 * Validate W_RESULT and write in to the default log file
 * @param pResult result
 * @param pLogType type of log
 * @param pTerminateProgram trminate program
 * @param pFMT fmt
*/
void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_	bool pTerminateProgram,
    _In_z_	const char* pFMT);

/**
 * Validate W_RESULT and write in to the default log file
 * @param pResult result
 * @param pLogType type of log
 * @param pTerminateProgram trminate program
 * @param pFMT fmt
*/
template<typename... w_args>
void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_	bool pTerminateProgram,
	_In_z_	const char* pFMT,
    _In_	const w_args&... pArgs);

#pragma endregion

