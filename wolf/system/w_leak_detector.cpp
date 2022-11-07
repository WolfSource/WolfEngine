#include "w_leak_detector.hpp"
#include <iostream>

using w_leak_detector = wolf::system::w_leak_detector;

w_leak_detector::w_leak_detector()
{
#ifdef WIN32
	//take a snapshot from memory
	_CrtMemCheckpoint(&this->_mem_state); 
#endif
}

w_leak_detector::~w_leak_detector()
{
	try
	{
#ifdef WIN32
		//take a snapshot from memory
		_CrtMemState _diff_mem;
		_CrtMemState _new_mem_state;
		_CrtMemCheckpoint(&_new_mem_state);

		if (_CrtMemDifference(&_diff_mem, &this->_mem_state, &_new_mem_state) == TRUE)
		{
			_CrtMemDumpStatistics(&_diff_mem);
			_CrtMemDumpAllObjectsSince(&this->_mem_state);
			_CrtDumpMemoryLeaks();
			// TODO: we should write to the log file and the log file will send to open telemetry or any preset configs
			std::cerr << "memory leak detected!\r\n";
			// "~w_leak_detector thrown an exception:" << p_ex.what();
			std::terminate();
		}
#endif
	}
	catch (const std::exception& p_ex)
	{
		// write to the log file
		// "~w_leak_detector thrown an exception:" << p_ex.what();
	}
}
