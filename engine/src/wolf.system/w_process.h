/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_xml.h
	Description		 : XML serializer & parser using rapid xml "https://github.com/dwd/rapidxml"
	Comment          :
*/

#ifndef __W_PROCESS_H__
#define __W_PROCESS_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "w_system_export.h"
#include <Windows.h>
#include <tlhelp32.h>//for checking process
#include <psapi.h>//list all processes

namespace wolf
{
	namespace system
	{
		class w_process
		{
		public:
			static void print_process_name_ID(DWORD pProcessID)
			{
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
				//_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, pProcessID);

				// Release the handle to the process.
				CloseHandle(hProcess);
			}

			static void enum_all_processes()
			{
				DWORD aProcesses[1024], cbNeeded, cProcesses;
				unsigned int i;

				if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) return;

				// Calculate how many process identifiers were returned.
				cProcesses = cbNeeded / sizeof(DWORD);

				// Print the name and process identifier for each process.

				for (i = 0; i < cProcesses; i++)
				{
					if (aProcesses[i] != 0)
					{
						print_process_name_ID(aProcesses[i]);
					}
				}
			}

			static bool is_two_instance_of_process_running(const wchar_t* pProcessName)
			{
				bool _ye = false;
				BYTE _exists = 0;
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
							if (_exists > 1)
							{
								_ye = true;
								break;
							}
						}
					}
				}
				CloseHandle(_snapshot);

				return _ye;
			}
		};
	}
}

#endif //__W_PROCESS_H__