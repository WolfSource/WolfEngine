#include "w_process.h"
#include <apr-1/apr_general.h>
#include <io/w_io.h>

#ifdef W_PLATFORM_WIN
#include <tlhelp32.h>//for checking process
#include <psapi.h>//list all processes
#endif

W_RESULT w_process_info_terminate(_Inout_ w_process_info* pProcessInfo)
{
	if (!pProcessInfo)
	{
		return APR_BADARG;
	}

	w_free(pProcessInfo->class_name);
	w_free(pProcessInfo->process_name);
	w_free(pProcessInfo->title_name);
#ifdef W_PLATFORM_WIN
	CloseHandle(pProcessInfo->handle);
#endif

	w_free(pProcessInfo);
}

W_RESULT w_process_kill_by_id(_In_ unsigned long pProcessID)
{
	W_RESULT _hr = W_SUCCESS;

#ifdef W_PLATFORM_WIN

	// Get a handle to the process.
	HANDLE _process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pProcessID);

	// Get the process name.
	if (_process)
	{
		if (!TerminateProcess(_process, 0))
		{
			_hr = W_FAILURE;
		}

		CloseHandle(_process);
	}

#else

#endif

	return _hr;
}

const wchar_t* w_process_get_name_by_id(_In_ unsigned long pProcessID)
{
#ifdef W_PLATFORM_WIN
	TCHAR _process_name[MAX_PATH] = L"<unknown>";

	// Get a handle to the process.
	HANDLE _process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pProcessID);

	int _ = sizeof(_process_name) / sizeof(TCHAR);
	// Get the process name.
	if (_process)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(_process, &hMod, sizeof(hMod), &cbNeeded))
		{
			int _ = sizeof(_process_name) / sizeof(TCHAR);
			GetModuleBaseName(_process, hMod, _process_name, _);
		}

		CloseHandle(_process);
	}

	size_t _len = wcslen(_process_name);
	wchar_t* _name = w_malloc(sizeof(wchar_t) * _len, "w_process_get_name_by_id");
	return wcscpy(_name, _process_name);
#else

#endif

}

W_RESULT w_process_enum_all_processesW(_Inout_ wchar_t** pProcessLists)
{
	const char* _trace_info = "w_process_enum_all_processes";
	char* _process_names = w_malloc(sizeof(char), _trace_info);
	if (!_process_names)
	{
		W_ASSERT_P(false,
			"could not allocate memory for _process_names. trace info: %s", _trace_info);
		return W_FAILURE;
	}
	wchar_t* _process_id_tmp = w_malloc(MAX_PATH, _trace_info);
	if (!_process_id_tmp)
	{
		W_ASSERT_P(false,
			"could not allocate memory for _process_id_tmp. trace info: %s", _trace_info);
		return W_FAILURE;
	}

#ifdef W_PLATFORM_WIN

	DWORD _processes[1024], _needed, _number_of_processes;
	if (!EnumProcesses(_processes, sizeof(_processes), &_needed)) return "";

	// Calculate how many process identifiers were returned.
	_number_of_processes = _needed / sizeof(DWORD);

	// Print the name and process identifier for each process.
	*pProcessLists = w_wstrcat(
		NULL,
		*pProcessLists,
		L"Process Name : Process ID\n",
		NULL);

	wchar_t* _process_name, _process_id;

	for (size_t i = 0; i < _number_of_processes; i++)
	{
		_ultow(_processes[i], _process_id_tmp, 10);
		//_process_name = w_process_get_name_by_id(_processes[i]);
		//print the process name and id
		*pProcessLists = w_wstrcat(
			NULL,
			*pProcessLists,
			L"_process_name",
			L" : ",
			_process_id_tmp,
			L"\n",
			NULL);
	}

	w_free(_process_id_tmp);

#endif

	return W_SUCCESS;
}

W_RESULT w_process_enum_all_processes(_Inout_ char** pProcessLists)
{
	wchar_t* _process_lists = L"";
	if (w_process_enum_all_processesW(&_process_lists) == W_SUCCESS)
	{
		size_t _len = wcslen(_process_lists);
		size_t _dst_len;
		return w_io_wchar_ptr_to_char_ptr(
			_process_lists,
			_len,
			pProcessLists);
	}

	return W_FAILURE;
}

//static std::wstringstream ProcessesList;

