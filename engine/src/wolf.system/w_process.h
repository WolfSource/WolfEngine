/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_xml.h
	Description		 : XML serializer & parser using rapid xml "https://github.com/dwd/rapidxml"
	Comment          :
*/

#pragma once

#include "w_system_export.h"
#include "w_std.h"

namespace wolf::system
{
	struct w_process_info
	{
#ifdef __WIN32
		PROCESS_INFORMATION   info;
		HWND				  handle;
		std::wstring          class_name;
		std::wstring          title_name;
		std::wstring          process_name;
#endif
		std::error_code		  error_code;
	};

	class w_process
	{
	public:
		//kill process by proces ID
		WSYS_EXP static W_RESULT kill_process_by_ID(_In_ const unsigned long& pProcessID);
		//print process name based on proces ID
		WSYS_EXP static std::wstring get_process_name_by_ID(_In_ const unsigned long& pProcessID);
		
		//enumurate all processes and store in to the string
		WSYS_EXP static const std::wstring enum_all_processes();

		//enumurate all processes and store in to the vector
		WSYS_EXP static void enum_all_processes(
			_Inout_ std::vector<w_process_info*>& pProcessHandles);

		//check whether two instances of same process is running
		WSYS_EXP static bool check_for_number_of_running_instances_from_process(_In_z_ const wchar_t* pProcessName,
			_In_ size_t pNumberOfRunningInstnacesToBeChecked = 1);
		//get number of running instances the process
		WSYS_EXP static size_t get_number_of_running_instances_from_process(_In_z_ const wchar_t* pProcessName);
		//create a process
		WSYS_EXP static w_process_info* create_process(
			_In_z_ const wchar_t* pPathtoProcess,
			_In_z_ const wchar_t* pCmdsArg,
			_In_z_ const wchar_t* pCurrentDirectoryPath,
			_In_  const long long pWaitAfterRunningProcess = 0,
			_In_ DWORD pCreationFlags = 0);

		//kill a process
		WSYS_EXP static bool kill_process(_In_ w_process_info* pProcessInfo);
		
		//force to kill process
		WSYS_EXP static bool force_kill_process_by_name(
			_In_z_ const std::wstring pProcessName,
			_In_ const bool pTerminateChildProcesses);
		
		//force to kill process by name as an administrator
		WSYS_EXP static bool force_kill_process_by_name_as_admin(
			_In_z_ const std::wstring pProcessName,
			_In_z_ const std::wstring pUserNameName,
			_In_z_ const std::wstring pPassword,
			_In_ const bool pTerminateChildProcesses);

		//force to kill process
		WSYS_EXP static bool force_kill_process(
			_In_ const DWORD pProcessID,
			_In_ const bool pTerminateChildProcesses);

		//force to kill process by name as an administrator
		WSYS_EXP static bool force_kill_process_by_name_as_admin(
			_In_ const DWORD pProcessID,
			_In_z_ const std::wstring pUserNameName,
			_In_z_ const std::wstring pPassword,
			_In_ const bool pTerminateChildProcesses);

		//kill all process by name
		WSYS_EXP static bool kill_all_processes(_In_ std::initializer_list<const wchar_t*> pProcessNames);
	};
}