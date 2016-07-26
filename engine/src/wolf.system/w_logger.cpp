#include "w_system_pch.h"
#include "w_logger.h"

using namespace wolf::system;

bool w_logger::initialize(std::wstring pAppName)
{
	this->_isReleased = false;

	std::wstring logFilePath;

#ifdef WIN32

	auto _directoryPath = io::get_current_directory();
	auto _logDirectory = _directoryPath + L"Log\\";

	//if directory of log is not existed
	if (!io::get_is_directory(_logDirectory))
	{
		//Create the directory of log inside the root directory
		io::create_directory_w(_logDirectory.c_str());
	}

	logFilePath = _logDirectory + io::get_unique_name() + L".wLog";

	//OutputDebugString(logFilePath.c_str());

#endif

	std::wstring _version = L"\t\"Version\"          : \"" + std::to_wstring(WOLF_MajorVersion) + L":" + std::to_wstring(WOLF_MinorVersion) +
		L":" + std::to_wstring(WOLF_PatchVersion) + L":" + std::to_wstring(WOLF_DebugVersion) + L"\",\r\n";
	std::wstring _createdTime = L"\t\"Time\"             : \"" + get_date_time_w() + L"\",\r\n";
	std::wstring _appName = L"\t\"Application Name\" : \"" + pAppName + L"\",\r\n";

	_logFile.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
	_logFile.open(logFilePath.c_str());

	_logFile << L"{\r\n";
	_logFile << L"\t\"Project\"          : \"Wolf Engine(http://WolfSource.io). Copyright(c) Pooya Eimandar(http://PooyaEimandar.com). All rights reserved.\",\r\n";
	_logFile << L"\t\"Info\"             : \"contact@WolfSource.io\r\n";
	_logFile << _version.c_str();
	_logFile << _createdTime.c_str();
	_logFile << _appName.c_str();
	_logFile << L"\t\"Logs\":{\r\n";
	_logFile.flush();
	return _logFile.is_open();
}

void w_logger::printf(std::wstring pMsg)
{
	this->_msgs.push_back(pMsg + L"\r\n");
}

std::vector<std::wstring> w_logger::flushf() const
{
	return this->_msgs;
}

void w_logger::clearf()
{
	this->_msgs.clear();
}

void w_logger::write(std::string pMsg, const std::string pState)
{
	auto _msg = std::wstring(pMsg.begin(), pMsg.end());
	auto _state = std::wstring(pState.begin(), pState.end());
	write(_msg, _state);
	_msg.clear();
	_state.clear();
}

void w_logger::write(std::wstring pMsg, const std::wstring pState)
{
	if (pMsg.empty())
	{
		pMsg = L"[Wolf] : Info : \tNULL : " + get_date_time_w();
	}
	else
	{
		pMsg = L"\"" + get_date_time_w() + L"\"" + L": {\"msg\":\"" + pMsg + L"\",\"state\":\"" + pState + L"\"},";
	}
#if defined(__MAYA)
	switch (pState)
	{
	default:
	"Info":
		MGlobal::displayInfo(Msg.c_str());
		break;
	"Warning":
		MGlobal::displayWarning(Msg.c_str());
		break;
	"Error":
		MGlobal::displayError(Msg.c_str());
		break;
	}
#else
	pMsg = L"\t\t" + pMsg + L"\r\n";
	OutputDebugString(pMsg.c_str());
#endif

	this->_logFile << pMsg.c_str();
	this->_logFile.flush();
}

void w_logger::user(std::wstring pMsg)
{
	write(pMsg, L"User");
}

void w_logger::user(std::string pMsg)
{
	write(pMsg, "User");
}

void w_logger::warning(std::wstring pMsg)
{
	write(pMsg, L"Warning");
}

void w_logger::warning(std::string pMsg)
{
	write(pMsg, "Warning");
}

void w_logger::error(std::wstring pMsg)
{
	write(pMsg, L"Error");
}

void w_logger::error(std::string pMsg)
{
	write(pMsg, "Error");
}

ULONG w_logger::release()
{
	if (this->_isReleased) return 0;

	write(L"Wolf shut down");
	this->_logFile << L"\r\n\t}\r\n}";
	this->_logFile.flush();
	this->_logFile.close();

	return 1;
}

//Declration of extern logger as shared 
#pragma data_seg (".shared")
wolf::system::w_logger logger;
#pragma data_seg ()

#pragma comment(linker,"/SECTION:.shared,RWS")