//const std::wstring w_process::enum_all_processes()
//{
//#ifdef __WIN32
//	DWORD aProcesses[1024], cbNeeded, cProcesses;
//	unsigned int i;
//
//	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) return L"";
//
//	// Calculate how many process identifiers were returned.
//	cProcesses = cbNeeded / sizeof(DWORD);
//
//	// Print the name and process identifier for each process.
//	ProcessesList.str(L"");
//	ProcessesList.clear();
//	ProcessesList << L"Process Name : Process ID\n";
//
//	std::wstring _process_name;
//	for (i = 0; i < cProcesses; i++)
//	{
//		if (aProcesses[i] != 0)
//		{
//			_process_name = get_process_name_by_ID(aProcesses[i]);
//			// Print the process name and identifier.
//			ProcessesList << _process_name << L" " << aProcesses[i] << "\n";
//		}
//	}
//
//	ProcessesList << "\0";
//	std::wstring _result = ProcessesList.str();
//	
//	ProcessesList.str(L"");
//	ProcessesList.clear();
//
//	return _result;
//#endif
//}
//
//bool w_process::check_for_number_of_running_instances_from_process(_In_z_ const wchar_t* pProcessName, 
//	_In_ size_t pNumberOfRunningInstnacesToBeChecked)
//{
//	bool _result = false;
//
//#ifdef __WIN32
//
//	size_t _exists = 0;
//	PROCESSENTRY32 _entry;
//	_entry.dwSize = sizeof(PROCESSENTRY32);
//
//	HANDLE _snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
//
//	if (Process32First(_snapshot, &_entry))
//	{
//		while (Process32Next(_snapshot, &_entry))
//		{
//			if (!_wcsicmp(_entry.szExeFile, pProcessName))
//			{
//				_exists++;
//				if (_exists == pNumberOfRunningInstnacesToBeChecked)
//				{
//					_result = true;
//					break;
//				}
//			}
//		}
//	}
//	CloseHandle(_snapshot);
//
//#else
//
//
//#endif
//
//	return _result;
//}
//
//size_t w_process::get_number_of_running_instances_from_process(_In_z_ const wchar_t* pProcessName)
//{
//#ifdef __WIN32
//
//	size_t _exists = 0;
//	PROCESSENTRY32 _entry;
//	_entry.dwSize = sizeof(PROCESSENTRY32);
//
//	HANDLE _snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
//
//	if (Process32First(_snapshot, &_entry))
//	{
//		while (Process32Next(_snapshot, &_entry))
//		{
//			if (!_wcsicmp(_entry.szExeFile, pProcessName))
//			{
//				_exists++;
//			}
//		}
//	}
//	CloseHandle(_snapshot);
//
//#else
//
//
//#endif
//
//	return _exists;
//}
//
//#ifdef __WIN32
//w_process_info* w_process::create_process(
//	_In_z_ const wchar_t* pPathtoProcess,
//	_In_z_ const wchar_t* pCmdsArg,
//	_In_z_ const wchar_t* pCurrentDirectoryPath,
//	_In_  const long long pWaitAfterRunningProcess,
//	_In_ DWORD pCreationFlags)
//{
//	STARTUPINFO _startup_info;
//	ZeroMemory(&_startup_info, sizeof(_startup_info));
//
//	PROCESS_INFORMATION _process_info;
//	ZeroMemory(&_process_info, sizeof(_process_info));
//
//	if (CreateProcess(
//		pPathtoProcess,
//		LPWSTR(pCmdsArg),
//		NULL,
//		NULL,
//		FALSE,
//		pCreationFlags,
//		NULL,
//		pCurrentDirectoryPath,
//		&_startup_info,
//		&_process_info))
//	{
//		WaitForSingleObject(_process_info.hProcess, pWaitAfterRunningProcess);
//
//		auto _w_process_info = new w_process_info();
//		_w_process_info->info = _process_info;
//		
//		DWORD _err_val = GetLastError();
//		_w_process_info->error_code = std::error_code(_err_val, std::system_category());
//
//		return _w_process_info;
//	}
//	else
//	{
//		logger.error(L"Process ID: \"{}\" could not run with arg \"{}\" . Error code : \"{}\"", pPathtoProcess, pCmdsArg, GetLastError());
//		return nullptr;
//	}
//}
//
//bool w_process::kill_process(_In_ w_process_info* pProcessInfo)
//{
//	if (!pProcessInfo) return false;
//	
//	if (!TerminateProcess(pProcessInfo->info.hProcess, 0))
//	{
//		logger.error(L"Error on killing process id: {}", pProcessInfo->info.dwProcessId);
//		return false;
//	}
//
//	if (!CloseHandle(pProcessInfo->info.hThread))
//	{
//		logger.error("Error on closing thread id: {} for process id: {}", pProcessInfo->info.dwThreadId, pProcessInfo->info.dwProcessId);
//	}
//	if (!CloseHandle(pProcessInfo->info.hProcess))
//	{
//		logger.error("Error on closing process id: {}", pProcessInfo->info.dwProcessId);
//		return false;
//	}
//
//	return true;
//}
//
//bool w_process::kill_all_processes(std::initializer_list<const wchar_t*> pProcessNames)
//{
//	W_RESULT _hr = W_PASSED;
//
//#ifdef __WIN32
//	DWORD aProcesses[1024], cbNeeded, cProcesses;
//	unsigned int i;
//
//	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) return L"";
//
//	// Calculate how many process identifiers were returned.
//	cProcesses = cbNeeded / sizeof(DWORD);
//
//	DWORD _process_id = 0;
//	std::wstring _process_name;
//	for (i = 0; i < cProcesses; i++)
//	{
//		_process_id = aProcesses[i];
//		if (_process_id != 0)
//		{
//			_process_name = get_process_name_by_ID(_process_id);
//			for (auto _name : pProcessNames)
//			{
//				if (wcscmp(_process_name.c_str(), _name) == 0)
//				{
//					if (kill_process_by_ID(_process_id) == W_FAILED)
//					{
//						_hr = W_FAILED;
//					}
//				}
//			}
//
//		}
//	}
//
//#endif
//
//	return _hr == W_PASSED;
//}
//
//#endif
