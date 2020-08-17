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

	typedef struct
	{
#ifdef W_PLATFORM_WIN
		PROCESS_INFORMATION		info;
		HWND					handle;
		wchar_t*				class_name;
		wchar_t*				title_name;
		wchar_t*				process_name;
		DWORD					error_code;
#endif
	} w_process_info;

	/**
	 * release resources of process info
	 * @param pProcessInfo , a pointer to process info
	 * @return result
	*/
	W_RESULT w_process_info_terminate(_Inout_ w_process_info* pProcessInfo);

	/**
	 * kill process by proces ID
	 * @param pProcessID process ID
	 * @return result
	*/
	W_RESULT w_process_kill_by_id(_In_ unsigned long pProcessID);

	/**
	 * get a process name based on process id
	 * @param pProcessID process ID
	 * @return result
	*/
	const wchar_t* w_process_get_name_by_id(_In_ unsigned long pProcessID);

	/**
	 * enumurate all processes 
	 * @return name of all processes
	*/
	//const wchar_t* w_process_enum_all_processes();
		
	////check whether two instances of same process is running
	//WSYS_EXP static bool check_for_number_of_running_instances_from_process(_In_z_ const wchar_t* pProcessName,
	//	_In_ size_t pNumberOfRunningInstnacesToBeChecked = 1);
	////get number of running instances the process
	//WSYS_EXP static size_t get_number_of_running_instances_from_process(_In_z_ const wchar_t* pProcessName);
	////create a process
	//WSYS_EXP static w_process_info* create_process(
	//	_In_z_ const wchar_t* pPathtoProcess,
	//	_In_z_ const wchar_t* pCmdsArg,
	//	_In_z_ const wchar_t* pCurrentDirectoryPath,
	//	_In_  const long long pWaitAfterRunningProcess = 0,
	//	_In_ DWORD pCreationFlags = 0);
	////kill a process
	//WSYS_EXP static bool kill_process(_In_ w_process_info* pProcessInfo);
	////kill all process by name
	//WSYS_EXP static bool kill_all_processes(_In_z_ std::initializer_list<const wchar_t*> pProcessNames);

#ifdef __cplusplus
}
#endif