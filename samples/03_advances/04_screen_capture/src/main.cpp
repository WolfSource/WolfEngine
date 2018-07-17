/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to load 3d model in Wolf Engine.
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/
*/

#include "pch.h"
#include <scene.h>
#include <w_window.h>

using namespace std;
using namespace wolf;

static std::unique_ptr<w_window> sWindow;
static std::unique_ptr<scene> sScene;

static void release()
{
	UNIQUE_RELEASE(sWindow);
	UNIQUE_RELEASE(sScene);
	release_heap_data();
}

WOLF_MAIN()
{
#ifdef __WIN32
    auto _msg_proc_func = [](HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam) -> HRESULT
    {
		switch (pMsg)
		{
		case WM_SIZE:
		{
			if (sScene)
			{
				//check which window resized
				if (pHWND == sWindow->get_HWND())
				{
					//get new width and height
					auto _width = (int)LOWORD(pLParam);
					auto _height = (int)HIWORD(pLParam);

					sWindow->set_width(_width);
					sWindow->set_width(_height);

					w_point _new_size;
					_new_size.x = _width;
					_new_size.y = _height;

					sScene->on_window_resized(0, _new_size);
				}
			}
		}
		break;
		//close window on KeyUp event of Escape button
		case WM_KEYUP:
		{
			if (pWParam == VK_ESCAPE)
			{
				sWindow->close();
			}
		}
		break;
		case WM_QUIT:
		case WM_CLOSE:
		{
			sWindow->close();
		}
		break;
		}

		auto _result = inputs_manager.update(pHWND, pMsg, pWParam, pLParam);
		if (_result) return _result;

		return (HRESULT)DefWindowProc(pHWND, pMsg, pWParam, pLParam);
    };
#endif

	//Initialize and content path and logPath
	auto _running_dir = wolf::system::io::get_current_directoryW();
	std::wstring _content_path;
#if defined(__WIN32) || defined(__UWP)
	_content_path = _running_dir + L"../../../../content/";
#elif defined(__APPLE__)
	_content_path = _running_dir + L"/../../../../../content/";
#elif defined(__linux)
	error
#elif defined(__ANDROID)
	error
#endif

	const uint32_t _width = 1280, _height = 720;
	wolf::system::w_logger_config _log_config;
	_log_config.app_name = L"wolf.vulkan.sample";
	_log_config.log_path = wolf::system::io::get_current_directoryW();
#ifdef __WIN32
	_log_config.log_to_std_out = false;
#else
	_log_config.log_to_std_out = true;
#endif
	sScene = make_unique<scene>(_content_path, _log_config);
	sWindow = make_unique<w_window>();
	sWindow->set_width(_width);
	sWindow->set_height(_height);
	//sWindow->enable_caption(true);

#ifdef __WIN32
	//sWindow->enable_tiled(true);
    sWindow->initialize(_msg_proc_func);
#else 
    sWindow->initialize();
#endif
    //create window info and add it to the map
	w_present_info _window_info;
    _window_info.width = _width;
    _window_info.height = _height;
    _window_info.is_full_screen = false;
    _window_info.v_sync = false;
	_window_info.force_to_disable_v_sync = true;//force to disable vsync just for debugging
	_window_info.swap_chain_format = 44;//VK_FORMAT_B8G8R8A8_UNORM in vulkan api
	_window_info.cpu_access_swap_chain_buffer = true;
	
#ifdef __WIN32
    _window_info.hwnd = sWindow->get_HWND();
    _window_info.hInstance = sWindow->get_HINSTANCE();
#elif defined(__linux)
    _window_info.xcb_connection = sWindow->get_xcb_connection(),
    _window_info.xcb_window = sWindow->get_xcb_window(),
#endif

	//window info for first graphics device
    std::map<int, w_present_info> _windows_info = { {0, _window_info } };
    //run the main loop of window
    sWindow->run([&]()->void
    {
        sScene->run(_windows_info);
    });

    //release all
    _windows_info.clear();
	//output a message to the log file
	logger.write(L"Shutting down Wolf");
	release();

    //exit
    return EXIT_SUCCESS;
}
