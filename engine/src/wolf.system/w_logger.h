/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
    Name			 : w_logger.h
    Description		 : a logger class which is shared over all dlls
    Comment          :
*/

#pragma once

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

#include <errno.h>
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
        struct w_logger_config
        {
            std::wstring app_name;
            std::wstring log_path;
			//false means flush will be called on info, true means flush level is warn
            bool flush_level = false;
            bool log_to_std_out = true;
        };
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
					this->_log_file << L"\t\"Project\"          : \"Wolf Engine(https://WolfEngine.app). Copyright(c) Pooya Eimandar(https://PooyaEimandar.github.io). All rights reserved.\",\r\n";
					this->_log_file << L"\t\"Info\"             : \"Pooya@WolfEngine.App\r\n";
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
			bool initialize(_In_ const w_logger_config& pConfig)
			{
#ifdef __WIN32
				auto _log_directory = pConfig.log_path + L"\\Log\\";
				auto _log_directory_cstr = _log_directory.c_str();
				//if directory of log is not existed
				if (io::get_is_directoryW(_log_directory_cstr) != W_PASSED)
				{
					//Create the directory of log inside the root directory
					io::create_directoryW(_log_directory_cstr);
				}
				auto _log_file_path = _log_directory + io::get_date_time_strW() + L".wLog";
#else
				auto _log_directory = wolf::system::convert::wstring_to_string(pConfig.log_path) + "/Log/";
				auto _log_directory_cstr = _log_directory.c_str();
				//if directory of log is not existed
				if (io::get_is_directory(_log_directory_cstr) != W_PASSED)
				{
					//Create the directory of log inside the root directory
					io::create_directory(_log_directory_cstr);
				}
				auto _log_file_path = _log_directory + io::get_date_time_str() + ".wLog";
#endif
				std::vector<spdlog::sink_ptr> sinks;

				sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>(_log_file_path));
