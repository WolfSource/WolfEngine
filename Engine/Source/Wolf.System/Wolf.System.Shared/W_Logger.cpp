#include "Wolf.System.Shared.PCH.h"
#include "W_Logger.h"

using namespace Wolf::System;

bool W_Logger::Initialize(std::wstring pAppName)
{
	this->isReleased = false;

	std::wstring logFilePath;

#if defined(__WIN32)

	auto directoryPath = IO::GetCurrentDirectory();
	auto logDirectory = directoryPath + L"Log\\";

	//if directory of log is not existed
	if (!IO::IsDirectoryExists(logDirectory))
	{
		//Create the directory of log inside the root directory
		IO::CreateWDirectory(logDirectory.c_str());
	}

	logFilePath = logDirectory + IO::GetUniqueName() + L".wLog";

	//OutputDebugString(logFilePath.c_str());

#endif

	std::wstring version = L"\t\"Version\"          : \"" + std::to_wstring(WMajorVersion) + L":" + std::to_wstring(WMinorVersion) + 
		L":" + std::to_wstring(WPatchVersion) + L":" + std::to_wstring(WDebugVersion) + L"\",\r\n";
	std::wstring createdTime = L"\t\"Time\"             : \"" + GetTimeW() + L"\",\r\n";
	std::wstring appName = L"\t\"Application Name\" : \"" + pAppName + L"\",\r\n";

	logFile.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
	logFile.open(logFilePath.c_str());

	logFile << L"{\r\n";
	logFile << L"\t\"Project\"          : \"Wolf Engine(http://WolfStudio.co). Copyright(c) Pooya Eimandar(http://PooyaEimandar.com). All rights reserved.\",\r\n";
	logFile << L"\t\"Info\"             : \"Please direct any bug to \"hello@WolfStudio.co\" or tweet \"@Wolf_Engine\" on twitter - source on https://github.com/PooyaEimandar/WolfEngine \",\r\n";
	logFile << version.c_str();
	logFile << createdTime.c_str();
	logFile << appName.c_str();
	logFile << L"\t\"Logs\":{\r\n";
	logFile.flush();
	return logFile.is_open();
}

void W_Logger::Write(std::string pMsg, const std::string pState)
{
	auto msg = std::wstring(pMsg.begin(), pMsg.end());
	auto state = std::wstring(pState.begin(), pState.end());
	Write(msg, state);
	msg.clear();
	state.clear();
}

void W_Logger::Write(std::wstring pMsg, const std::wstring pState)
{
	if (pMsg.empty())
	{
		pMsg = L"[Wolf] : Info : \tNULL : " + GetTimeW();
	}
	else
	{
		pMsg = L"\"" + GetTimeW() + L"\"" + L": {\"msg\":\"" + pMsg + L"\",\"state\":\"" + pState + L"\"},";
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

	logFile << pMsg.c_str();
	logFile.flush();
}

void W_Logger::User(std::wstring pMsg)
{
	Write(pMsg, L"User");
}

void W_Logger::User(std::string pMsg)
{
	Write(pMsg, "User");
}

void W_Logger::Warning(std::wstring pMsg)
{
	Write(pMsg, L"Warning");
}

void W_Logger::Warning(std::string pMsg)
{
	Write(pMsg, "Warning");
}

void W_Logger::Error(std::wstring pMsg)
{
	Write(pMsg, L"Error");
}

void W_Logger::Error(std::string pMsg)
{
	Write(pMsg, "Error");
}

ULONG W_Logger::Release()
{
	if (this->isReleased) return 0;

	Write(L"Wolf shut down");
	logFile << L"\r\n\t}\r\n}";
	logFile.flush();
	logFile.close();

	return 1;
}

//Declration of extern logger as shared 
#pragma data_seg (".shared")
Wolf::System::W_Logger Logger;
#pragma data_seg ()

#pragma comment(linker,"/SECTION:.shared,RWS")

