/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_logger.h
	Description		 : Shared log over dlls
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_LOGGER_H__
#define __W_LOGGER_H__

#include "w_system_export.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <locale>
#include "w_io.h"
#include <vector>

#if defined(__MAYA)
#include <maya/MGlobal.h>
#endif

inline std::string get_date_time()
{
	char _buffer[256] = "DD-MM-YY HH:MM:SS";
	struct tm _time;
	time_t _rawtime;
	time(&_rawtime);

#if defined(__WIN32) || defined(__UWP)
	localtime_s(&_time, &_rawtime);
#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
	_time = *localtime(&_rawtime);
#endif

	strftime(_buffer, sizeof(_buffer), "%d-%b-%Y %X%p", &_time);

	return std::string(_buffer);
}

inline std::wstring get_date_timeW()
{
	std::string _msg = get_date_time();
	auto _wmsg = std::wstring(_msg.begin(), _msg.end());
	_msg.clear();
	return _wmsg;
}

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace system
	{ 
		enum LogType { _DEFAULT, _USER, _WARNING, _ERROR };
        
        //forward decalaration
        class w_logger_pimp;
		class w_logger
		{
		public:
            WSYS_EXP w_logger();
            WSYS_EXP virtual ~w_logger();
            
			//Initialize the logger and create a log file inside a Log folder into output directory
#if defined(__WIN32) 
			WSYS_EXP bool initialize(_In_z_ const std::wstring pAppName, _In_z_ const std::wstring pLogPath);
#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)
			WSYS_EXP bool initialize(_In_z_ const std::string pAppName, _In_z_ const std::string pLogPath);
#elif defined(__UWP)
			WSYS_EXP bool initialize(_In_z_ const std::wstring pAppName);
#endif
			//Print buffered messages in to the screen, this function will be store the messages into the buffer
			WSYS_EXP void print_buffer(_In_z_ const std::wstring pMsg);
			//Clear screen and all buffered messages
			WSYS_EXP void clear_buffer();
			//flush the output stream
			WSYS_EXP void flush();
			//get the buffer of messages
			WSYS_EXP std::vector<std::wstring> get_buffer();
			//Write and output message
			WSYS_EXP void write(_In_z_ std::string pMsg, _In_z_ const std::string pState = "Info");
			//Write and output message
			WSYS_EXP void write(_In_z_ std::wstring pMsg, _In_z_ const std::wstring pState = L"Info");
			//Write an output user message 
			WSYS_EXP void user(_In_z_ const std::wstring pMsg);
			//Write an output user message 
			WSYS_EXP void user(_In_z_ const std::string pMsg);
			//Write an output warning message 
			WSYS_EXP void warning(_In_z_ const std::wstring pMsg);
			//Write an output warning message 
			WSYS_EXP void warning(_In_z_ const std::string pMsg);
			//Write an output error message 
			WSYS_EXP void error(_In_z_ const std::wstring pMsg);
			//Write an output error message 
			WSYS_EXP void error(_In_z_ const std::string pMsg);
			//Release all resources and close the log file
			WSYS_EXP ULONG release();

		private:
            //Prevent copying
            w_logger(w_logger const&);
            w_logger& operator= (w_logger const&);
            
            w_logger_pimp* _pimp;
		};
	}
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

extern WSYS_EXP wolf::system::w_logger logger;

/*
	Validate HResult and write in to the log file
	pHR							= Status
	pMSG						= the log message
	pLogType					= 0: SYSTEM, 1: USER, 2: WARNING, 3: ERROR
	pTerminateAll				= abort appilcation. Strongly not recommended, please make sure release all your resources before aborting Wolf.Engine
	pCheckForLastDirectXError	= check last error of GPU API
*/
inline void V(HRESULT pHR, std::wstring pMSG = L"Undefined message",
	std::string pTraceClass = "Undefined trace", unsigned char pLogType = 0,
	bool pTerminateAll = false, bool pCheckForLastGPUError = false)
{
	using namespace std;

	if (pHR == S_OK) return;

	auto _wstr_trace = std::wstring(pTraceClass.begin(), pTraceClass.end());
	wstring _errorMsg = L"";
	if (pCheckForLastGPUError)
	{

#if defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

		_errorMsg = L"Error on " + pMSG + L" with the following error info : ";

#elif defined(__WIN32) || defined(__UWP) || defined(__MAYA)

		auto _err = GetLastError();
		_errorMsg = L"Error on " + pMSG + L" with the following error info : ";
		LPVOID _lpMsgBuf;
		DWORD _bufLen = FormatMessage(

#if defined(__WIN32) || defined(__MAYA)
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
#endif // defined(__MAYA) || defined(__WIN32)

			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			_err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&_lpMsgBuf,
			0, NULL);
		if (_bufLen)
		{
			auto _lpMsgStr = (LPCSTR)_lpMsgBuf;
			wstring result(_lpMsgStr, _lpMsgStr + _bufLen);

#if defined(__WIN32) || defined(__MAYA)
			LocalFree(_lpMsgBuf);
#endif //defined(__MAYA) || defined(__WIN32)

			_errorMsg += result;
		}

		_errorMsg += L"Trace info " + _wstr_trace + L".";

#endif // __ANDROID || __linux || __APPLE__

	}
	else
	{
		_errorMsg = L"Error on " + pMSG + L" with the following error info : " + L"Trace info " + _wstr_trace + L".";
	}

	switch (pLogType)
	{
	default:
	case 0: //SYSTEM
		logger.write(_errorMsg);
		break;
	case 1: //USER
		logger.user(_errorMsg);
		break;
	case 2: //WARNING:
		logger.warning(_errorMsg);
		break;
	case 3: //ERROR:
		logger.error(_errorMsg);
		break;
	}

	if (pTerminateAll)
	{
		logger.release();
		std::exit(EXIT_FAILURE);
	}
}

inline void V(int pHR, std::wstring pMSG = L"Undefined Error",
	std::string pTraceClass = "Undefined Trace", unsigned char pLogType = 0,
	bool pExitNow = false, bool pCheckForLastGPUError = false)
{
        V(pHR == 0 ? S_OK : S_FALSE, pMSG, pTraceClass, pLogType, pExitNow, pCheckForLastGPUError);
}

/*
	Validate HResult and write in to the log file
	pHR						= Status
	pMSG						= the log message
	pLogType					= 0: SYSTEM, 1: USER, 2: WARNING, 3: ERROR
	pExitNow					= abort appilcation
	pCheckForLastDirectXError	= check last error of GPU API
*/
inline void V(HRESULT pHR, std::string pMSG = "Undefined Error",
	std::string pTraceClass = "Undefined Trace", unsigned char pLogType = 0,
	bool pExitNow = false, bool pCheckForLastGPUError = false)
{
	auto _msg = std::wstring(pMSG.begin(), pMSG.end());
	V(pHR, _msg, pTraceClass, pLogType, pExitNow, pCheckForLastGPUError);
	_msg.clear();
}

#if defined(__WIN32) || defined(__UWP) || defined(__MAYA)
inline void DebugTrace(_In_z_ _Printf_format_string_ const char* format, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start(args, format);

	char buff[1024] = { 0 };
	vsprintf_s(buff, format, args);
	OutputDebugStringA(buff);
#else
	UNREFERENCED_PARAMETER(format);
#endif
}
#endif //defined(__WIN32) || defined(__UWP) || defined(__MAYA)

#endif //__W_LOGGER_H__