#if defined(_MSC_VER) && !defined(MinSizeRel)
				sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif
				if (pConfig.log_to_std_out)
				{
					sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
				}

				this->_log_file = std::make_shared<spdlog::logger>(
					wolf::system::convert::wstring_to_string(pConfig.app_name), begin(sinks), end(sinks));
				if (!this->_log_file)
				{
					return false;
				}

				this->_log_file->info(
					"Project: \"Wolf Engine(https://WolfEngine.App). "\
					"Copyright(c) Pooya Eimandar(https://PooyaEimandar.github.io). All rights reserved.\". "\
					"Contact: \"Pooya@WolfEngine.App\" "\
					"Version: {}.{}.{}.{}", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
				
				if (pConfig.flush_level)
				{
					this->_log_file->set_level(spdlog::level::level_enum::warn);
					this->_log_file->flush_on(spdlog::level::level_enum::warn);
				}
				else
				{
					this->_log_file->flush_on(spdlog::level::level_enum::info);
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

#pragma region write
            //Write an output message
			void write(_In_z_ const char* fmt)
			{
				this->_log_file->info(fmt);
			}

			//Write an output message
			void write(_In_z_ const wchar_t* fmt)
			{
				this->_log_file->info(fmt);
			}

			//Write an output message
			template<typename... w_args>
			void write(_In_z_ const char* fmt, _In_ const w_args&... args)
			{
				this->_log_file->info(fmt, args...);
			}

			//Write an output message
			template<typename... w_args>
			void write(_In_z_ const wchar_t* fmt, _In_ const w_args&... args)
			{
				this->_log_file->info(fmt, args...);
			}
#pragma endregion

#pragma region warning
			//Write a warning message
			void warning(_In_z_ const char* fmt)
			{
				this->_log_file->warn(fmt);
			}

			//Write a warning message
			void warning(_In_z_ const wchar_t* fmt)
			{
				this->_log_file->warn(fmt);
			}

			//Write a warning message
			template<typename... w_args>
			void warning(_In_z_ const char* fmt, _In_ const w_args&... args)
			{
				this->_log_file->warn(fmt, args...);
			}

			//Write a warning message
			template<typename... w_args>
			void warning(_In_z_ const wchar_t* fmt, _In_ const w_args&... args)
			{
				this->_log_file->warn(fmt, args...);
			}
#pragma endregion

#pragma region error
			//Write an error message
			void error(_In_z_ const char* fmt)
			{
				this->_log_file->error(fmt);
			}

			//Write an error message
			void error(_In_z_ const wchar_t* fmt)
			{
				this->_log_file->error(fmt);
			}

			//Write an error message
			template<typename... w_args>
			void error(_In_z_ const char* fmt, _In_ const w_args&... args)
			{
				this->_log_file->error(fmt, args...);
			}

			//Write an error message
			template<typename... w_args>
			void error(_In_z_ const wchar_t* fmt, _In_ const w_args&... args)
			{
				this->_log_file->error(fmt, args...);
			}
#pragma endregion
            
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

enum w_log_type { W_INFO = 0, W_WARNING, W_ERROR };

#pragma region V for wchar_t

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_z_	const wchar_t* fmt)
{
	using namespace std;
	using namespace wolf;

	if (pResult == W_PASSED) return;
	wolf::logger.write(fmt);
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_z_	const wchar_t* fmt,
	_In_	const w_args&... args)
{
	if (pResult == W_PASSED) return;
	wolf::logger.write(fmt, args...);
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_z_	const wchar_t* fmt)
{
	if (pResult == W_PASSED) return;

	switch (pLogType)
	{
	default:
	case w_log_type::W_INFO:
		wolf::logger.write(fmt);
		break;
	case w_log_type::W_WARNING:
		wolf::logger.warning(fmt);
		break;
	case w_log_type::W_ERROR:
		wolf::logger.error(fmt);
		break;
	}
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_z_	const wchar_t* fmt,
	_In_	const w_args&... args)
{
	if (pResult == W_PASSED) return;

	switch (pLogType)
	{
	default:
	case w_log_type::W_INFO:
		wolf::logger.write(fmt, args...);
		break;
	case w_log_type::W_WARNING:
		wolf::logger.warning(fmt, args...);
		break;
	case w_log_type::W_ERROR:
		wolf::logger.error(fmt, args...);
		break;
	}
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_	bool pTerminateProgram,
	_In_z_	const wchar_t* fmt)
{
	using namespace std;
	using namespace wolf;

	if (pResult == W_PASSED) return;

	switch (pLogType)
	{
	default:
	case w_log_type::W_INFO:
		logger.write(fmt);
		break;
	case w_log_type::W_WARNING:
		logger.warning(fmt);
		break;
	case w_log_type::W_ERROR:
		logger.error(fmt);
		break;
	}

	if (pTerminateProgram)
	{
		release_heap_data();
		std::exit(EXIT_FAILURE);
	}
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_	bool pTerminateProgram,
	_In_z_	const wchar_t* fmt,
	_In_	const w_args&... args)
{
	using namespace std;
	using namespace wolf;

	if (pResult == W_PASSED) return;

	switch (pLogType)
	{
	default:
	case w_log_type::W_INFO:
		logger.write(fmt, args...);
		break;
	case w_log_type::W_WARNING:
		logger.warning(fmt, args...);
		break;
	case w_log_type::W_ERROR:
		logger.error(fmt, args...);
		break;
	}

	if (pTerminateProgram)
	{
		release_heap_data();
		std::exit(EXIT_FAILURE);
	}
}

#pragma endregion

#pragma region V for char

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_z_	const char* fmt)
{
	using namespace std;
	using namespace wolf;

	if (pResult == W_PASSED) return;
	wolf::logger.write(fmt);
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_z_	const char* fmt,
	_In_	const w_args&... args)
{
	if (pResult == W_PASSED) return;
	wolf::logger.write(fmt, args...);
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_z_	const char* fmt)
{
	if (pResult == W_PASSED) return;

	switch (pLogType)
	{
	default:
	case w_log_type::W_INFO:
		wolf::logger.write(fmt);
		break;
	case w_log_type::W_WARNING:
		wolf::logger.warning(fmt);
		break;
	case w_log_type::W_ERROR:
		wolf::logger.error(fmt);
		break;
	}
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_z_	const char* fmt,
	_In_	const w_args&... args)
{
	if (pResult == W_PASSED) return;

	switch (pLogType)
	{
	default:
	case w_log_type::W_INFO:
		wolf::logger.write(fmt, args...);
		break;
	case w_log_type::W_WARNING:
		wolf::logger.warning(fmt, args...);
		break;
	case w_log_type::W_ERROR:
		wolf::logger.error(fmt, args...);
		break;
	}
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_	bool pTerminateProgram,
	_In_z_	const char* fmt)
{
	using namespace std;
	using namespace wolf;

	if (pResult == W_PASSED) return;

	switch (pLogType)
	{
	default:
	case w_log_type::W_INFO:
		logger.write(fmt);
		break;
	case w_log_type::W_WARNING:
		logger.warning(fmt);
		break;
	case w_log_type::W_ERROR:
		logger.error(fmt);
		break;
	}

	if (pTerminateProgram)
	{
		release_heap_data();
		std::exit(EXIT_FAILURE);
	}
}

//Validate W_RESULT and write in to the log file
template<typename... w_args>
inline void V(
	_In_	W_RESULT pResult,
	_In_	w_log_type pLogType,
	_In_	bool pTerminateProgram,
	_In_z_	const char* fmt,
	_In_	const w_args&... args)
{
	using namespace std;
	using namespace wolf;

	if (pResult == W_PASSED) return;

	switch (pLogType)
	{
	default:
	case w_log_type::W_INFO:
		logger.write(fmt, args...);
		break;
	case w_log_type::W_WARNING:
		logger.warning(fmt, args...);
		break;
	case w_log_type::W_ERROR:
		logger.error(fmt, args...);
		break;
	}

	if (pTerminateProgram)
	{
		release_heap_data();
		std::exit(EXIT_FAILURE);
	}
}

#pragma endregion
