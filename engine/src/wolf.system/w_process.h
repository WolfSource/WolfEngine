/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_xml.h
	Description		 : XML serializer & parser using rapid xml "https://github.com/dwd/rapidxml"
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_PROCESS_H__
#define __W_PROCESS_H__

#include "w_system_export.h"
#include "w_std.h"

namespace wolf
{
	namespace system
	{
		struct w_process_info
		{
#ifdef __WIN32
			PROCESS_INFORMATION info;
#endif
		};

		class w_process
		{
		public:
			//kill process by proces ID
			WSYS_EXP static W_RESULT kill_process_by_ID(_In_ const unsigned long& pProcessID);
			//print process name based on proces ID
			WSYS_EXP static std::wstring get_process_name_by_ID(_In_ const unsigned long& pProcessID);
			//enumurate all processes
			WSYS_EXP static const std::wstring enum_all_processes();
			//check whether two instances of same process is running
			WSYS_EXP static bool check_for_number_of_running_instances_from_process(_In_z_ const wchar_t* pProcessName,
				_In_ size_t pNumberOfRunningInstnacesToBeChecked = 1);
			//create a process
			WSYS_EXP static w_process_info* create_process(
				_In_z_ const wchar_t* pPathtoProcess, 
				_In_z_ const wchar_t* pCmdsArg,
				_In_z_ const wchar_t* pCurrentDirectoryPath,
				_In_  const long long pWaitAfterRunningProcess = 0);
			//kill a process
			WSYS_EXP static bool kill_process(_In_ w_process_info* pProcessInfo);
			//kill all process by name
			WSYS_EXP static bool kill_all_processes(_In_z_ std::initializer_list<const wchar_t*> pProcessNames);
		};
	}
}

#endif //__W_PROCESS_H__