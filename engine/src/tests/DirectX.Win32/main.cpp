#include "pch.h"
#include "scene.h"
#include <w_tcp_client.h>

#define IDC_MAIN_EDIT	102			// Edit box identifier

using namespace std;

static std::unique_ptr<w_window> sWindow;
static std::unique_ptr<scene> sScene;

static void release()
{
	UNIQUE_RELEASE(sWindow);
	UNIQUE_RELEASE(sScene);
}

//Entry point of program 
int WINAPI WinMain(HINSTANCE pHInstance, HINSTANCE pPrevHInstance, PSTR pSTR, int pCmdshow)
{
	//Initialize scene & window
	sScene = make_unique<scene>();
	sWindow = make_unique<w_window>();

	//first add msg_proc
	w_window::msg_proc_func = [](HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam) -> HRESULT
	{
		switch (pMsg)
		{
		case WM_CREATE:
		{
			// Create an edit box
			auto hEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
				L"EDIT",
				L"",
				WS_CHILD | WS_VISIBLE |
				ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
				50,
				100,
				200,
				100,
				pHWND,
				(HMENU) IDC_MAIN_EDIT,
				GetModuleHandle(NULL),
				NULL);
			HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hEdit,
				WM_SETFONT,
				(WPARAM) hfDefault,
				MAKELPARAM(FALSE, 0));
			SendMessage(hEdit,
				WM_SETTEXT,
				(WPARAM)hfDefault,
				(LPARAM)L"Insert Text");
		}
		break;
		}

		return sScene->on_msg_proc(pHWND, pMsg, pWParam, pLParam);
	};

	//iniatilize window
	sWindow->initialize();

	//run the main loop
	map<int, vector<w_window_info>> _windowsInfo = { { 0, { { sWindow->get_HWND(), sWindow->get_width(), sWindow->get_height() } } } };
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