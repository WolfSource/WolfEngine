/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : w_logger.h
    Description		 : a logger class which is shared over all dlls
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
#include <map>
#include "w_inputs_manager.h"

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
#ifdef __UWP
            WSYS_EXP bool initialize(_In_z_ const std::wstring pAppName);
#else
            WSYS_EXP bool initialize(_In_z_ const std::wstring pAppName, _In_z_ const std::wstring pLogPath);            
#endif
            //Print buffered messages in to the screen, this function will be store the messages into the buffer
            WSYS_EXP void print_buffer(_In_z_ const std::wstring pMsg);
            //Clear screen and all buffered messages
            WSYS_EXP void clear_buffer();
            //Flush the output stream
            WSYS_EXP void flush();
            //Get the buffer of messages
            WSYS_EXP std::vector<std::wstring> get_buffer();
            //Write an output message
            WSYS_EXP void write(_In_z_ std::string pMsg, _In_z_ const std::string pState = "Info");
            //Write an output message
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

#pragma region Getters
            WSYS_EXP bool get_is_open() const;
#pragma endregion

        private:
            //Prevent copying
            w_logger(w_logger const&);
            w_logger& operator= (w_logger const&);

            w_logger_pimp* _pimp;
        };
    }

    extern WSYS_EXP system::w_logger logger;
    extern WSYS_EXP std::wstring content_path;
    extern WSYS_EXP system::w_inputs_manager inputs_manager;
    extern WSYS_EXP std::map<uint32_t, float> windows_frame_time_in_sec;
    extern WSYS_EXP void release_shared_data_over_all_instances();
}

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
    bool pTerminateAll = false)
{
    using namespace std;
    using namespace wolf;

    if (pHR == S_OK) return;

    auto _wstr_trace = std::wstring(pTraceClass.begin(), pTraceClass.end());
    wstring _errorMsg = L"Error on " + pMSG + L" with the following error info : " + L"Trace info " + _wstr_trace + L".";
    
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
    bool pExitNow = false)
{
    V(pHR == 0 ? S_OK : S_FALSE, pMSG, pTraceClass, pLogType, pExitNow);
}

/*
    Validate HResult and write in to the log file
    pHR						= Status
    pMSG						= the log message
    pLogType					= 0: SYSTEM, 1: USER, 2: WARNING, 3: ERROR
    pExitNow					= abort application
    pCheckForLastDirectXError	= check last error of GPU API
*/
inline void V(HRESULT pHR, std::string pMSG = "Undefined Error",
    std::string pTraceClass = "Undefined Trace", unsigned char pLogType = 0,
    bool pExitNow = false)
{
    auto _msg = std::wstring(pMSG.begin(), pMSG.end());
    V(pHR, _msg, pTraceClass, pLogType, pExitNow);
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
