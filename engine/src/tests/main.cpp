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
    wolf::release_shared_data_over_all_instances();
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

        auto _result = wolf::inputs_manager.update(pHWND, pMsg, pWParam, pLParam);
        if (_result) return _result;

		return (HRESULT)DefWindowProc(pHWND, pMsg, pWParam, pLParam);
	};

	//Initialize window 720p
	sWindow = make_unique<w_window>();
    sWindow->set_id(0);
    sWindow->set_width(1280);
	sWindow->set_height(720);
    //sWindow->set_position();
	sWindow->initialize(_msg_proc_func);

	//run the vulkan sample
	w_window_info _window_info;
	_window_info.width = sWindow->get_width();
	_window_info.height = sWindow->get_height();
	_window_info.hwnd = sWindow->get_HWND();
	_window_info.hInstance = sWindow->get_HINSTANCE();
    _window_info.v_sync = false;
	_window_info.swap_chain_format = 37;//VK_FORMAT_R8G8B8A8_UNORM
    _window_info.cpu_access_swap_chain_buffer = true;

	//call init_window from objective-c and get the pointer to the window
	std::map<int, w_window_info> _windows_info;
	_windows_info.insert({ 0, _window_info });

	//Initialize and content path and logPath
    auto _running_dir = wolf::system::io::get_current_directory();
	std::wstring _content_path;
	#if defined(__WIN32) || defined(__UWP)
		_content_path = _running_dir + L"../../../../content/";
	#elif defined(__APPLE__)
	_content_path = _running_dir + L"/../../../../../content/";
	#elif defined(__linux)
		error
	#elif defined(__ANDROID)
		error
	#endif

	sScene = make_unique<scene>(_content_path, _running_dir,  L"wolf.engine.vulkan.test");
	sWindow->run([&_windows_info]()->void
    {
        sScene->run(_windows_info);
    });
	//release all
	release();

	return EXIT_SUCCESS;
}
