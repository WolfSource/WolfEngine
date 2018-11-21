#include "pch.h"
#include <w_window.h>
#include "scene.h"

using namespace std;

static std::unique_ptr<w_window> sWindow;
static std::unique_ptr<w_window> sChildWindow;
static std::unique_ptr<scene> sScene;

static void release()
{
	//UNIQUE_RELEASE(sScene);
	UNIQUE_RELEASE(sWindow);
    wolf::release_heap_data();
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
	sWindow->enable_caption(true);
	sWindow->enable_dialog_frame(true);
	sWindow->enable_system_menu(true);
    sWindow->set_width(1280);
	sWindow->set_height(720);
    //sWindow->set_position();
	sWindow->initialize(_msg_proc_func);


	sChildWindow = make_unique<w_window>();
	sChildWindow->set_id(1);
	sChildWindow->enable_caption(true);
	sChildWindow->enable_dialog_frame(true);
	sChildWindow->enable_system_menu(true);
	sChildWindow->set_width(640);
	sChildWindow->set_height(480);
	sChildWindow->set_parent(sWindow->get_HWND());
	sChildWindow->initialize(_msg_proc_func);

	//run the vulkan sample
	w_present_info _window_info;
	_window_info.width = sWindow->get_width();
	_window_info.height = sWindow->get_height();
	_window_info.hwnd = sWindow->get_HWND();
	_window_info.hInstance = sWindow->get_HINSTANCE();
    _window_info.v_sync = false;
	_window_info.swap_chain_format = 37;//VK_FORMAT_R8G8B8A8_UNORM
    _window_info.cpu_access_swap_chain_buffer = true;

	//call init_window from objective-c and get the pointer to the window
	std::map<int, w_present_info> _windows_info;
	_windows_info.insert({ 0, _window_info });

	//Initialize and content path and logPath
    auto _running_dir = wolf::system::io::get_current_directoryW();
	std::wstring _content_path = _running_dir + L"../../../../content/";

	wolf::system::w_logger_config _log_config;
	_log_config.app_name = L"wolf.engine.vulkan.test";
	_log_config.log_path = _running_dir;
	_log_config.flush_level = false;
	_log_config.log_to_std_out = true;

	//sScene = make_unique<scene>(_content_path, _log_config);
	sWindow->run([&_windows_info]()->void
    {
        //sScene->run(_windows_info);
    });
	//release all
	release();

	return EXIT_SUCCESS;
}
