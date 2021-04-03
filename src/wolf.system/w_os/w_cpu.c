//#include "w_cpu.h"
//
//#ifdef W_PLATFORM_WIN
//#include <pdh.h>
//#endif
//
//typedef struct
//{
//#ifdef W_PLATFORM_WIN
//
//    HQUERY          query_handle;
//    HCOUNTER        counter_handle;
//    
//#else
//
//    float           last_total_idle_time;
//    float           last_total_time;
//    float           last_total_active_time;
//    
//#endif
//    
//} w_cpu_pv;
//
//
//w_cpu w_cpu_init(void)
//{
//    //w_cpu_imp _cpu = {0};
//    
//#ifdef W_PLATFORM_WIN
//    
//    PDH_STATUS _rt = PdhOpenQuery(NULL, 0, &_queryHandle);
//	if (_rt != ERROR_SUCCESS)
//	{
//        _cpu->error_code = W_FAILURE;
//		return false;
//	}
////	// Set query object to poll all cpus in the system.
////	_status = PdhAddCounter(this->_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &this->_counterHandle);
////	if (_status != ERROR_SUCCESS)
////	{
////		this->_canReadCpu = false;
////		return false;
////	}
////
////	this->_lastSampleTime = GetTickCount();
////
////	this->_cpuUsage = 0;
//#else
//    
//#endif
//    
//    return 0;
//}
////
////void w_cpu::update()
////{
////	PDH_FMT_COUNTERVALUE _value;
////
////	if (this->_canReadCpu)
////	{
////		if ((this->_lastSampleTime + 1000) < GetTickCount())
////		{
////			this->_lastSampleTime = GetTickCount();
////
////			PdhCollectQueryData(this->_queryHandle);
////
////			PdhGetFormattedCounterValue(this->_counterHandle, PDH_FMT_LONG, NULL, &_value);
////
////			this->_cpuUsage = _value.longValue;
////		}
////	}
////}
////
////int w_cpu::get_cpu_percentage() const
////{
////	if (this->_canReadCpu) return (int)this->_cpuUsage;
////
////	return 0;
////}
////
////ULONG w_cpu::release()
////{
////	if (this->_is_released) return 1;
////
////	if (this->_canReadCpu)
////	{
////		PdhCloseQuery(this->_queryHandle);
////	}
////
////	this->_is_released = true;
////	return 0;
////}
