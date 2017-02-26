#include "pch.h"
#include "scene.h"
#include <w_tcp_client.h>

#define W_GUI_ID_LIST_WIDGET_TEXT_EDIT		0xF0000000

using namespace std;

static std::unique_ptr<w_window> sWindow;
static std::unique_ptr<scene> sScene;

static void release()
{
	//release all textbox
	for (size_t i = 0; i < sTextBoxesHandles.size(); i++)
	{
		auto _hwnd = std::get<0>(sTextBoxesHandles[i]);
		DestroyWindow(_hwnd);
		_hwnd = NULL;
	}

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
			for (size_t i = 0; i < 50; i++)
			{
				auto _x = 200 + i;
				auto _y = 5;

				auto _hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
					L"EDIT",
					L"",
					WS_CHILD | WS_VISIBLE |
					ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
					_x, _y,
					100, 25,
					pHWND,
					(HMENU) W_GUI_ID_LIST_WIDGET_TEXT_EDIT,
					GetModuleHandle(NULL),
					NULL);
				
				ShowWindow(_hwnd, FALSE);
				EnableWindow(_hwnd, FALSE);

				sTextBoxesHandles.push_back(std::make_tuple(_hwnd, _x, _y));
			}
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
	sScene = make_unique<scene>();
	sWindow = make_unique<w_window>();
	sWindow->set_width(1920);
	sWindow->set_height(1080);
	//iniatilize window
	sWindow->initialize(_msg_proc_func);

	//run the main loop
	map<int, vector<w_window_info>> _windowsInfo = { 
		{ 
			0, 
			{ 
				{ sWindow->get_HWND(), sWindow->get_HINSTANCE(), sWindow->get_width(), sWindow->get_height() }
			} 
		} 
	};
	std::function<void(void)> _run_func = [&_windowsInfo]()->void
	{
		service.run();
		sScene->run(_windowsInfo);
	};
	
	sWindow->run(_run_func);

	//release all
	release();

	return EXIT_SUCCESS;
}