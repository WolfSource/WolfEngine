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

///**
// * write to specific log file
// * @param pLogType type of log
//        W_INFO,
//        W_WARNING,
//        W_ERROR
// * @param pLogID log id
// * @param pFMT fmt
// * @return result
//*/
//W_RESULT  w_log(_In_ const w_log_type pLogType,
//                _In_ const int pLogID,
//                _In_z_ const wchar_t* pFMT);
//
///**
// * write to default log file
// * @param pLogType type of log
//        W_INFO,
//        W_WARNING,
//        W_ERROR
// * @param pFMT fmt
// * @param pArgs are the argumans of fmt
// * @return result
//*/
//template<typename... w_args>
//W_RESULT  w_log(_In_ const w_log_type pLogType, _In_z_ const char* pFMT, _In_ const w_args&... pArgs);
//
///**
// * write to default log file
// * @param pLogType type of log
//        W_INFO,
//        W_WARNING,
//        W_ERROR
// * @param pFMT fmt
// * @param pArgs are the argumans of fmt
// * @return result
//*/
//template<typename... w_args>
//W_RESULT  w_log(_In_ const w_log_type pLogType, _In_z_ const wchar_t* pFMT, _In_ const w_args&... pArgs);
//
///**
// * write to default log file
// * @param pLogType type of log
//        W_INFO,
//        W_WARNING,
//        W_ERROR
// * @param pLogID log id
// * @param pFMT fmt
// * @param pArgs are the argumans of fmt
// * @return result
//*/
//template<typename... w_args>
//W_RESULT  w_log(_In_ const w_log_type pLogType,
//                _In_ const int pLogID,
//                _In_z_ const char* pFMT,
//                _In_ const w_args&... pArgs);
//
///**
// * write to default log file
// * @param pLogType type of log
//        W_INFO,
//        W_WARNING,
//        W_ERROR
// * @param pLogID log id
// * @param pFMT fmt
// * @param pArgs are the argumans of fmt
// * @return result
//*/
//template<typename... w_args>
//W_RESULT  w_log(_In_ const w_log_type pLogType,
//                _In_ const int pLogID,
//                _In_z_ const wchar_t* pFMT,
//                _In_ const w_args&... pArgs);
//
//#pragma region VALIDATE
///**
// * Validate W_RESULT and write in to the log file
// * @param pResult result
// * @param pFMT fmt
//*/
//template<typename... w_args>
//void V(_In_ const W_RESULT pResult, _In_z_ const char* pFMT);
//
///**
// * Validate W_RESULT and write in to the log file
// * @param pResult result
// * @param pFMT fmt
//*/
//template<typename... w_args>
//void V(_In_	const W_RESULT pResult, _In_z_ const wchar_t* pFMT);

