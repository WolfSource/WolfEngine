/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to manage scalable memory in Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace wolf;
using namespace wolf::system;

//Entry point of program 
WOLF_MAIN()
{
    w_logger_config _log_config;
    _log_config.app_name = L"09_memory";
    _log_config.log_path = wolf::system::io::get_current_directoryW();
#ifdef __WIN32
    _log_config.log_to_std_out = false;
#else
    _log_config.log_to_std_out = true;
#endif
    logger.initialize(_log_config);

    size_t _length = 10;

    w_memory_pool _memory;
    _memory.alloc(_length * sizeof(float));

    //get pointer to start of memory
    auto _f = (float*)_memory.get_start_ptr();
    //assign new data to memory
    for (size_t i = 0; i < _length; ++i)
    {
        _f[i] = i;
    }
    //output them
    for (size_t i = 0; i < _length; ++i)
    {
        logger.write("{}", _f[i]);
    }

    //re-allocate larger memory with new data type
    struct my_struct
    {
        int         number;
        char		name[256];
    };
    _length = 20;

	auto _new_f = static_cast<my_struct*>(_memory.re_alloc(_length * sizeof(my_struct)));
	if (_new_f)
	{
		for (int i = 0; i < _length; ++i)
		{
			_new_f[i].number = 100 + i;
			w_sprintf(_new_f[i].name, 256, "Ryan %d", i);
		}
	}

    //output new data
    for (size_t i = 0; i < _length; ++i)
    {
        logger.write("{}", _new_f[i].number);
        logger.write(_new_f[i].name);
    }
    
    logger.release();

	return EXIT_SUCCESS;
}
