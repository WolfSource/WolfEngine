/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to use fiber
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"
#include <w_timer.h>
#include <w_compress.hpp>

//namespaces
using namespace wolf;
using namespace wolf::system;

WOLF_MAIN()
{
    w_logger_config _log_config;
    _log_config.app_name = L"23_compressing";
    _log_config.log_path = wolf::system::io::get_current_directoryW();
#ifdef __WIN32
    _log_config.log_to_std_out = false;
#else
    _log_config.log_to_std_out = true;
#endif
    //initialize logger, and log in to the output debug window of visual studio(just for windows) and Log folder inside running directory
    logger.initialize(_log_config);
    
    //log to output file
    logger.write("Wolf initialized");
    
	const char* _src = "When I was young, there was an amazing publication called The Whole Earth Catalog, " \
		"which was one of the bibles of my generation...It was sort of like Google in paperback form, " \
		"35 years before Google came along: it was idealistic, and overflowing with neat tools and " \
		"great notions...(publisher) Stewart (Brand) and his team put out several issues of The Whole Earth Catalog, " \
		"and then when it had run its course, they put out a final issue. It was the mid-1970s, and I was your age. " \
		"On the back cover of their final issue was a photograph of an early morning country road, the kind you might find yourself " \
		"hitchhiking on if you were so adventurous. Beneath it were the words: \"Stay Hungry.Stay Foolish.\" " \
		"It was their farewell message as they signed off...And I have always wished that for myself. " \
		"And now, as you graduate to begin a new, I wish that for you. " \
		"Stay Hungry. Stay Foolish.";

#pragma region compress
	w_compress_result _compress_result;
	_compress_result.size_in = strlen(_src) + 1; //1 for '\0
    
	w_timer _timer;

	_timer.start();
	if (w_compress::compress_buffer(_src, &_compress_result) != W_PASSED)
	{
		logger.error("could not compress buffer");
		return EXIT_FAILURE;
	}
	_timer.stop();

	logger.write("compress time: {}ms", _timer.get_milliseconds());
#pragma endregion

#pragma region decompress
	w_compress_result _decompress_result;
	_decompress_result.size_in = _compress_result.size_out;

	_timer.start();
	if (w_compress::decompress_buffer(_compress_result.data, &_decompress_result) != W_PASSED)
	{
		logger.error("could not decompress buffer");
		return EXIT_FAILURE;
	}
	_timer.stop();

	logger.write("decompress time: {}ms", _timer.get_milliseconds());
#pragma endregion
    
    //release logger
    logger.release();

    return EXIT_SUCCESS;
}
