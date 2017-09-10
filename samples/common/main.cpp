/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create a scene in Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/
*/

#include "pch.h"
#include <scene.h>
#include <w_window.h>

using namespace std;

static std::unique_ptr<w_window> sWindow;
static std::unique_ptr<scene> sScene;

static void release()
{
	UNIQUE_RELEASE(sWindow);
	UNIQUE_RELEASE(sScene);
}

//Entry point of program 
#ifdef __WIN32
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
#elif defined(__linux) || defined(__APPLE__) 
int main(int pArgc, const char * pArgv[])
#endif
{

#ifdef __WIN32
    auto _msg_proc_func = [](HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam) -> HRESULT
    {
        switch (pMsg)
        {
        case WM_CREATE:
        {
            logger.write(L"The window just created");
        }
        break;
        //close window on KeyUp event of Escape button
        case WM_KEYUP:
        {
            if (pWParam == VK_ESCAPE)
            {
                sWindow->close();
                logger.write(L"The windows just closed");
            }
        }
        break;
        }

        return sScene->on_msg_proc(pHWND, pMsg, pWParam, pLParam);
    };
#endif

    //Initialize scene & window
    sScene = make_unique<scene>();
    sWindow = make_unique<w_window>();
#ifdef __WIN32
    sWindow->initialize(_msg_proc_func);
#else 
    sWindow->initialize();
#endif
    //create window info and add it to the map
    w_window_info _window_info;
    _window_info.width = 800;
    _window_info.height = 800;
    _window_info.is_full_screen = false;
    _window_info.v_sync_enable = false;
    _window_info.swap_chain_format = 44;//using vulkan as render api

#ifdef __WIN32
    _window_info.hwnd = sWindow->get_HWND();
    _window_info.hInstance = sWindow->get_HINSTANCE();
#elif defined(__linux)
    _window_info.xcb_connection = sWindow->get_xcb_connection(),
    _window_info.xcb_window = sWindow->get_xcb_window(),
#endif

    std::map<int, std::vector<w_window_info>> _windows_info = { {0, {_window_info} } };
    std::function<void(void)> _run_func = [&]()->void
    {
        sScene->run(_windows_info);
    };

    //run the main loop of window
    sWindow->run(_run_func);

    //release all
    _windows_info.clear();
    release();

    //output a message to the log file
    logger.write(L"Shutting down Wolf");

    //exit
    return EXIT_SUCCESS;
}
