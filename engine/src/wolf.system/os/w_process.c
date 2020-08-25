#include "w_process.h"
#include <apr-1/apr_general.h>
#include <io/w_io.h>

#ifdef W_PLATFORM_WIN
#include <tlhelp32.h>//for checking process
#include <psapi.h>//list all processes
#endif

size_t w_process_get_count_of_instances(_In_z_ const wchar_t* pProcessName)
{
	size_t _instances = 0;

#if defined W_PLATFORM_WIN

	PROCESSENTRY32 _entry;
	_entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE _snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (_snapshot)
	{
		if (Process32First(_snapshot, &_entry))
		{
			while (Process32Next(_snapshot, &_entry))
			{
				if (!_wcsicmp(_entry.szExeFile, pProcessName))
				{
					_instances++;
				}
			}
		}
		CloseHandle(_snapshot);
	}

#elif defined W_PLATFORM_OSX

	pid_t pids[2048];
	std::wstring _input_name, _compare_name;
	_input_name = std::wstring(name);
	size_t size = 0;

	int bytes = proc_listpids(PROC_ALL_PIDS, 0, pids, sizeof(pids));
	int n_proc = bytes / sizeof(pids[0]);
	int i;
	for (i = 0; i < n_proc; i++)
	{
		struct proc_bsdinfo proc;
		int st = proc_pidinfo(pids[i], PROC_PIDTBSDINFO, 0, &proc, sizeof(proc));
		if (st == sizeof(proc))
		{
			size = std::strlen(proc.pbi_name);
			if (size > 0)
			{
				_compare_name.resize(size);
				std::mbstowcs(&_compare_name[0], proc.pbi_name, size);
			}

			if (!wcscmp(_input_name.data(), _compare_name.data()))
			{
				_instances++;
			}
		}

#elif defined W_PLATFORM_LINUX

	DIR* dir;
	struct dirent* ent;
	char* endptr;
	char buf[512];
	std::wstring _input_name, _compare_name;
	_input_name = std::wstring(name);
	size_t size = 0;

	if (!(dir = opendir("/proc"))) {
		perror("can't open /proc");
		return num;
	}

	while ((ent = readdir(dir)) != NULL)
	{
		/* if endptr is not a null character, the directory is not
		 * entirely numeric, so ignore it */
		long lpid = strtol(ent->d_name, &endptr, 10);
		if (*endptr != '\0')
		{
			continue;
		}

		/* try to open the cmdline file */
		snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
		FILE* fp = fopen(buf, "r");

		if (fp)
		{
			if (fgets(buf, sizeof(buf), fp) != NULL)
			{
				/* check the first token in the file, the program name */
				char* first = strtok(buf, " ");

				size = std::strlen(buf);
				if (size > 0)
				{
					_compare_name.resize(size);
					std::mbstowcs(&_compare_name[0], buf, size);
				}


				auto _tmp = std::string(buf);
				auto pos = _compare_name.find_last_of('/');
				std::wstring _file = _compare_name;
				if (pos != std::wstring::npos)
					_file = _compare_name.substr(pos + 1);
				if (!wcscmp((const wchar_t*)_file.data(), (const wchar_t*)_input_name.data()))
				{
					_instances++;
				}
			}
			fclose(fp);
		}

	}

	closedir(dir);
#endif

	return _instances;
}

const wchar_t* w_process_get_process_name_by_id(
	_Inout_ w_mem_pool pMemPool,
	_In_ unsigned long pProcessID)
{
	const char* _trace_info = "w_process_get_process_name_by_id";
	if (!pMemPool)
	{
		W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
		return APR_BADARG;
	}

#ifdef W_PLATFORM_WIN
	wchar_t* _process_name = (wchar_t*)w_malloc(
		pMemPool,
		MAX_PATH * sizeof(wchar_t),
		_trace_info);// = L"<unknown>";
	if (!_process_name)
	{
		W_ASSERT(false, "could allocate memory. trace info: w_process_get_name_by_id");
		return W_FAILURE;
	}

	// Get a handle to the process.
	HANDLE _process = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, 
		pProcessID);

	// Get the process name.
	wchar_t* _name = NULL;
	if (_process)
	{
		HMODULE _hMod;
		DWORD _needed;

		if (EnumProcessModules(_process, &_hMod, sizeof(_hMod), &_needed))
		{
			GetModuleBaseName(_process, _hMod, _process_name, MAX_PATH);
		}
		CloseHandle(_process);

		size_t _len = wcslen(_process_name);
		_name = w_malloc(
			pMemPool,
			sizeof(wchar_t) * _len, 
			_trace_info);
		if (_name)
		{
			_name = wcscpy(_name, _process_name);
		}
	}

	w_free(_process_name);
	return _name;
#else

#endif

}

