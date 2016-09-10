/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create multiple scenes and render them to multiple windows in Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"
#include "scene_0.h"
#include "scene_1.h"

using namespace std;

//static windows declaration
static std::unique_ptr<w_window> sWindow_0;
static std::unique_ptr<w_window> sWindow_1;

//static scene declaration
static std::unique_ptr<scene_0> sScene_0;
static std::unique_ptr<scene_1> sScene_1;

//static windows information declaration
static std::map<int, std::vector<w_window_info>> sWindowsInfo_0;
static std::map<int, std::vector<w_window_info>> sWindowsInfo_1;

static void release()
{
	//release windows
	UNIQUE_RELEASE(sWindow_0);
	UNIQUE_RELEASE(sWindow_1);

	//release scenes
	UNIQUE_RELEASE(sScene_0);
	UNIQUE_RELEASE(sScene_1);
}

void loop_window_0()
{
	//run the first scene
	sScene_0->run(sWindowsInfo_0);
}

void loop_window_1()
{
	//run the second scene
	sScene_1->run(sWindowsInfo_1);
}

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	//create the scenes
	sScene_0 = make_unique<scene_0>();
	sScene_1 = make_unique<scene_1>();

	//get information of monitor(s)
	w_enumerate_monitors _monitors_info;

	/*
		set message procedure function fo windows
		we use it for handling messages from window, such as keyboard status, on createing window event or etc.
	*/
	w_window::msg_proc_func = [](HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam) -> HRESULT
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
				sWindow_0->close();
				sWindow_1->close();

				logger.write(L"The windows just closed");
			}
		}
		break;
		}

		//check the message procedure of first scene 
		auto _hr = sScene_0->on_msg_proc(pHWND, pMsg, pWParam, pLParam);
		if (_hr == S_OK) return _hr;
		
		//now check the message procedure of second scene
		return sScene_1->on_msg_proc(pHWND, pMsg, pWParam, pLParam);
	};

	//Initialize and run windows on seperated threads
	tbb::parallel_invoke([&_monitors_info]()
	{
		int _screen_pos_x = 0, _screen_pos_y = 0;

		//set screen position based on information of monitor(s)
		if (_monitors_info.monitors.size() > 0)
		{
			_screen_pos_x = _monitors_info.monitors.at(0).left;
			_screen_pos_y = _monitors_info.monitors.at(0).top;
		}

		//Create an instance of window
		sWindow_0 = make_unique<w_window>();
		sWindow_0->set_width(800);
		sWindow_0->set_height(600);
		sWindow_0->set_position(_screen_pos_x, _screen_pos_y);
		sWindow_0->initialize();

		sWindowsInfo_0[0] =
		{
			{ sWindow_0->get_HWND(), sWindow_0->get_width(), sWindow_0->get_height() },
		};

		//run the main loop of window_1
		std::function<void(void)> _f = std::bind(&loop_window_0);
		sWindow_0->run(_f);
	},
		[&_monitors_info]()
	{
		int _screen_pos_x = 805, _screen_pos_y = 0;

		//set screen position based on information of monitor(s)
		if (_monitors_info.monitors.size() > 1)
		{
			_screen_pos_x = _monitors_info.monitors.at(1).left;
			_screen_pos_y = _monitors_info.monitors.at(1).top;
		}

		//Create an instance of window 
		sWindow_1 = make_unique<w_window>();
		sWindow_1->set_width(640);
		sWindow_1->set_height(480);
		sWindow_1->set_position(_screen_pos_x, _screen_pos_y);
		sWindow_1->initialize();

		sWindowsInfo_1[0] =
		{
			{ sWindow_1->get_HWND(), sWindow_1->get_width(), sWindow_1->get_height() },
		};

		//run the main loop of window_1
		std::function<void(void)> _f = std::bind(&loop_window_1);
		sWindow_1->run(_f);
	});
		
	//release
	release();

	//output a message to the log file
	logger.write(L"Shutting down Wolf");

	//exit
	return EXIT_SUCCESS;
}