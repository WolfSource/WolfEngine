#include "w_system_pch.h"
#include "wolf.h"

//Declaration of extern shared objects

#ifdef __WIN32
#pragma data_seg (".shared")
#endif

wolf::system::w_logger logger;
std::wstring content_path;
wolf::system::w_inputs_manager inputs_manager;
std::map<uint32_t, float> windows_frame_time_in_sec;

#ifdef __WIN32
#pragma data_seg ()
#pragma comment(linker,"/SECTION:.shared,RWS")
#endif