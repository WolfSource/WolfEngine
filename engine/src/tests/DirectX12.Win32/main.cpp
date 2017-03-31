#include "pch.h"
#include "scene.h"

using namespace std;

static std::unique_ptr<w_window> sWindow;
static std::unique_ptr<scene> sScene;

static void release()
{
	UNIQUE_RELEASE(sScene);
	UNIQUE_RELEASE(sWindow);
}

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	//first add msg_proc
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
				sWindow->close();
				logger.write(L"The window just closed");
			}
		}
		break;
		}

		return sScene->on_msg_proc(pHWND, pMsg, pWParam, pLParam);
	};

	//Initialize scene & window
	sScene = make_unique<scene>(wolf::system::io::get_current_directoryW());
	sWindow = make_unique<w_window>();
	sWindow->set_width(1920);
	sWindow->set_height(1080);
	sWindow->set_position(10, 10);
	//iniatilize window
	sWindow->initialize(_msg_proc_func);

	//run the main loop
	w_window_info _w_info;
	_w_info.hwnd = sWindow->get_HWND();
	_w_info.hInstance = sWindow->get_HINSTANCE();
	_w_info.width = sWindow->get_width();
	_w_info.height = sWindow->get_height();
	_w_info.v_sync_enable = false;
	_w_info.swap_chain_format = 87;

	map<int, vector<w_window_info>> _windowsInfos = { { 0, { _w_info } } };

	std::function<void(void)> _run_func = [&_windowsInfos]()->void
	{
		sScene->run(_windowsInfos);
	};
	
	sWindow->run(_run_func);

	//release all
	release();

	return EXIT_SUCCESS;
}