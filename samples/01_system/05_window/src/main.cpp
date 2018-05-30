/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to create a window in Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace std;
using namespace wolf;

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
WOLF_MAIN()
{
	logger.initialize(L"01_system-05_window", wolf::system::io::get_current_directoryW());

	//log to output file
	logger.write(L"Wolf initialized");

#ifdef __WIN32
	/*
		set message procedure function
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
					logger.write(L"The window just closed");
				}
			}
			break;
		}

		//make sure return false if you want to leave handle of DefWindowProc to the Wolf Engine
		return S_FALSE;
	};
#endif

	//Create an instance of window and then initialize it
	sWindow = make_unique<w_window>();
        
#ifdef __WIN32
	sWindow->set_class_name(L"WOLF_WINDOW_CLS");
    sWindow->initialize(_msg_proc_func);
#else
    sWindow->initialize();
#endif       

	//run the main loop
	std::function<void(void)> _f = std::bind(&loop);
	sWindow->run(_f);

	//release all
	release();

	return EXIT_SUCCESS;
}