W_RESULT w_process_print_allW(
	_Inout_ w_mem_pool pMemPool,
	_Inout_ wchar_t** pProcessLists)
{
	const char* _trace_info = "w_process_enum_all_processes";
	if (!pMemPool)
	{
		W_ASSERT_P(false, "missing memory pool. trace info: %s", _trace_info);
		return APR_BADARG;
	}
	char* _process_names = w_malloc(pMemPool, sizeof(char), _trace_info);
	if (!_process_names)
	{
		W_ASSERT_P(false,
			"could not allocate memory for _process_names. trace info: %s", _trace_info);
		return W_FAILURE;
	}
	wchar_t* _process_id_tmp = w_malloc(pMemPool, MAX_PATH, _trace_info);
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

W_RESULT w_process_print_all(
	_Inout_ w_mem_pool pMemPool,
	_Inout_ char** pProcessLists)
{
	wchar_t* _process_lists = L"";
	if (w_process_print_allW(pMemPool, &_process_lists) == W_SUCCESS)
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

#ifdef W_PLATFORM_WIN

//W_RESULT w_process_create(
//	_In_z_ const wchar_t* pPathToProcess,
//	_In_z_ const wchar_t* pCmdsArg,
//	_In_z_ const wchar_t* pCurrentDirectoryPath,
//	_In_  const double pWaitAfterRunningProcess,
//	_In_ DWORD pCreationFlags,
//	_Out_ w_process_info** pProcessInfo)
//{
	/*STARTUPINFO _startup_info;
	ZeroMemory(&_startup_info, sizeof(_startup_info));

	PROCESS_INFORMATION _process_info;
	ZeroMemory(&_process_info, sizeof(_process_info));

	if (CreateProcessW(
		pPathToProcess,
		pCmdsArg,
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
		return W_FAILURE;
	}*/
//}

#endif

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

//DWORD FindProcessId(const char* processname)
//{
//	HANDLE hProcessSnap;
//	PROCESSENTRY32 pe32;
//	DWORD result = NULL;
//
//	// Take a snapshot of all processes in the system.
//	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (INVALID_HANDLE_VALUE == hProcessSnap) return(FALSE);
//
//	pe32.dwSize = sizeof(PROCESSENTRY32); // <----- IMPORTANT
//
//	// Retrieve information about the first process,
//	// and exit if unsuccessful
//	if (!Process32First(hProcessSnap, &pe32))
//	{
//		CloseHandle(hProcessSnap);          // clean the snapshot object
//		printf("!!! Failed to gather information on system processes! \n");
//		return(NULL);
//	}
//	LPCSTR pOut;
//
//	do
//	{
//		w_io_wchar_ptr_to_char_ptr(
//			pe32.szExeFile,
//			200,
//			&pOut);
//
//		printf("Checking process %ls\n", pe32.szExeFile);
//		if (0 == strcmp(processname, (pOut)))
//		{
//
//			result = pe32.th32ProcessID;
//			break;
//		}
//	} while (Process32Next(hProcessSnap, &pe32));
//
//	CloseHandle(hProcessSnap);
//
//	return result;
//}