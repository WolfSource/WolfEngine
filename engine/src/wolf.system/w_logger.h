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

#include "wolf_version.h"
#include "w_system_export.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <locale>
#include "w_io.h"
#include <vector>
#include <map>
#include "w_inputs_manager.h"
#include <iostream>
#include <mutex>
#include <type_traits>
#include <exception>

#ifdef __ANDROID

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Wolf_Engine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Wolf_Engine", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Wolf_Engine", __VA_ARGS__))

#endif

#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#define SPDLOG_WCHAR_FILENAMES
#include "spdlog/spdlog.h"

#ifndef MinSizeRel
#ifdef _MSC_VER
	#include "spdlog/sinks/msvc_sink.h"
#endif
#endif

namespace wolf
{
    namespace system
    {
        class w_logger
        {
        public:
			w_logger() : _is_released(false), _opened(false) {}
			virtual ~w_logger() { release(); }

            //Initialize the logger and create a log file inside a Log folder into output directory
#ifdef __UWP
			WSYS_EXP bool initialize(_In_z_ const std::wstring pAppName)
			{
				//if directory of log is not existed
				auto _local_folder = Windows::Storage::ApplicationData::Current->LocalFolder;
				auto _log_file_path = _local_folder->Path + ref new Platform::String(L"\\log\\") + ref new Platform::String((io::get_unique_nameW() + L".wLog").c_str());

				auto _function = [this, _log_file_path, pAppName]() ->bool
				{
					this->_log_file.open(_log_file_path->Data());

					std::wstring _created_time = L"\t\"Time\"             : \"" + get_date_timeW() + L"\",\r\n";
					std::wstring _app_name = L"\t\"Application Name\" : \"" + pAppName + L"\",\r\n";

					std::wstring _version = L"\t\"Version\"          : \"" + std::to_wstring(WOLF_MAJOR_VERSION) + L":" + std::to_wstring(WOLF_MINOR_VERSION) +
						L":" + std::to_wstring(WOLF_PATCH_VERSION) + L":" + std::to_wstring(WOLF_DEBUG_VERSION) + L"\",\r\n";

					this->_log_file << L"{\r\n";
					this->_log_file << L"\t\"Project\"          : \"Wolf Engine(http://WolfSource.io). Copyright(c) Pooya Eimandar(http://PooyaEimandar.com). All rights reserved.\",\r\n";
					this->_log_file << L"\t\"Info\"             : \"contact@WolfSource.io\r\n";
					this->_log_file << _version.c_str();
					this->_log_file << _created_time.c_str();
					this->_log_file << _app_name.c_str();
					this->_log_file << L"\t\"Logs\":{\r\n";

					//store all buffered logs
					auto _buffer = get_buffer();
					for (size_t i = 0; i < _buffer.size(); ++i)
					{
						this->_log_file << _buffer[i].c_str();
					}
					clear_buffer();

					this->_log_file.flush();

					return this->_log_file.is_open();
				};

				wolf::system::io::get_directory_exists_async(_local_folder, "log")
					.then([=](_In_ BOOL pExists)
				{
					if (!pExists)
					{
						wolf::system::io::create_directory_async(_local_folder, "log")
							.then([=](Windows::Storage::StorageFolder^ pFolder)
						{
							_function();
						});
					}
					else
					{
						_function();
					}
				});

				this->_is_released = false;


				return true;
			}
#else
			bool initialize(
				_In_z_ const std::wstring& pAppName,
				_In_z_ const std::wstring& pLogPath,
				_In_ const bool& pFlushJustOnError = false)
			{
#ifdef __WIN32
				auto _log_directory = pLogPath + L"\\Log\\";
				auto _log_directory_cstr = _log_directory.c_str();
				//if directory of log is not existed
				if (io::get_is_directoryW(_log_directory_cstr) != W_PASSED)
				{
					//Create the directory of log inside the root directory
					io::create_directoryW(_log_directory_cstr);
				}
				auto _log_file_path = _log_directory + io::get_unique_nameW() + L".wLog";
#else
				auto _log_directory = wolf::system::convert::wstring_to_string(pLogPath) + "/Log/";
				auto _log_directory_cstr = _log_directory.c_str();
				//if directory of log is not existed
				if (io::get_is_directory(_log_directory_cstr) != W_PASSED)
				{
					//Create the directory of log inside the root directory
					io::create_directory(_log_directory_cstr);
				}
				auto _log_file_path = _log_directory + io::get_unique_name() + ".wLog";
#endif
				std::vector<spdlog::sink_ptr> sinks;

				sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>(_log_file_path));
#ifdef _MSC_VER
				sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#else
				sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
#endif

