/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_process.h
	Description		 : enum all OS processes
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

#ifdef W_PLATFORM_OSX
typedef unsigned long   DWORD;
#endif

	typedef struct w_process_info_t
	{
#ifdef W_PLATFORM_WIN
		PROCESS_INFORMATION* info;
		HWND					handle;
		wchar_t* class_name;
		wchar_t* title_name;
		wchar_t* process_name;
		DWORD					error_code;
#endif
	} w_process_info_t;
	typedef w_process_info_t* w_process_info;

	/**
	 * get number of instances for a specific process
	 * @param pProcessName the name of process
	 * @return number of processes
	*/
	W_SYSTEM_EXPORT
		size_t w_process_get_count_of_instances(_In_z_ const wchar_t* pProcessName);

	/**
	 * check the process is running
	 * @param pProcessID the id of process
	 * @return result code
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_get_is_running(_In_ const unsigned long pProcessID);

	/**
	 * get a process name based on process id
	 * @param pMemPool The pool to allocate out of
	 * @param pProcessID process ID
	 * @param pProcessName the process name
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_get_name_by_id(
			_Inout_ w_mem_pool pMemPool,
			_In_ DWORD pProcessID,
			_Inout_ wchar_t** pProcessName);

	/**
	 * enumurate all processes names and print all in wide characters
	 * @param pMemPool The pool to allocate out of
	 * @param pProcessLists which will be updated
	 * @return results
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_print_allW(
			_Inout_ w_mem_pool pMemPool,
			_Inout_ wchar_t** pProcessLists);

	/**
	 * enumurate all processes names
	 * @param pMemPool The pool to allocate out of
	 * @param pProcessLists which will be updated
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_print_all(
			_Inout_ w_mem_pool pMemPool,
			_Inout_ char** pProcessLists);

	/**
	 * create a process
	 * @param pMemPool The pool to allocate out of
	 * @param pPathToProcess , the path to the process
	 * @param pCmdsArg , command args
	 * @param pCurrentDirectoryPath , the current directory path of process
	 * @param pWaitAfterRunningProcess , wait in seconds after running the process
	 * @param pCreationFlags , the creation flags
	 * @param pProcessInfo , the output process info
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_create(
			_Inout_ w_mem_pool pMemPool,
			_In_z_ const wchar_t* pPathToProcess,
			_In_z_ const wchar_t* pCmdsArg,
			_In_z_ const wchar_t* pCurrentDirectoryPath,
			_In_  DWORD pWaitAfterRunningProcess,
			_In_ DWORD pCreationFlags,
			_Inout_opt_ w_process_info* pProcessInfo);

	/**
	 * kill process by process info
	 * @param pProcessID process ID
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_kill_by_info(_In_ w_process_info pProcessInfo);

	/**
	 * kill process by name as administrator
	 * @param pProcessName process name
	 * @param pUserName administrator's user name
	 * @param pPassword administrator's password
	 * @param pTerminateChildProcesses terminate child processes
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_kill_by_name_as_admin(
			_In_z_ const wchar_t* pProcessName,
			_In_z_ const wchar_t* pUserName,
			_In_z_ const wchar_t* pPassword,
			_In_ bool pTerminateChildProcesses);

	/**
	 * kill process by name
	 * @param pProcessName process name
	 * @param pTerminateChildProcesses terminate child processes
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_kill_by_name(
			_In_z_ const wchar_t* pProcessName,
			_In_ bool pTerminateChildProcesses);

	/**
	 * kill process by id as an administrator
	 * @param pProcessID process id
	 * @param pTerminateChildProcesses terminate child processes
	 * @param pUserName administrator's user name
	 * @param pPassword administrator's password
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_kill_by_id_as_admin(
			_In_ DWORD pProcessID,
			_In_z_ const wchar_t* pUserName,
			_In_z_ const wchar_t* pPassword,
			_In_ bool pTerminateChildProcesses);

	/**
	 * kill process by id
	 * @param pProcessID process id
	 * @param pTerminateChildProcesses terminate child processes
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_kill_by_id(
			_In_ DWORD pProcessID,
			_In_ bool pTerminateChildProcesses);

	/**
	 * kill process by id with OpenProcess
	 * @param pProcessID process id
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_kill_by_id_handle(_In_ DWORD pProcessID);


	/**
	 * kill all processes by name
	 * @param pMemPool The pool to allocate out of
	 * @param ... list of processes which are going to kill (like L"p.exe", L"a.exe")
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_kill_all(
			_In_ w_mem_pool pMemPool,
			_In_ ...);

	/**
	 * release handle of process
	 * @param pProcessInfo , a pointer to process info
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_info_fini(_Inout_ w_process_info pProcessInfo);

#ifdef __cplusplus
}
#endif
