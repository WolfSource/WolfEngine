#include "w_log.h"
#include <memory/w_string.h>
#include <chrono/w_timespan.h>
#include <io/w_io.h>
#include "zlog/zlog.h"

//Declaration of extern objects as shared
//#if defined(__WIN32) && !defined(__STATIC_LIB)
//#pragma data_seg (".shared")
//#endif

//wolf::system::w_logger wolf::logger;
//std::wstring wolf::content_path;
//wolf::system::w_inputs_manager wolf::inputs_manager;
//std::map<uint32_t, float> wolf::windows_frame_time_in_sec;
//
//void wolf::release_heap_data()
//{
//	//release all loggers
//	curl_global_cleanup();
//	spdlog::drop_all();
//    logger.release();
//	content_path.clear();
//    inputs_manager.reset();
//    windows_frame_time_in_sec.clear();
//}

//#if defined(__WIN32) && !defined(__STATIC_LIB)
//#pragma data_seg ()
//#pragma comment(linker,"/SECTION:.shared,RWS")
//#endif

static zlog_category_t* s_wolf_category;
static apr_file_t* s_log_file;
static char* s_log_write_buffer;
static bool s_log_to_console;

int wolf_zlog_output(_In_ zlog_msg_t* pMsg)
{
    //end character is \n, so we can ignore it
    if (!pMsg && pMsg->len < 2) return 1;
    
    pMsg->buf[pMsg->len - 1] = '\0';
    //prepare the log file
    apr_snprintf(s_log_write_buffer, MAX_BUFFER_SIZE, "[%s ][%s]\n", pMsg->path, pMsg->buf);
    //log to the console
    if (s_log_to_console)
    {
        printf("%s", s_log_write_buffer);
    }
    //store to file
    size_t _buf_len = strlen(s_log_write_buffer);
    return apr_file_write(s_log_file, s_log_write_buffer, &_buf_len);
}

W_RESULT w_log_init(_In_ const w_log_config* pConfig)
{
    if (!pConfig)
    {
        W_ASSERT(false, "pConfig in NULL. trace info: w_log_init");
        return W_FAILED;
    }

    s_log_to_console = pConfig->log_to_console;
    char* _log_directory =  w_string_concat(2, pConfig->log_path, "/Log/");
    if (w_io_get_is_directory(_log_directory) != W_PASSED)
    {
        if(w_io_create_directory(_log_directory) != W_PASSED)
        {
            W_ASSERT_P(false, "could not create directory %s. trace info: w_log_init", _log_directory);
            return W_FAILED;
        }
    }
    
    //create a config file from following data
    const char* _config_data = "[formats]\r\nsimple    = \"%m%n\"\r\n[rules]\r\nWolfEngine.*        $wolf_output, \" %c %d\";simple";
    char* _log_conf_path =  w_string_concat(2, _log_directory, "w_log_config.conf");
    //delete the conf file, if exists
    w_io_delete_file_with_path(_log_conf_path);
    
    //create another conf file
    if( w_io_save_to_file(_log_conf_path, // path
                         _config_data, // content of file
                         false, // binary mode
                         false, // buffered mode
                         false, // none block mode
                         false, // multithreaded mode
                         false, // open append
                         false, // is larg file
                         false // error if file exists
                         ) == W_FAILED)
    {
        W_ASSERT(false, "could not initialize log config file. trace info: w_log_init");
        return W_FAILED;
    }
    
    int _ret = zlog_init(_log_conf_path);
    //make sure delete config file
    w_io_delete_file_with_path(_log_conf_path);
    //check the return result of zlog_init
    if (_ret)
    {
        W_ASSERT(false, "could not initialize log file. trace info: w_log_init");
        return W_FAILED;
    }
    
    zlog_set_record("wolf_output", wolf_zlog_output);
    s_wolf_category = zlog_get_category("WolfEngine");
    if (!s_wolf_category)
    {
        W_ASSERT(false, "could not get wolf category from log file. trace info: w_log_init");
        zlog_fini();
        return W_FAILED;
    }
    
    //create log file
    const char* _date_time_str = w_timespan_get_current_date_time_string();
    char* _log_file_path =  w_string_concat(3, _log_directory, _date_time_str, ".wLog");
    s_log_file = w_io_create_file(_log_file_path,    // path
                                  "",                // content of file
                                  false,             // binary mode
                                  true,              // buffered mode
                                  true,              // none block mode
                                  true,              // multithreaded mode
                                  false,             // open append
                                  true,              // is larg file
                                  false              // error if file exists
                                  );
    if (!s_log_file)
    {
        W_ASSERT(false, "could not create log file. trace info: w_log_init");
        zlog_fini();
        return W_FAILED;
    }
    
    //create log buffer
    s_log_write_buffer = w_string_create(MAX_BUFFER_SIZE);
    if (!s_log_write_buffer)
    {
        W_ASSERT(false, "could not allocate write buffer for log file. trace info: w_log_init");
        zlog_fini();
        return W_FAILED;
    }
    
    return W_PASSED;
}

void w_log_write(_In_ const char* fmt, ...)
{
    va_list _arg_list;
    va_start(_arg_list, fmt);
    zlog_info(s_wolf_category, fmt, _arg_list);
    va_end(_arg_list);
}

void w_log_warning(_In_ const char* fmt, ...)
{
    va_list _arg_list;
    va_start(_arg_list, fmt);
    zlog_warn(s_wolf_category, fmt, _arg_list);
    va_end(_arg_list);
}

void w_log_error(_In_ const char* fmt, ...)
{
    va_list _arg_list;
    va_start(_arg_list, fmt);
    zlog_error(s_wolf_category, fmt, _arg_list);
    va_end(_arg_list);
}

void w_log_terminate()
{
    if (s_log_file)
    {
        apr_file_flush(s_log_file);
        apr_file_close(s_log_file);
    }
    if (s_wolf_category)
    {
        zlog_fini();
        s_wolf_category = NULL;
    }
}

