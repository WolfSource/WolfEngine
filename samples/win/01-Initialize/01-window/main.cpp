/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : pch.h
	Description		 : This sample shows how to create a window in Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"
#include <w_window.h>

//namespaces
using namespace std;

//global variables
static std::unique_ptr<w_window> sWindow;

void release()
{
	//release unique pointer of window
	UNIQUE_RELEASE(sWindow);
	logger.release();
}

void loop()
{
	/* TODO: This is the main loop, write your code here  */
	logger.write(L"Wolf is running");
}

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	//initialize logger, and log in to the output debug window of visual studio and Log folder beside the ".exe".
	logger.initialize(L"01_Window_DX11_1.Win32");

	//log to output file
	logger.write(L"Starting Wolf");

	//Create an instance of window and then initialize it
	sWindow = make_unique<w_window>();
	sWindow->initialize();

	/*
		set message procedure function
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
					sWindow->close();
					logger.write(L"The window just closed");
				}
			}
			break;
		}

		//make sure return false if you want to leave handle of DefWindowProc to the Wolf Engine
		return S_FALSE;
	};

	//run the main loop
	std::function<void(void)> _f = std::bind(&loop);
	sWindow->run(_f);
	
	//release all
	release();

	//output a message to the log file
	logger.write(L"Shutting down Wolf");

	//exit
	return EXIT_SUCCESS;
}