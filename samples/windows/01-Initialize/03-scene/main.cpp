/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create a scene in Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"
#include "scene.h"

using namespace std;

static std::unique_ptr<w_window> sWindow;
static std::unique_ptr<scene> sScene;
static std::map<int, std::vector<w_window_info>> sWindowsInfo;

static void release()
{
	UNIQUE_RELEASE(sWindow);
	UNIQUE_RELEASE(sScene);
	sWindowsInfo.clear();
}

void loop_window()
{
	sScene->run(sWindowsInfo);
}

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	/*
		No need following code, the w_game class construction will be create an instance of logger 
		
		//initialize logger, and log in to the output debug window of visual studio and Log folder beside the ".exe".
		logger.initialize(L"Multiple_Windows_DX11_1.Win32");
	
		//log to output file
		logger.write(L"Starting Wolf");
	*/

	/*
		set message procedure function fo windows
		we use it for handling messages from window, such as keyboard status, on createing window event or etc.
	*/
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

	//Initialize scene & window
	sScene = make_unique<scene>();
	sWindow = make_unique<w_window>();
	sWindow->initialize(_msg_proc_func);

	//create window info and add it to the map
	sWindowsInfo[0] = { { sWindow->get_HWND(), sWindow->get_HINSTANCE(), sWindow->get_width(), sWindow->get_height() } };

	//run the main loop of window
	std::function<void(void)> _f = std::bind(&loop_window);
	sWindow->run(_f);

	//release all
	release();

	//output a message to the log file
	logger.write(L"Shutting down Wolf");

	//exit
	return EXIT_SUCCESS;
}