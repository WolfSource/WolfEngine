#include "Wolf.System.Shared.PCH.h"
#include "W_Logger.h"

using namespace Wolf::System;

bool W_Logger::Initialize(std::string pAppName)
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

	std::string version = "\t\"Version\"          : \"" + std::to_string(WMajorVersion) + ":" + std::to_string(WMinorVersion) + ":" + std::to_string(WPatchVersion) + ":" + std::to_string(WDebugVersion) + "\",\r\n";
	std::string createdTime = "\t\"Time\"             : \"" + GetTime() + "\",\r\n";
	std::string appName = "\t\"Application Name\" : \"" + pAppName + "\",\r\n";

	logFile.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
	logFile.open(logFilePath.c_str());

	logFile << "{\r\n";
	logFile << "\t\"Project\"          : \"Wolf Engine(http://WolfStudio.co). Copyright(c) Pooya Eimandar(http://PooyaEimandar.com). All rights reserved.\",\r\n";
	logFile << "\t\"Info\"             : \"Please direct any bug to \"hello@WolfStudio.co\" or tweet \"@PooyaEimandar\" on twitter - source on https://github.com/PooyaEimandar/WolfEngine \",\r\n";
	logFile << version.c_str();
	logFile << createdTime.c_str();
	logFile << appName.c_str();
	logFile << "\t\"Logs\":{\r\n";
	logFile.flush();
	return logFile.is_open();
}

void W_Logger::Write(std::wstring pMsg, std::string pState)
{
	auto msg = std::string(pMsg.begin(), pMsg.end());
	Write(msg, pState);
	msg.clear();
}

void W_Logger::Write(std::string pMsg, std::string pState)
{
	if (pMsg.empty())
	{
		pMsg = "[Wolf] : Info : \tNULL : " + GetTime();
	}
	else
	{
		pMsg = "\"" + GetTime() + "\"" + ": {\"msg\":\"" + pMsg + "\",\"state\":\"" + pState + "\"},";
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
	pMsg = "\t\t" + pMsg + "\r\n";
	OutputDebugStringA(pMsg.c_str());
#endif

	logFile << pMsg.c_str();
	logFile.flush();
}

void W_Logger::Warning(std::string pMsg)
{
	Write(pMsg, "Warning");
}

void W_Logger::Error(std::string pMsg)
{
	Write(pMsg, "Error");
}

ULONG W_Logger::Release()
{
	if (this->isReleased) return 0;

	Write("Wolf shut down");
	logFile << "\r\n\t}\r\n}";
	logFile.flush();
	logFile.close();

	return 1;
}

//Declration of extern logger as shared 
#pragma data_seg (".shared")
Wolf::System::W_Logger Logger;
#pragma data_seg ()

#pragma comment(linker,"/SECTION:.shared,RWS")

