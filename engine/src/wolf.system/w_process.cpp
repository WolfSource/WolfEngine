#include "w_system_pch.h"
#include "w_process.h"

#ifdef __WIN32
#include <tlhelp32.h>//for checking process
#include <psapi.h>//list all processes
#endif

using namespace wolf;
using namespace wolf::system;

static std::wstringstream ProcessesList;

void w_process::print_process_name_ID(_In_ const unsigned long& pProcessID)
{
#ifdef __WIN32
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

	// Get a handle to the process.
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pProcessID);

	// Get the process name.
	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
		}
	}

	// Print the process name and identifier.
	ProcessesList << szProcessName  << L" " << pProcessID << "\n";

	// Release the handle to the process.
	CloseHandle(hProcess);
#else


#endif
}

const std::wstring w_process::enum_all_processes()
{
#ifdef __WIN32
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) return L"";

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.
	ProcessesList.str(L"");
	ProcessesList.clear();
	ProcessesList << L"Process Name : Process ID\n";

	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			print_process_name_ID(aProcesses[i]);
		}
	}

	ProcessesList << "\0";
	std::wstring _result = ProcessesList.str();
	
	ProcessesList.str(L"");
	ProcessesList.clear();

	return _result;
#endif
}

bool w_process::check_for_number_of_running_instances_from_process(_In_z_ const wchar_t* pProcessName, 
	_In_ size_t pNumberOfRunningInstnacesToBeChecked)
{
	bool _result = false;

#ifdef __WIN32

	size_t _exists = 0;
	PROCESSENTRY32 _entry;
	_entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE _snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(_snapshot, &_entry))
	{
		while (Process32Next(_snapshot, &_entry))
		{
			if (!_wcsicmp(_entry.szExeFile, pProcessName))
			{
				_exists++;
				if (_exists == pNumberOfRunningInstnacesToBeChecked)
				{
					_result = true;
					break;
				}
			}
		}
	}
	CloseHandle(_snapshot);

#else


#endif

	return _result;
}

#ifdef __WIN32
w_process_info* w_process::create_process(
	_In_z_ const wchar_t* pPathtoProcess, 
	_In_z_ const wchar_t* pCmdsArg, 
	_In_z_ const wchar_t* pCurrentDirectoryPath,
	_In_  const unsigned long pWaitAfterRunningProcess)
{
	STARTUPINFO _startup_info;
	ZeroMemory(&_startup_info, sizeof(_startup_info));

	PROCESS_INFORMATION _process_info;
	ZeroMemory(&_process_info, sizeof(_process_info));

	if (CreateProcess(
		pPathtoProcess,
		LPWSTR(pCmdsArg),
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		pCurrentDirectoryPath,
		&_startup_info,
		&_process_info))
	{
		WaitForSingleObject(_process_info.hProcess, pWaitAfterRunningProcess);

		auto _w_process_info = new w_process_info();
		_w_process_info->info = _process_info;
		
		return _w_process_info;
	}
	else
	{
		logger.error(L"Process ID: \"{}\" could not run with arg \"{}\" . Error code : \"{}\"", pPathtoProcess, pCmdsArg, GetLastError());
		return nullptr;
	}
}

bool w_process::kill_process(_In_ w_process_info* pProcessInfo)
{
	if (!pProcessInfo) return false;
	
	if (!TerminateProcess(pProcessInfo->info.hProcess, 0))
	{
		logger.error(L"Error on killing process id: {}", pProcessInfo->info.dwProcessId);
		return false;
	}

	if (!CloseHandle(pProcessInfo->info.hThread))
	{
		logger.error("Error on closing thread id: {} for process id: {}", pProcessInfo->info.dwThreadId, pProcessInfo->info.dwProcessId);
	}
	if (!CloseHandle(pProcessInfo->info.hProcess))
	{
		logger.error("Error on closing process id: {}", pProcessInfo->info.dwProcessId);
		return false;
	}

	return true;
}

#endif