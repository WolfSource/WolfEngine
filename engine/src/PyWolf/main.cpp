#include "w_python_pch.h"

using namespace boost::python;

static std::unique_ptr<PyScene> sScene;
static w_window_info sWindowInfo;
static std::atomic<bool> sInitialized = false;

static int initialize(PyObject* pHWND, const char* pAppName, const char* pLogPath, const char* pContentPath)
{
    auto _app_name = wolf::system::convert::string_to_wstring(pAppName);
    auto _running_dir = wolf::system::convert::string_to_wstring(pLogPath);

    content_path = wolf::system::convert::string_to_wstring(pContentPath);
    sScene = std::make_unique<PyScene>(_running_dir, _app_name);

    HWND _hwnd = (HWND)pHWND;
    HINSTANCE _hInstance = (HINSTANCE)GetModuleHandle(NULL);
    
    //create window info and add it to the map
    sWindowInfo.width = 800;
    sWindowInfo.height = 600;
    sWindowInfo.is_full_screen = false;
    sWindowInfo.v_sync_enable = true;//Must be true for PySide
    sWindowInfo.swap_chain_format = 44;//using vulkan as render api
    sWindowInfo.hwnd = _hwnd;
    sWindowInfo.hInstance = _hInstance;

    sInitialized = true;
    return 0;
}

static void render()
{
    if (!sInitialized) return;

    std::map<int, std::vector<w_window_info>> _windows_info = { { 0,{ sWindowInfo } } };
    sScene->run(_windows_info);
    _windows_info.clear();
}

static void release()
{
    UNIQUE_RELEASE(sScene);
}

BOOST_PYTHON_MODULE(PyWolf)
{
    def("initialize", initialize);
    def("render", render);
    def("release", release);
}
