///*
//    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
//    https://github.com/WolfEngine/WolfEngine
//*/
//
//#pragma once
//
//#ifdef WOLF_SYSTEM_LOG
//
//#include <wolf.hpp>
//#include <filesystem>
//
//namespace wolf::system::log
//{
//    enum class w_log_level
//    {
//        TRACE = 0,
//        DEBUG_ = 1,
//        INFO = 2,
//        WARN = 3,
//        ERROR_ = 4,
//        CRITICAL = 5,
//        OFF = 6
//    };
//
//    // create a log with a combination of console, telemetry, visual studio and (async file/async daily file)
//    // please note that you can't select both async file and async daily file
//    enum w_log_sink
//    {
//        CONSOLE = (1u << 0),
//        ASYNC_FILE = (1u << 1),
//        ASYNC_DAILY_FILE = (1u << 2),
//        //TELEMETRY = (1u << 3),
//#ifdef _MSC_VER
//        VISUAL_STUDIO = (1u << 4),
//#endif
//    };
//
//    struct w_log_config
//    {
//        // create an async logger
//        bool async = false;
//        // enable multi-threaded
//        bool multi_threaded = false;
//        // the path of log file
//        std::filesystem::path path;
//        // the log level
//        w_log_level level;
//        // the flush level
//        w_log_level flush_level;
//        // the sinks of log
//        w_log_sink type;
//        // max file size in Mb (e.g. 100 * 1048576 meaning maximum 100 Mb size for each log file)
//        size_t max_file_size_in_mb = 100 * 1048576;
//        // max number of files
//        size_t max_files = 10;
//        // rotate file on open
//        bool rotate_on_open = false;
//        // start creation time for daily log
//        int hour = 0;
//        // start creation time for daily log
//        int minute = 0;
//    };
//
//} // namespace wolf::system::log
//
//#endif