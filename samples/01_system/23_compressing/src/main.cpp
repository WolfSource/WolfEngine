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
#include <w_compress.h>

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
    logger.write(L"Wolf initialized");
    
  //  logger.write("Compressing using LZ4 version: {}", LZ4_versionNumber());
    
    
    w_timer _timer;
    _timer.start();
    
    //compress a file
    FILE* const _ifile = fopen("/Users/pooyaeimandar/Desktop/H.mp4", "rb");
    FILE* const _ofile = fopen("/Users/pooyaeimandar/Desktop/H_com.mp4", "wb");

    //compressResult_t const ret = compress_file(_ifile, _ofile);

    fclose(_ifile);
    fclose(_ofile);
    
    _timer.stop();
    
    logger.write("time is: {}", _timer.get_milliseconds() / 1000.000f);
    
    //release logger
    logger.release();

    return EXIT_SUCCESS;
}
