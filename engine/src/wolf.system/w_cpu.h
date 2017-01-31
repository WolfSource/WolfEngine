/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_cpu.h
	Description		 : This class responsible to show the information of CPU
	Comment          :
*/

#ifndef __W_CPU_H__
#define __W_CPU_H__

#ifdef __WIN32

#if _MSC_VER > 1000
#pragma once
#endif

#include <pdh.h>
#include "w_object.h"
#include "w_system_export.h"

#pragma comment(lib, "pdh.lib")

namespace wolf
{
	namespace system
	{
		class w_cpu : public system::w_object
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
			WSYS_EXP ULONG release() override;

		private:
			bool _canReadCpu;
			HQUERY _queryHandle;
			HCOUNTER _counterHandle;
			unsigned long _lastSampleTime;
			long _cpuUsage;

		};
	}
}

#endif // __WIN32

#endif //  __W_CPU_H__