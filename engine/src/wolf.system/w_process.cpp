#include "w_system_pch.h"
#include "w_process.h"
#include <stdlib.h>

#ifdef __WIN32
#include <tlhelp32.h>//for checking process
#include <psapi.h>//list all processes
#endif

using namespace wolf;
using namespace wolf::system;

static std::wstringstream ProcessesList;

W_RESULT w_process::kill_process_by_ID(_In_ const unsigned long& pProcessID)
{
	W_RESULT _hr = W_PASSED;

#ifdef __WIN32

	// Get a handle to the process.
	HANDLE _hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pProcessID);

	// Get the process name.
	if (_hProcess != NULL)
	{
		if (!TerminateProcess(_hProcess, 0))
		{
			logger.error(L"Error on killing process id: {}", pProcessID);
			_hr = W_FAILED;
		}
	}

	// Release the handle to the process.
	CloseHandle(_hProcess);
#else

#endif

	return _hr;
}

std::wstring w_process::get_process_name_by_ID(_In_ const unsigned long& pProcessID)
{
	std::wstring _name;
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

	_name = szProcessName;
	
	// Release the handle to the process.
	CloseHandle(hProcess);
#else

#endif

	return _name;
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

	std::wstring _process_name;
	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			_process_name = get_process_name_by_ID(aProcesses[i]);
			// Print the process name and identifier.
			ProcessesList << _process_name << L" " << aProcesses[i] << "\n";
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

size_t w_process::get_number_of_running_instances_from_process(_In_z_ const wchar_t* pProcessName)
{
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
			}
		}
	}
	CloseHandle(_snapshot);

#else


#endif

	return _exists;
}

#ifdef __WIN32
w_process_info* w_process::create_process(
	_In_z_ const wchar_t* pPathtoProcess,
	_In_z_ const wchar_t* pCmdsArg,
	_In_z_ const wchar_t* pCurrentDirectoryPath,
	_In_  const long long pWaitAfterRunningProcess,
	_In_ DWORD pCreationFlags)
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
		pCreationFlags,
		NULL,
		pCurrentDirectoryPath,
		&_startup_info,
		&_process_info))
	{
		WaitForSingleObject(_process_info.hProcess, pWaitAfterRunningProcess);

		auto _w_process_info = new w_process_info();
		_w_process_info->info = _process_info;
		
		DWORD _err_val = GetLastError();
		_w_process_info->error_code = std::error_code(_err_val, std::system_category());

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

bool w_process::kill_all_processes(_In_ std::initializer_list<const wchar_t*> pProcessNames)
{
	W_RESULT _hr = W_PASSED;

#ifdef __WIN32
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) return L"";

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	DWORD _process_id = 0;
	std::wstring _process_name;
	for (i = 0; i < cProcesses; i++)
	{
		_process_id = aProcesses[i];
		if (_process_id != 0)
		{
			_process_name = get_process_name_by_ID(_process_id);
			for (auto _name : pProcessNames)
			{
				if (wcscmp(_process_name.c_str(), _name) == 0)
				{
					if (kill_process_by_ID(_process_id) == W_FAILED)
					{
						_hr = W_FAILED;
					}
				}
			}

		}
	}

#endif

	return _hr == W_PASSED;
}

bool w_process::force_kill_process_by_name(_In_z_ const std::wstring pProcessName,
	_In_ const bool pTerminateChildProcesses)
{
	std::wstring _cmd = L"taskkill /IM " + pProcessName + L" /F";
	if (pTerminateChildProcesses)
	{
		_cmd += L" /T";
	}
	_wsystem(_cmd.c_str());
	return true;
}

bool w_process::force_kill_process_by_name_as_admin(
	_In_z_ const std::wstring pProcessName,
	_In_z_ const std::wstring pUserNameName,
	_In_z_ const std::wstring pPassword,
	_In_ const bool pTerminateChildProcesses)
{
	std::wstring _cmd = L"taskkill /IM " + pProcessName +
		L" /U " + pUserNameName +
		L" /P " + pPassword +
		L" /F";
	if (pTerminateChildProcesses)
	{
		_cmd += L" /T";
	}
	_wsystem(_cmd.c_str());
	return true;
}

bool w_process::force_kill_process(_In_z_ const DWORD pProcessID, 
	_In_ const bool pTerminateChildProcesses)
{
	std::wstring _cmd = L"taskkill /PID " + std::to_wstring(pProcessID) + L" /F";
	if (pTerminateChildProcesses)
	{
		_cmd += L" /T";
	}
	_wsystem(_cmd.c_str());
	return true;
}

bool w_process::force_kill_process_by_name_as_admin(
	_In_ const DWORD pProcessID,
	_In_z_ const std::wstring pUserNameName,
	_In_z_ const std::wstring pPassword,
	_In_ const bool pTerminateChildProcesses)
{
	std::wstring _cmd = L"taskkill /PID " + std::to_wstring(pProcessID) +
		L" /U " + pUserNameName +
		L" /P " + pPassword +
		L" /F";
	if (pTerminateChildProcesses)
	{
		_cmd += L" /T";
	}
	_wsystem(_cmd.c_str());
	return true;
}

#endif
