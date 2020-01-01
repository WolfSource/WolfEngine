#include "w_log.h"

////Declaration of extern objects as shared
//#if defined(__WIN32) && !defined(__STATIC_LIB)
//#pragma data_seg (".shared")
//#endif
//
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
//
//#if defined(__WIN32) && !defined(__STATIC_LIB)
//#pragma data_seg ()
//#pragma comment(linker,"/SECTION:.shared,RWS")
//#endif
