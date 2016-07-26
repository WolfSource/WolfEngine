/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_logger.h
	Description		 : Shared log over dlls
	Comment          :
*/

#ifndef __W_LOGGER_H__
#define __W_LOGGER_H__

#include "w_object.h"
#include "w_system_dll.h"
#include <typeinfo>
#include <ctime>
#include <fstream>
#include <iostream>
#include <locale>
#include <codecvt>
#include <vector>

#ifdef WIN32 
#include "w_io.h"
#endif

#if defined(__MAYA)
#include <maya/MGlobal.h>
#endif

inline std::wstring to_UTF16(const std::string &pData)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(pData);
}

inline std::string to_UTF8(const std::wstring &pData)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(pData);
}

inline std::string get_date_time()
{
	char _szBuffer[80] = "DD-MM-YY HH:MM:SS";
	struct tm _time;
	time_t _rawtime;
	time(&_rawtime);

	errno_t _err = localtime_s(&_time, &_rawtime);
	if (_err)
	{
		std::string _msg = "Could not get the time from localtime_s";
#if defined(MAYA)
		MGlobal::displayError(_msg.c_str());
#else
		_msg += "\r\n";
		OutputDebugStringA(_msg.c_str());
#endif
	}
	else
	{
		strftime(_szBuffer, sizeof(_szBuffer), "%d-%b-%Y %X%p", &_time);
	}

	return std::string(_szBuffer);
}

inline std::wstring get_date_time_w()
{
	std::string _msg = get_date_time();
	auto _wmsg = std::wstring(_msg.begin(), _msg.end());
	_msg.clear();
	return _wmsg;
}

namespace wolf
{
	namespace system
	{ 
		enum LogType { _DEFAULT, _USER, _WARNING, _ERROR };
		class w_logger
		{
		public:
			//Initialize the logger and create a log file inside a Log folder into output directory
			SYS_EXP bool initialize(std::wstring pAppName);
			//Print message in to the screen, this fuction will be store the messages into the buffer
			SYS_EXP void printf(std::wstring pMsg);
			//Clear screen and all messages
			SYS_EXP void clearf();
			//Print all messages in to the screen
			SYS_EXP std::vector<std::wstring> flushf() const;
			//Write and output message
			SYS_EXP void write(std::wstring pMsg, const std::wstring pState = L"Info");
			//Write and output message
			SYS_EXP void write(std::string pMsg, const std::string pState = "Info");
			//Write an output user message 
			SYS_EXP void user(std::wstring pMsg);
			//Write an output user message 
			SYS_EXP void user(std::string pMsg);
			//Write an output warning message 
			SYS_EXP void warning(std::wstring pMsg);
			//Write an output warning message 
			SYS_EXP void warning(std::string pMsg);
			//Write an output error message 
			SYS_EXP void error(std::wstring pMsg);
			//Write an output error message 
			SYS_EXP void error(std::string pMsg);
			//Release all resources and close the log file
			SYS_EXP ULONG release();

		private:
			bool _isReleased;
			std::wofstream _logFile;
			std::vector<std::wstring> _msgs;
		};
	}
}

extern SYS_EXP wolf::system::w_logger logger;

/*
	Validate HResult and write in to the log file
	pHR							= Status
	pMSG						= the log message
	pLogType					= 0: SYSTEM, 1: USER, 2: WARNING, 3: ERROR
	pExitNow					= abort appilcation
	pCheckForLastDirectXError	= check last error of DirectX api
*/
inline void V(HRESULT pHR, std::wstring pMSG = L"Undefined message",
	std::wstring pTrace = L"Undefined trace", unsigned char pLogType = 0,
	bool pTerminateAll = false, bool pCheckForLastDirectXError = false)
{
	using namespace std;

	if (pHR == S_OK) return;

	wstring _errorMsg = L"";
	if (pCheckForLastDirectXError)
	{
		auto _err = GetLastError();
		auto  _errorMsg = L"Error on " + pMSG + L" with the following error info : ";
		LPVOID _lpMsgBuf;
		DWORD _bufLen = FormatMessage(
#if defined(__MAYA) || defined(WIN32)
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
#endif
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
#if defined(__MAYA) || defined(WIN32)
			LocalFree(_lpMsgBuf);
#endif
			_errorMsg += result;
		}

		_errorMsg += L"Trace info " + pTrace + L".";
	}
	else
	{
		_errorMsg = L"Error on " + pMSG + L" with the following error info : " + L"Trace info " + pTrace + L".";
	}

	switch (pLogType)
	{
	default:
	case 0://SYSTEM
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

	if (pTerminateAll) abort();
}

/*
	Validate HResult and write in to the log file
	pHR							= Status
	pMSG						= the log message
	pLogType					= 0: SYSTEM, 1: USER, 2: WARNING, 3: ERROR
	pExitNow					= abort appilcation
	pCheckForLastDirectXError	= check last error of DirectX api
*/
inline void V(HRESULT pHR, std::string pMSG = "Undefined Error",
	std::string pTrace = "Undefined Trace", unsigned char pLogType = 0,
	bool pExitNow = false, bool pCheckForLastDirectXError = false)
{
	auto _msg = std::wstring(pMSG.begin(), pMSG.end());
	auto _trace = std::wstring(pTrace.begin(), pTrace.end());
	V(pHR, _msg, _trace, pLogType, pExitNow, pCheckForLastDirectXError);
	_msg.clear();
	_trace.clear();
}

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


#ifdef __OPENCL__

/*
	Write in to the log file
	pHR							= Status
	pMSG						= the log message
	pLogType					= 0: SYSTEM, 1: USER, 2: WARNING, 3: ERROR
	pExitNow					= abort appilcation
	pCheckForLastDirectXError	= check last error of DirectX api
*/
inline void OnFailedCLW(int pOPENCL_STATUS, std::wstring pMSG = L"Undefined OpenCL Error",
	std::wstring pTrace = L"Undefined Trace", byte pLogType = 0,
	bool pExitNow = false)
{
	OnFailed(pOPENCL_STATUS != 0, pMSG, pTrace, pLogType, pExitNow, false);
}

/*
	Write in to the log file
	pHR							= Status
	pMSG						= the log message
	pLogType					= 0: SYSTEM, 1: USER, 2: WARNING, 3: ERROR
	pExitNow					= abort appilcation
	pCheckForLastDirectXError	= check last error of DirectX api
*/
inline void OnFailedCLW(int pOPENCL_STATUS, std::string pMSG = "Undefined OpenCL Error",
	std::string pTrace = "Undefined Trace", byte pLogType = 0,
	bool pExitNow = false)
{
	auto _msg = std::wstring(pMSG.begin(), pMSG.end());
	auto _trace = std::wstring(pTrace.begin(), pTrace.end());

	OnFailed(pOPENCL_STATUS != 0, _msg, _trace, pLogType, pExitNow, false);

	_msg.clear();
	_trace.clear();
}

#endif

#endif