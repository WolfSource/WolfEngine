#include "w_python_pch.h"

using namespace boost::python;

static PyScene* sScene = nullptr;
static w_window_info sWindowInfo;
static std::wstring _app_name;
static std::wstring _running_dir;
static HWND sHWND;
static HINSTANCE sHINSTANCE;

enum DLL_EXIT_STATE { RUNNING, EXITING, EXITED};
static tbb::atomic<DLL_EXIT_STATE> sExiting;
static tbb::interface5::condition_variable sExitingCV;
static tbb::mutex sExitingMutex;

static int initialize(unsigned int pIntHWND, const char* pAppName, const char* pLogPath, const char* pContentPath)
{
	sExiting = DLL_EXIT_STATE::RUNNING;
    _app_name = wolf::system::convert::string_to_wstring(pAppName);
    _running_dir = wolf::system::convert::string_to_wstring(pLogPath);
    content_path = wolf::system::convert::string_to_wstring(pContentPath);
  
    sHWND = (HWND)pIntHWND;
    sHINSTANCE = (HINSTANCE)GetModuleHandle(NULL);
    
    wolf::system::w_task::execute_async([&]()
    {
		if (sScene)
		{
			SAFE_RELEASE(sScene);
		}
        sScene = new PyScene(_running_dir, _app_name);
        
        //create window info and add it to the map
        sWindowInfo.width = 800;
        sWindowInfo.height = 600;
        sWindowInfo.is_full_screen = false;
        sWindowInfo.v_sync_enable = false;
        sWindowInfo.swap_chain_format = 44;//using vulkan as render api
        sWindowInfo.hwnd = sHWND;
        sWindowInfo.hInstance = sHINSTANCE;

        std::map<int, std::vector<w_window_info>> _windows_info = { { 0,{ sWindowInfo } } };
        while (true)
        {
			if (sExiting == DLL_EXIT_STATE::EXITING)
			{
				SAFE_RELEASE(sScene);
				break;
			}
			sScene->run(_windows_info);
        }
        _windows_info.clear();
		
		//notify for 
		{
			tbb::interface5::unique_lock<tbb::mutex> _lock(sExitingMutex);
			sExitingCV.notify_all();
		}
    });

    return 0;
}

static void logg()
{
    logger.write("Hiiii");
}

static void release()
{
	if (sExiting == DLL_EXIT_STATE::EXITED) return;

	sExiting = DLL_EXIT_STATE::EXITING;
	{
		tbb::interface5::unique_lock<tbb::mutex> _lock(sExitingMutex);
		sExitingCV.wait(_lock);
	}
	sExiting = DLL_EXIT_STATE::EXITED;
}

BOOST_PYTHON_MODULE(PyWolf)
{
    def("initialize", initialize);
    def("logg", logg);
    def("release", release);
}
