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
     * @param pMemPool The pool to allocate out of
     * @param pConfig is the configuration of log
     * @return log ID, -1 means function failed
    */
    W_SYSTEM_EXPORT
        int  w_log_init(
            _Inout_ w_mem_pool pMemPool,
            _In_ const w_log_config* pConfig);

    /**
     * Write a message in to the default log file
     * @param pLogType type of the log
     * @param pFMT fmt
    */
    W_SYSTEM_EXPORT
        void LOG(
            _In_     w_log_type pLogType,
            _In_z_   const char* pFMT);

    /**
     * Write a message in to the default log file
     * @param pLogType type of the log
     * @param pLogID the id of the log
     * @param pFMT fmt
    */
    W_SYSTEM_EXPORT
        void LOG_EX(
            _In_     w_log_type pLogType,
            _In_     int pLogID,
            _In_z_   const char* pFMT);

    /**
     * Write a message in to the default log file
     * @param pLogType type of the log
     * @param pFMT fmt
    */
    W_SYSTEM_EXPORT
        void LOG_P(
            _In_     w_log_type pLogType,
            _In_z_   const char* pFMT,
            _In_     ...);

    /**
     * Write a message in to the specific log file
     * @param pLogType type of the log
     * @param pLogID the id of the log
     * @param pFMT fmt
    */
    W_SYSTEM_EXPORT
        void LOG_P_EX(
            _In_     w_log_type pLogType,
            _In_     int pLogID,
            _In_z_   const char* pFMT,
            _In_     ...);

    /**
     * Validate W_RESULT and write a message in to the default log file if the W_RESULT is not equal to W_SUCCESS
     * @param pResult result
     * @param pLogType type of the log
     * @param pFMT fmt
    */
    W_SYSTEM_EXPORT
        void V(
            _In_	    W_RESULT pResult,
            _In_        w_log_type pLogType,
            _In_z_	    const char* pFMT);

    /**
     * Validate W_RESULT and write a message in to the default log file if the W_RESULT is not equal to W_SUCCESS
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
     * Validate W_RESULT and write a message in to the default log file if the W_RESULT is not equal to W_SUCCESS
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
     * Validate W_RESULT and write a message in to the specific log file if the W_RESULT is not equal to W_SUCCESS
     * @param pResult result
     * @param pLogID the id of the log
     * @param pLogType type of the log
     * @param pTerminateProgram terminate the program
     * @param pFMT fmt
    */
    W_SYSTEM_EXPORT
        void VALIDATE_EX(
            _In_        W_RESULT pResult,
            _In_        int pLogID,
            _In_        w_log_type pLogType,
            _In_        bool pTerminateProgram,
            _In_z_      const char* pFMT,
            _In_        ...);

    /**
     * flush the default log file
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT  w_log_flush(void);

    /**
     * flush the specific log file
     * @param pLogID the id of log
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT  w_log_flush_ex(_In_ int pLogID);

    /**
     * this function will terminate all logs
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT  w_log_fini();

    /**
     * terminate the specific log file
     * @param pLogID the id of log
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT  w_log_fini_ex(_In_ int pLogID);

#define W_ASSERT(c, fmt)            { if (!c) { LOG_P(W_LOG_ERROR, fmt); assert(c);} }
#define W_ASSERT_P(c, fmt, ...)     { if (!c) { LOG_P(W_LOG_ERROR, (fmt), __VA_ARGS__); assert(c);} }

#ifdef __cplusplus
}
#endif