				this->_log_file = std::make_shared<spdlog::logger>(
					wolf::system::convert::wstring_to_string(pAppName), begin(sinks), end(sinks));
				if (!this->_log_file)
				{
					return false;
				}

				this->_log_file->info(
					L"{\n\t\"Project\": \"Wolf Engine(http://WolfSource.io). "\
					L"Copyright(c) Pooya Eimandar(http://PooyaEimandar.com). All rights reserved.\". "\
					L"\"Contact\"\ : \"Contact@WolfSource.io\n");
				this->_log_file->info(L"\t\"Version\": {}.{}.{}.{}", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);

				if (pFlushJustOnError)
				{
					this->_log_file->flush_on(spdlog::level::level_enum::err);
				}
				this->_opened = true;
				return true;
			}

#endif //__UWP

            //Flush the output stream
			void flush()
			{
				this->_log_file->flush();
			}

            //Write an output message
			void write(_In_z_ const char* fmt)
			{
				//this->_log_file->info(fmt);

//#ifndef MinSizeRel
//
//#if defined(__WIN32) || defined(__UWP)
//				OutputDebugStringA(fmt);
//#elif defined(__linux) || defined(__APPLE__)
//				std::cout << fmt;
//#elif defined(__ANDROID)
//				LOGI(fmt);
//#endif
//
//#endif
			}

			//Write an output message
			void write(_In_z_ const wchar_t* fmt)
			{
				//this->_log_file->info(fmt);

//#if defined(__WIN32) || defined(__UWP)
//				OutputDebugStringW(fmt);
//#elif defined(__linux) || defined(__APPLE__)
//				std::wcout << fmt;
//#elif defined(__ANDROID)
//				LOGI(fmt);
//#endif
			}

			//Write an output message
			template<typename... w_args>
			void write(_In_z_ const char* fmt, _In_ const w_args &... args)
			{
				//this->_log_file->info(fmt, args...);

//#ifndef MinSizeRel
//				std::string _str = format(fmt, args);
//#if defined(__WIN32) || defined(__UWP)
//				OutputDebugStringA(_str.c_str());
//#elif defined(__linux) || defined(__APPLE__)
//				std::cout << _str.c_str();
//#elif defined(__ANDROID)
//				LOGI(_str.c_str());
//#endif

//#endif//MinSizeRel 

			}

			//Write an output message
			template<typename... w_args>
			void write(_In_z_ const wchar_t* fmt, _In_ const w_args &... args)
			{
				//this->_log_file->info(fmt, args...);

//#if defined(__WIN32) || defined(__UWP)
//				OutputDebugStringW(fmt);
//#elif defined(__linux) || defined(__APPLE__)
//				std::wcout << fmt;
//#elif defined(__ANDROID)
//				LOGI(fmt);
//#endif
			}

			//Write an output message
			void warning(_In_z_ const char* fmt)
			{
//				this->_log_file->warn(fmt);
//
//#if defined(__WIN32) || defined(__UWP)
//				OutputDebugStringA(fmt);
//#elif defined(__linux) || defined(__APPLE__)
//				std::cout << fmt;
//#elif defined(__ANDROID)
//				LOGI(fmt);
//#endif
			}

