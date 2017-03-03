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
	/*
		No need following code, the w_game class construction will be create an instance of logger 
		
		//initialize logger, and log in to the output debug window of visual studio and Log folder beside the ".exe".
		logger.initialize(L"Multiple_Windows_DX11_1.Win32");
	
		//log to output file
		logger.write(L"Starting Wolf");
	*/

	/*
		set message procedure function fo windows
		we use it for handling messages from window, such as keyboard status, on creating window event or etc.
	*/
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
        std::map<int, std::vector<w_window_info>> _windows_info = 
        { 
            { 
                0, { { 
#ifdef __WIN32
                    sWindow->get_HWND(), 
                    sWindow->get_HINSTANCE(), 
#elif defined(__linux)
                    sWindow->get_xcb_connection(), 
                    sWindow->get_xcb_window(),        
#endif
                    sWindow->get_width(), 
                    sWindow->get_height() 
                   } } 
            } 
        };
        
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