////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_z_	const wchar_t* fmt,
//	_In_	const w_args&... args)
//{
//	if (pResult == W_PASSED) return;
//	wolf::logger.write(fmt, args...);
//}
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_	w_log_type pLogType,
//	_In_z_	const wchar_t* fmt)
//{
//	if (pResult == W_PASSED) return;
//
//	switch (pLogType)
//	{
//	default:
//	case w_log_type::W_INFO:
//		wolf::logger.write(fmt);
//		break;
//	case w_log_type::W_WARNING:
//		wolf::logger.warning(fmt);
//		break;
//	case w_log_type::W_ERROR:
//		wolf::logger.error(fmt);
//		break;
//	}
//}
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_	w_log_type pLogType,
//	_In_z_	const wchar_t* fmt,
//	_In_	const w_args&... args)
//{
//	if (pResult == W_PASSED) return;
//
//	switch (pLogType)
//	{
//	default:
//	case w_log_type::W_INFO:
//		wolf::logger.write(fmt, args...);
//		break;
//	case w_log_type::W_WARNING:
//		wolf::logger.warning(fmt, args...);
//		break;
//	case w_log_type::W_ERROR:
//		wolf::logger.error(fmt, args...);
//		break;
//	}
//}
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_	w_log_type pLogType,
//	_In_	bool pTerminateProgram,
//	_In_z_	const wchar_t* fmt)
//{
//	using namespace std;
//	using namespace wolf;
//
//	if (pResult == W_PASSED) return;
//
//	switch (pLogType)
//	{
//	default:
//	case w_log_type::W_INFO:
//		logger.write(fmt);
//		break;
//	case w_log_type::W_WARNING:
//		logger.warning(fmt);
//		break;
//	case w_log_type::W_ERROR:
//		logger.error(fmt);
//		break;
//	}
//
//	if (pTerminateProgram)
//	{
//		release_heap_data();
//		std::exit(EXIT_FAILURE);
//	}
//}
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_	w_log_type pLogType,
//	_In_	bool pTerminateProgram,
//	_In_z_	const wchar_t* fmt,
//	_In_	const w_args&... args)
//{
//	using namespace std;
//	using namespace wolf;
//
//	if (pResult == W_PASSED) return;
//
//	switch (pLogType)
//	{
//	default:
//	case w_log_type::W_INFO:
//		logger.write(fmt, args...);
//		break;
//	case w_log_type::W_WARNING:
//		logger.warning(fmt, args...);
//		break;
//	case w_log_type::W_ERROR:
//		logger.error(fmt, args...);
//		break;
//	}
//
//	if (pTerminateProgram)
//	{
//		release_heap_data();
//		std::exit(EXIT_FAILURE);
//	}
//}
//
//#pragma endregion
//
//#pragma region V for char
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_z_	const char* fmt)
//{
//	using namespace std;
//	using namespace wolf;
//
//	if (pResult == W_PASSED) return;
//	wolf::logger.write(fmt);
//}
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_z_	const char* fmt,
//	_In_	const w_args&... args)
//{
//	if (pResult == W_PASSED) return;
//	wolf::logger.write(fmt, args...);
//}
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_	w_log_type pLogType,
//	_In_z_	const char* fmt)
//{
//	if (pResult == W_PASSED) return;
//
//	switch (pLogType)
//	{
//	default:
//	case w_log_type::W_INFO:
//		wolf::logger.write(fmt);
//		break;
//	case w_log_type::W_WARNING:
//		wolf::logger.warning(fmt);
//		break;
//	case w_log_type::W_ERROR:
//		wolf::logger.error(fmt);
//		break;
//	}
//}
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_	w_log_type pLogType,
//	_In_z_	const char* fmt,
//	_In_	const w_args&... args)
//{
//	if (pResult == W_PASSED) return;
//
//	switch (pLogType)
//	{
//	default:
//	case w_log_type::W_INFO:
//		wolf::logger.write(fmt, args...);
//		break;
//	case w_log_type::W_WARNING:
//		wolf::logger.warning(fmt, args...);
//		break;
//	case w_log_type::W_ERROR:
//		wolf::logger.error(fmt, args...);
//		break;
//	}
//}
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_	w_log_type pLogType,
//	_In_	bool pTerminateProgram,
//	_In_z_	const char* fmt)
//{
//	using namespace std;
//	using namespace wolf;
//
//	if (pResult == W_PASSED) return;
//
//	switch (pLogType)
//	{
//	default:
//	case w_log_type::W_INFO:
//		logger.write(fmt);
//		break;
//	case w_log_type::W_WARNING:
//		logger.warning(fmt);
//		break;
//	case w_log_type::W_ERROR:
//		logger.error(fmt);
//		break;
//	}
//
//	if (pTerminateProgram)
//	{
//		release_heap_data();
//		std::exit(EXIT_FAILURE);
//	}
//}
//
////Validate W_RESULT and write in to the log file
//template<typename... w_args>
//inline void V(
//	_In_	W_RESULT pResult,
//	_In_	w_log_type pLogType,
//	_In_	bool pTerminateProgram,
//	_In_z_	const char* fmt,
//	_In_	const w_args&... args)
//{
//	using namespace std;
//	using namespace wolf;
//
//	if (pResult == W_PASSED) return;
//
//	switch (pLogType)
//	{
//	default:
//	case w_log_type::W_INFO:
//		logger.write(fmt, args...);
//		break;
//	case w_log_type::W_WARNING:
//		logger.warning(fmt, args...);
//		break;
//	case w_log_type::W_ERROR:
//		logger.error(fmt, args...);
//		break;
//	}
//
//	if (pTerminateProgram)
//	{
//		release_heap_data();
//		std::exit(EXIT_FAILURE);
//	}
//}

#pragma endregion