			//Write an output message
			void warning(_In_z_ const wchar_t* fmt)
			{
//				this->_log_file->warn(fmt);
//
//#if defined(__WIN32) || defined(__UWP)
//				OutputDebugStringW(fmt);
//#elif defined(__linux) || defined(__APPLE__)
//				std::wcout << fmt;
//#elif defined(__ANDROID)
//				LOGI(fmt);
//#endif
			}

            //Write an output warning message 
			void warning(_In_z_ const std::string pMsg)
			{

			}

            //Write an output error message 
			void error(_In_z_ const std::wstring pMsg)
			{

			}

            //Write an output error message 
			void error(_In_z_ const std::string pMsg)
			{

			}
            
			//Release all resources and close the log file
			ULONG release()
			{
				if (this->_is_released) return 1;

				this->_is_released = true;

				write("wolf shutting down");
				this->_log_file->flush();

				return 0;
			}

#pragma region Getters
			bool get_is_open() const { return this->_opened; }
			bool get_is_released() const { return _is_released; }
#pragma endregion

        private:
            //Prevent copying
            w_logger(w_logger const&);
            w_logger& operator= (w_logger const&);

			bool							_is_released;
			bool							_opened;
			std::mutex						_mutex;

			std::shared_ptr<spdlog::logger> _log_file;
        };
    }

    extern WSYS_EXP system::w_logger logger;
    extern WSYS_EXP std::wstring content_path;
    extern WSYS_EXP system::w_inputs_manager inputs_manager;
    extern WSYS_EXP std::map<uint32_t, float> windows_frame_time_in_sec;
	extern WSYS_EXP void release_heap_data();
}

/*
    Validate W_RESULT and write in to the log file
    pHR							= Status
    pMSG						= the log message
    pLogType					= 0: SYSTEM, 1: USER, 2: WARNING, 3: ERROR
    pTerminateAll				= abort appilcation. Strongly not recommended, please make sure release all your resources before aborting Wolf.Engine
    pCheckForLastDirectXError	= check last error of GPU API
*/
inline void V(W_RESULT pHR, std::wstring pMSG = L"Undefined message",
    std::string pTraceClass = "Undefined trace", unsigned char pLogType = 0,
    bool pTerminateAll = false)
{
    using namespace std;
    using namespace wolf;

    if (pHR == W_PASSED) return;

   // auto _wstr_trace = std::wstring(pTraceClass.begin(), pTraceClass.end());
   // wstring _errorMsg = pMSG + L" with the following error info : " + L"Trace info " + _wstr_trace + L".";
    
    //switch (pLogType)
    //{
    //default:
    //case 0: //SYSTEM
    //    logger.write(_errorMsg);
    //    break;
    //case 1: //USER
    //    logger.user(_errorMsg);
    //    break;
    //case 2: //WARNING:
    //    logger.warning(_errorMsg);
    //    break;
    //case 3: //ERROR:
    //    logger.error(_errorMsg);
    //    break;
    //}

    if (pTerminateAll)
    {
		release_heap_data();
        std::exit(EXIT_FAILURE);
    }
}

inline void V(int pHR, std::wstring pMSG = L"Undefined Error",
    std::string pTraceClass = "Undefined Trace", unsigned char pLogType = 0,
    bool pExitNow = false)
{
    V(pHR == 0 ? W_PASSED : W_FAILED, pMSG, pTraceClass, pLogType, pExitNow);
}

/*
    Validate W_RESULT and write in to the log file
    pHR						= Status
    pMSG						= the log message
    pLogType					= 0: SYSTEM, 1: USER, 2: WARNING, 3: ERROR
    pExitNow					= abort application
    pCheckForLastDirectXError	= check last error of GPU API
*/
inline void V(W_RESULT pHR, std::string pMSG = "Undefined Error",
    std::string pTraceClass = "Undefined Trace", unsigned char pLogType = 0,
    bool pExitNow = false)
{
    auto _msg = std::wstring(pMSG.begin(), pMSG.end());
    V(pHR, _msg, pTraceClass, pLogType, pExitNow);
    _msg.clear();
}

#endif //__W_LOGGER_H__
