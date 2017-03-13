#include "pch.h"
#include <w_window.h>
#include "scene.h"

using namespace std;

static std::unique_ptr<w_window> sWindow;
static std::unique_ptr<scene> sScene;

static void release()
{
    UNIQUE_RELEASE(sScene);
    UNIQUE_RELEASE(sWindow);
}

int main(int pArgc, char** pArgv) 
{
    sScene = make_unique<scene>();
    
    sWindow = make_unique<w_window>();
    sWindow->set_width(1920);
    sWindow->set_height(1080);
   
    //initialize window
    sWindow->initialize();
    
    w_window_info _w_info;
#ifdef __WIN32
    _w_info.hwnd = sWindow->get_HWND();
    _w_info.hInstance = sWindow->get_HINSTANCE();
#elif defined(__linux)
    _w_info.xcb_connection = sWindow->get_xcb_connection();
    _w_info.xcb_window = sWindow->get_xcb_window();
#endif
    _w_info.width = sWindow->get_width();
    _w_info.height = sWindow->get_height();
    
    std::map<int, std::vector<w_window_info>> _windows_info = { { 0, { _w_info } } }; 

    std::function<void(void)> _run_func = [&]()->void
    {
        sScene->run(_windows_info);
    };
    sWindow->run(_run_func);
    
    //release all
    release();    
    
    return EXIT_SUCCESS;
}

