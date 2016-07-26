#include "w_system_pch.h"
#include "w_cpu.h"

using namespace wolf::system;

w_cpu::w_cpu()
{
}

w_cpu::~w_cpu()
{
}

bool w_cpu::initialize()
{
	this->_canReadCpu = true;

	PDH_STATUS _status = PdhOpenQuery(NULL, 0, &this->_queryHandle);
	if (_status != ERROR_SUCCESS)
	{
		this->_canReadCpu = false;
		return false;
	}

	// Set query object to poll all cpus in the system.
	_status = PdhAddCounter(this->_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &this->_counterHandle);
	if (_status != ERROR_SUCCESS)
	{
		this->_canReadCpu = false;
		return false;
	}

	this->_lastSampleTime = GetTickCount();

	this->_cpuUsage = 0;

	return true;
}

void w_cpu::update()
{
	PDH_FMT_COUNTERVALUE _value;

	if (this->_canReadCpu)
	{
		if ((this->_lastSampleTime + 1000) < GetTickCount())
		{
			this->_lastSampleTime = GetTickCount();

			PdhCollectQueryData(this->_queryHandle);

			PdhGetFormattedCounterValue(this->_counterHandle, PDH_FMT_LONG, NULL, &_value);

			this->_cpuUsage = _value.longValue;
		}
	}
}

int w_cpu::get_cpu_percentage() const
{
	if (this->_canReadCpu) return (int)this->_cpuUsage;

	return 0;
}

ULONG w_cpu::release()
{
	if (this->is_released()) return 0;

	if (this->_canReadCpu)
	{
		PdhCloseQuery(this->_queryHandle);
	}

	return w_object::release();
}