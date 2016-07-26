#include "pch.h"
#include <w_window.h>

using namespace std;

static std::unique_ptr<w_window> window;

void release()
{
	UNIQUE_RELEASE(window);
}

void loop()
{
	/* TODO: Main loop, write your code here  */
	logger.write(L"Wolf is running");
}

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	logger.write(L"Starting Wolf");

	//Initialize scene & window
	window = make_unique<w_window>();
	window->initialize();

	w_window::msg_proc_func = [](HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam) -> HRESULT
	{
		switch (pMsg)
		{
			case WM_CREATE:
			{
				logger.write(L"The window just created");
			}
			break;
			case WM_KEYUP:
			{
				if (pWParam == VK_ESCAPE)
				{
					window->close();
					logger.write(L"The window just closed");
				}
			}
			break;
		}

		//make sure return false if you want to leave handle of DefWindowProc to the Wolf
		return S_FALSE;
	};

	//run the main loop
	std::function<void(void)> _loop_function = std::bind(&loop);
	window->run(_loop_function);
	
	//release all
	release();

	logger.write(L"Shutting down Wolf");

	return EXIT_SUCCESS;
}