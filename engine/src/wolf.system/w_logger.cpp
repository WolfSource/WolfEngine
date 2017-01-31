#include "w_system_pch.h"
#include "w_logger.h"

#if defined(__WIN32)

#include <codecvt>

#elif defined(__UNIVERSAL)

#include <codecvt>
#include <ppltasks.h>

#elif defined(__linux)

#include <iostream>

#elif defined(__ANDROID)

#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Wolf.Engine", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Wolf.Engine", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Wolf.Engine", __VA_ARGS__))

#endif

using namespace wolf::system;

#if defined(__WIN32) || defined(__ANDROID) || defined(__linux)

#ifdef __WIN32

bool w_logger::initialize(_In_z_ const std::wstring pAppName, _In_z_ const std::wstring pLogPath)
{
	auto _log_directory = pLogPath + L"\\Log\\";
	auto _log_directory_cstr = _log_directory.c_str();

	//if directory of log is not existed
	if (S_OK != io::get_is_directoryW(_log_directory_cstr))
	{
		//Create the directory of log inside the root directory
		io::create_directoryW(_log_directory_cstr);
	}

	auto _log_file_path = _log_directory + io::get_unique_nameW() + L".wLog";

	std::wstring _created_time = L"\t\"Time\"             : \"" + get_date_timeW() + L"\",\r\n";
	std::wstring _app_name = L"\t\"Application Name\" : \"" + pAppName + L"\",\r\n";

	this->_log_file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
	this->_log_file.open(_log_file_path.c_str());

#elif defined(__ANDROID) || defined(__linux)

bool w_logger::initialize(_In_z_ const std::string pAppName, _In_z_ const std::string pLogPath)
{
	auto _log_directory = pLogPath + "/Log/";
	auto _log_directory_cstr = _log_directory.c_str();

	//if directory of log is not existed
	if (!io::get_is_directory(_log_directory_cstr))
	{
		//Create the directory of log inside the root directory
		io::create_directory(_log_directory_cstr);
	}

	auto _log_file_path = _log_directory + io::get_unique_name() + ".wLog";

	std::string _created_time = "\t\"Time\"             : \"" + get_date_time() + "\",\r\n";
	std::string _app_name = "\t\"Application Name\" : \"" + pAppName + "\",\r\n";

	this->_log_file.open(_log_file_path.c_str());


#endif

	this->_is_released = false;

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
	auto _buffer = this->get_buffer();
	for (size_t i = 0; i < _buffer.size(); ++i)
	{
		this->_log_file << _buffer[i].c_str();
	}

	this->_log_file.flush();

	return this->_log_file.is_open();
}

#elif defined(__UNIVERSAL)

bool w_logger::initialize(_In_z_ const std::wstring pAppName)
{
	//if directory of log is not existed
	auto _local_folder = Windows::Storage::ApplicationData::Current->LocalFolder;
	auto _log_file_path = _local_folder->Path + ref new Platform::String(L"\\log\\") + ref new Platform::String((io::get_unique_nameW() + L".wLog").c_str());

	auto _function = [this, _log_file_path, pAppName]() ->bool
	{
		this->_log_file.open(_log_file_path->Data());

		std::wstring _created_time = L"\t\"Time\"             : \"" + get_date_timeW() + L"\",\r\n";
		std::wstring _app_name = L"\t\"Application Name\" : \"" + pAppName + L"\",\r\n";


		std::wstring _version = L"\t\"Version\"          : \"" + std::to_wstring(WOLF_MajorVersion) + L":" + std::to_wstring(WOLF_MinorVersion) +
			L":" + std::to_wstring(WOLF_PatchVersion) + L":" + std::to_wstring(WOLF_DebugVersion) + L"\",\r\n";

		this->_log_file << L"{\r\n";
		this->_log_file << L"\t\"Project\"          : \"Wolf Engine(http://WolfSource.io). Copyright(c) Pooya Eimandar(http://PooyaEimandar.com). All rights reserved.\",\r\n";
		this->_log_file << L"\t\"Info\"             : \"contact@WolfSource.io\r\n";
		this->_log_file << _version.c_str();
		this->_log_file << _created_time.c_str();
		this->_log_file << _app_name.c_str();
		this->_log_file << L"\t\"Logs\":{\r\n";

		//store all buffered logs
		auto _buffer = this->get_buffer();
		for (size_t i = 0; i < _buffer.size(); ++i)
		{
			this->_log_file << _buffer[i].c_str();
		}

		this->_log_file.flush();

		return this->_log_file.is_open();
	};

	wolf::system::io::get_directory_exists_async(_local_folder, "log").then([=](_In_ BOOL pExists)
	{
		if (!pExists)
		{
			wolf::system::io::create_directory_async(_local_folder, "log").then([=](Windows::Storage::StorageFolder^ pFolder)
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

#endif //defined(__WIN32) || defined(__ANDROID)

void w_logger::printf(std::wstring pMsg)
{
	std::unique_lock<std::mutex> _lock(this->_mutex);
	this->_msgs.push_back(pMsg + L"\r\n");
}

void w_logger::clearf()
{
	std::unique_lock<std::mutex> _lock(this->_mutex);
	this->_msgs.clear();
}

std::vector<std::wstring> w_logger::get_buffer()
{
	return this->_msgs;
}

void w_logger::flush()
{
	this->_log_file.flush();
}

void w_logger::write(_In_z_ std::string pMsg, _In_z_ const std::string pState)
{
	auto _msg = std::wstring(pMsg.begin(), pMsg.end());
	auto _state = std::wstring(pState.begin(), pState.end());
	write(_msg, _state);
	_msg.clear();
	_state.clear();
}

void w_logger::write(_In_z_ std::wstring pMsg, _In_z_ const std::wstring pState)
{
	pMsg = L"\t\t\"" + get_date_timeW() + L"\"" + L": {\"msg\":\"" + (pMsg.empty() ? L"NULL" : pMsg) + L"\",\"state\":\"" + pState + L"\"},\r\n";

#if defined(__WIN32) || defined(__UNIVERSAL)
	OutputDebugString(pMsg.c_str());
#elif defined(__linux)
        std::cout << std::string(pMsg.begin(), pMsg.end()).c_str();
#elif defined(__ANDROID)
	if (pState == L"Warning")
	{
		LOGW(std::string(pMsg.begin(), pMsg.end()).c_str());
	}
	else if (pState == L"Error")
	{
		LOGE(std::string(pMsg.begin(), pMsg.end()).c_str());
	}
	else
	{
		LOGI(std::string(pMsg.begin(), pMsg.end()).c_str());
	}
#elif defined(MAYA)
	switch (pState)
	{
	default:
		"Info" :
			MGlobal::displayInfo(Msg.c_str());
		break;
		"Warning":
		MGlobal::displayWarning(Msg.c_str());
		break;
		"Error":
		MGlobal::displayError(Msg.c_str());
		break;
	}
#endif

	if (this->_log_file.is_open())
	{
		this->_log_file << pMsg.c_str();
		this->_log_file.flush();
	}
	else
	{
		//log file is not available yet, so store it in to the buffer
		std::unique_lock<std::mutex> _lock(this->_mutex);
		this->_msgs.push_back(pMsg);
	}
}

void w_logger::user(_In_z_ std::wstring pMsg)
{
	write(pMsg, L"User");
}

void w_logger::user(_In_z_ std::string pMsg)
{
	write(pMsg, "User");
}

void w_logger::warning(_In_z_ std::wstring pMsg)
{
	write(pMsg, L"Warning");
}

void w_logger::warning(_In_z_ std::string pMsg)
{
	write(pMsg, "Warning");
}

void w_logger::error(_In_z_ std::wstring pMsg)
{
	write(pMsg, L"Error");
}

void w_logger::error(_In_z_ std::string pMsg)
{
	write(pMsg, "Error");
}

ULONG w_logger::release()
{
	if (this->_is_released) return 0;

	write("Wolf shutting down");
	this->_log_file << L"\r\n\t}\r\n}";
	this->_log_file.flush();
	this->_log_file.close();

	return 1;
}

//Declaration of extern logger as shared 

#ifdef __WIN32
#pragma data_seg (".shared")
#endif

wolf::system::w_logger logger;

#ifdef __WIN32
#pragma data_seg ()
#pragma comment(linker,"/SECTION:.shared,RWS")
#endif

