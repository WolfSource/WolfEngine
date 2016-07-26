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

#include <pdh.h>
#include "W_Object.h"
#include "w_system_dll.h"

#pragma comment(lib, "pdh.lib")

namespace wolf
{
	namespace system
	{
		class w_cpu : public system::w_object
		{
		public:
			SYS_EXP w_cpu();
			SYS_EXP virtual ~w_cpu();

			//Initialize the cpu info and return true or false value which indicates that function did initialize successfully or not. 
			SYS_EXP bool initialize();
			//Update the percentage of CPU usage
			SYS_EXP void update();
			//Get the value of CPU percentage
			SYS_EXP int get_cpu_percentage() const;
			//Release all resources
			SYS_EXP ULONG release() override;

		private:
			bool _canReadCpu;
			HQUERY _queryHandle;
			HCOUNTER _counterHandle;
			unsigned long _lastSampleTime;
			long _cpuUsage;

		};
	}
}

#endif

