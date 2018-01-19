/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create two output windows with one graphic device in Wolf Engine.
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/
*/

#include "pch.h"
#include <scene.h>
#include <w_task.h>

using namespace std;
using namespace wolf;

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++
static std::unique_ptr<w_window> sWindow_0;
static std::unique_ptr<w_window> sWindow_1;
//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++

static std::unique_ptr<scene> sScene;

static void release()
{
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	UNIQUE_RELEASE(sWindow_0);
	UNIQUE_RELEASE(sWindow_1);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	UNIQUE_RELEASE(sScene);
	release_shared_data_over_all_instances();
}

//Entry point of program 
WOLF_MAIN()
{
#ifdef __WIN32
	auto _msg_proc_func = [](HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam) -> HRESULT
	{
		switch (pMsg)
		{
		case WM_CREATE:
		{
		}
		break;
		//close window on KeyUp event of Escape button
		case WM_KEYUP:
		{
			if (pWParam == VK_ESCAPE)
			{
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//The following codes have been added for this project
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				sWindow_0->close();
				sWindow_1->close();
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
			}
		}
		break;
		}

		auto _result = inputs_manager.update(pHWND, pMsg, pWParam, pLParam);
		if (_result) return _result;

		return (HRESULT)DefWindowProc(pHWND, pMsg, pWParam, pLParam);
	};
#endif

	//Initialize scene & window
	auto _running_dir = wolf::system::io::get_current_directory();
	sScene = make_unique<scene>(_running_dir, L"wolf.vulkan.sample");

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	const uint32_t _window_0_width = 800;
	const uint32_t _window_0_height = 600;
	const uint32_t _window_1_width = 640;
	const uint32_t _window_1_height = 480;

	sWindow_0 = make_unique<w_window>();
	sWindow_0->set_width(_window_0_width);
	sWindow_0->set_height(_window_0_height);

	sWindow_1 = make_unique<w_window>();
	sWindow_1->set_width(_window_1_width);
	sWindow_1->set_height(_window_1_height);

#pragma region set windows position

	//get information of monitors
	w_enumerate_screens _monitors_info;

	int _screen_0_left_offset = 0;
	int _screen_0_top_offset = 0;

	int _screen_1_left_offset = 0;
	int _screen_1_top_offset = 0;

	if (_monitors_info.screens.size() > 0)
	{
		_screen_0_left_offset = _monitors_info.screens[0].left;
		_screen_0_top_offset = _monitors_info.screens[0].top;
	}

	//if two monitors are avaiable, use offsets of second monitor
	if (_monitors_info.screens.size() > 1)
	{
		_screen_1_left_offset = _monitors_info.screens[1].left;
		_screen_1_top_offset = _monitors_info.screens[1].top;
	}
	else
	{
		//two monitors are not avaiable, use center of first monitor
		_screen_1_left_offset = (_monitors_info.screens[0].right - _monitors_info.screens[0].left ) / 2;
		_screen_1_top_offset = 0;
	}

	sWindow_0->set_position(_screen_0_left_offset, _screen_0_top_offset);
	sWindow_1->set_position(_screen_1_left_offset, _screen_1_top_offset);

#pragma endregion


#ifdef __WIN32
	sWindow_0->initialize(_msg_proc_func);
	sWindow_1->initialize(_msg_proc_func);
#else 
	sWindow_0->initialize();
	sWindow_1->initialize();
#endif
	
	//create windows info
	w_window_info _window_0_info;
	_window_0_info.width = _window_0_width;
	_window_0_info.height = _window_0_height;
	_window_0_info.is_full_screen = false;
	_window_0_info.v_sync_enable = false;
	_window_0_info.swap_chain_format = 44;//using vulkan as render api

	w_window_info _window_1_info;
	_window_1_info.width = _window_1_width;
	_window_1_info.height = _window_1_height;
	_window_1_info.is_full_screen = false;
	_window_1_info.v_sync_enable = false;
	_window_1_info.swap_chain_format = 44;//using vulkan as render api

#ifdef __WIN32
	_window_0_info.hwnd = sWindow_0->get_HWND();
	_window_1_info.hwnd = sWindow_1->get_HWND();
	_window_0_info.hInstance = sWindow_0->get_HINSTANCE();
	_window_1_info.hInstance = sWindow_1->get_HINSTANCE();
#elif defined(__linux)
	_window_0_info.xcb_connection = sWindow->get_xcb_connection(),
	_window_1_info.xcb_connection = sWindow->get_xcb_connection(),
	_window_0_info.xcb_window = sWindow->get_xcb_window(),
	_window_1_info.xcb_window = sWindow->get_xcb_window(),
#endif

	std::map<int, std::vector<w_window_info>> _windows_info = 
	{ 
		{ 0,{ _window_0_info } },
		{ 1,{ _window_1_info } }
	};
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	std::function<void(void)> _run_func = [&]()->void
	{
		sScene->run(_windows_info);
	};

	//run the main loop of first window
	sWindow_0->run(_run_func);

	//release all
	_windows_info.clear();
	//output a message to the log file
	logger.write(L"Shutting down Wolf");
	release();

	//exit
	return EXIT_SUCCESS;
}
