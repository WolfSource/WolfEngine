/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_cpu.h
	Description		 : This class responsible to show the information of CPU
	Comment          :
*/

#pragma once

#ifdef __WIN32

#include <pdh.h>
#include "w_system_export.h"

#pragma comment(lib, "pdh.lib")

namespace wolf::system
{
	class w_cpu
	{
	public:
		WSYS_EXP w_cpu();
		WSYS_EXP virtual ~w_cpu();

		//Initialize the cpu info and return true or false value which indicates that function did initialize successfully or not. 
		WSYS_EXP bool initialize();
		//Update the percentage of CPU usage
		WSYS_EXP void update();
		//Get the value of CPU percentage
		WSYS_EXP int get_cpu_percentage() const;
		//Release all resources
		WSYS_EXP ULONG release();

	private:
		bool _is_released;
		bool _canReadCpu;
		HQUERY _queryHandle;
		HCOUNTER _counterHandle;
		unsigned long _lastSampleTime;
		long _cpuUsage;

	};
}

#endif // __WIN32
