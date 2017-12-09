#include "pch.h"
#include <w_window.h>
#include "scene.h"

using namespace std;

static std::unique_ptr<w_window> sWindow;
static std::unique_ptr<scene> sScene;

static void release()
{
	//UNIQUE_RELEASE(sScene);
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
            }
		}
		break;
		}

        if (sScene)
        {
            auto _result = sScene->on_msg_proc(pHWND, pMsg, pWParam, pLParam);
            if (_result) return _result;
        }
		return DefWindowProc(pHWND, pMsg, pWParam, pLParam);
	};

	//Initialize window 720p
	sWindow = make_unique<w_window>();
	sWindow->set_width(1280);
	sWindow->set_height(720);
	sWindow->initialize(_msg_proc_func);

	//run the vulkan sample
	w_window_info _window_info;
	_window_info.width = sWindow->get_width();
	_window_info.height = sWindow->get_height();
	_window_info.hwnd = sWindow->get_HWND();
	_window_info.hInstance = sWindow->get_HINSTANCE();
    _window_info.v_sync_enable = false;

	//call init_window from objective-c and get the pointer to the window
	std::map<int, std::vector<w_window_info>> _windows_info;
	_windows_info.insert({ 0,{ _window_info } });

	//Initialize and run scene
    auto _running_dir = wolf::system::io::get_current_directoryW();
	sScene = make_unique<scene>(_running_dir, L"wolf.engine.vulkan.test");
	std::function<void(void)> _run_func = [&_windows_info]()->void
	{
		sScene->run(_windows_info);
	};

	sWindow->run(_run_func);

	//release all
	release();

	return EXIT_SUCCESS;
}
