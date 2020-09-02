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

	typedef struct w_process_info_t
	{
#ifdef W_PLATFORM_WIN
		PROCESS_INFORMATION*	info;
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
	 * get a process name based on process id
	 * @param pMemPool The pool to allocate out of
	 * @param pProcessID process ID
	 * @param pProcessName the process name
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_get_name_by_id(
			_Inout_ w_mem_pool pMemPool,
			_In_ unsigned long pProcessID,
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
			_Out_ w_process_info* pProcessInfo);

	/**
	 * kill process by process ID
	 * @param pProcessID process ID
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_kill_by_id(_In_ unsigned long pProcessID);

	/**
	 * kill process by process info
	 * @param pProcessID process ID
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_kill_by_info(_In_ w_process_info pProcessInfo);

	/**
	 * release resources of process info
	 * @param pProcessInfo , a pointer to process info
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_process_info_fini(
			_Inout_ w_mem_pool pMemPool,
			_Inout_ w_process_info pProcessInfo);

#ifdef __cplusplus
}
#endif