#include "pch.h"

enum DLL_EXIT_STATE { RUNNING, EXITING, EXITED};
static tbb::atomic<DLL_EXIT_STATE> sDLLExitState;

#if defined(__WIN32) || defined (__UWP)

BOOL APIENTRY DllMain(_In_ HMODULE pHModule, _In_ DWORD pULReasonForCall, _In_ LPVOID pLPReserved)
{
	switch (pULReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		sDLLExitState = RUNNING;
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		sDLLExitState = EXITED;
		break;
	}
	return TRUE;
}

#endif


//static scene* sScene = nullptr;
//static w_window_info sWindowInfo;
//static std::wstring _app_name;
//static std::wstring _running_dir;
//static HWND sHWND;
//static HINSTANCE sHINSTANCE;
//

//static tbb::interface5::condition_variable sExitingCV;
//static tbb::mutex sExitingMutex;

//static int initialize(unsigned int pIntHWND, const char* pAppName, const char* pLogPath, const char* pContentPath)
//{
//	//sExiting = DLL_EXIT_STATE::RUNNING;
// //   _app_name = wolf::system::convert::string_to_wstring(pAppName);
// //   _running_dir = wolf::system::convert::string_to_wstring(pLogPath);
// //   wolf::content_path = wolf::system::convert::string_to_wstring(pContentPath);
// // 
// //   sHWND = (HWND)pIntHWND;
// //   sHINSTANCE = (HINSTANCE)GetModuleHandle(NULL);
//    
//  //  wolf::system::w_task::execute_async([&]()
//  //  {
//		//if (sScene)
//		//{
//		//	SAFE_RELEASE(sScene);
//		//}
//  //      sScene = new scene(_running_dir, _app_name);
//  //      
//  //      //create window info and add it to the map
//  //      sWindowInfo.width = 640;
//  //      sWindowInfo.height = 480;
//  //      sWindowInfo.is_full_screen = false;
//  //      sWindowInfo.v_sync_enable = false;
//  //      sWindowInfo.swap_chain_format = 44;//using vulkan as render api
//  //      sWindowInfo.hwnd = sHWND;
//  //      sWindowInfo.hInstance = sHINSTANCE;
//
//  //      std::map<int, std::vector<w_window_info>> _windows_info = { { 0,{ sWindowInfo } } };
//  //      while (true)
//  //      {
//		//	if (sExiting == DLL_EXIT_STATE::EXITING)
//		//	{
//		//		SAFE_RELEASE(sScene);
//		//		break;
//		//	}
//		//	sScene->run(_windows_info);
//  //      }
//  //      _windows_info.clear();
//		//
//		////notify for 
//		//{
//		//	tbb::interface5::unique_lock<tbb::mutex> _lock(sExitingMutex);
//		//	sExitingCV.notify_all();
//		//}
//  //  });
//
//    return 0;
//}
//
//static int load_scene(const char* pScenePath)
//{
//    auto _wstr = wolf::system::convert::string_to_wstring(pScenePath);
//    return sScene && sScene->load_scene(_wstr) == S_OK ? 0 : 1;
//}
//
//static int set_camera_position(float X, float Y, float Z)
//{
//	if (!sScene) return 1;
//	sScene->set_camera_position(X, Y, Z);
//	return 0;
//}
//
//static int set_camera_lookat(float X, float Y, float Z)
//{
//	if (!sScene) return 1;
//	sScene->set_camera_lookat(X, Y, Z);
//	return 0;
//}
//
//static int remove_all_models()
//{
//	return sScene && sScene->remove_all_models() == S_OK ? 0 : 1;
//}
//
//static void release_shared_data_over_all_instances()
//{
//    wolf::release_shared_data_over_all_instances();
//}

//static void release()
//{
//	if (sExiting == DLL_EXIT_STATE::EXITED) return;
//
//	sExiting = DLL_EXIT_STATE::EXITING;
//	{
//		tbb::interface5::unique_lock<tbb::mutex> _lock(sExitingMutex);
//		sExitingCV.wait(_lock);
//	}
//	sExiting = DLL_EXIT_STATE::EXITED;
//}

using namespace boost::python;
using namespace wolf::system;

#include <python_exporter/py_logger.h>

BOOST_PYTHON_MODULE(pyWolf)
{
    //export wolf::system classes to pyWolf.system scope
    {
        struct system {};
        scope _system = class_<system>("system");
        
        pywolf::py_bounding_export();
        pywolf::py_color_export();
        pywolf::py_game_time_export();
        pywolf::py_timer_export();
        pywolf::py_time_span_export();
        pywolf::py_inputs_manager_export();
        pywolf::py_point_export();
        pywolf::py_rectangle_export();
        pywolf::py_window_export();
    }

    //export wolf::content_pipeline classes to pyWolf.content_pipeline scope
    {


    }

    //export wolf::media_core classes to pyWolf.media_core scope
    {


    }

    //export wolf::graphics classes to pyWolf.graphics scope
    {
        struct graphics {};
        scope _graphics = class_<graphics>("graphics");

        pywolf::py_graphics_device_manager_export();  
		pywolf::py_fences_export();
		pywolf::py_semaphore_export();
		pywolf::py_texture_export();
        pywolf::py_command_buffer_export();
    }

    //export wolf::framework classes to pyWolf.framework scope
    {
        struct framework {};
        scope _framework = class_<framework>("framework");

        pywolf::py_game_export();

    }

    //global scope
    def("bounding_box_from_bounding_sphere", w_bounding_box::create_from_bounding_sphere);
    def("bounding_sphere_from_bounding_box", w_bounding_sphere::create_from_bounding_box);
    //export logger in global scope
    pywolf::py_logger_export();

    def("release_shared_data_over_all_instances", wolf::release_shared_data_over_all_instances);
}
