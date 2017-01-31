#include "w_system_pch.h"
#include "w_window.h"
#include <map>

static std::map<std::wstring, std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)>> sMsgsProcFunctions;

w_window::w_window() :
	_full_screen(false), 
	_screen_width(800), 
	_screen_height(600), 
	_screen_posX(-1), 
	_screen_posY(-1),
	_close(false),
	_hInstance(NULL),
	_hwnd(NULL)
{
	set_fixed_time_step(true);
}

w_window::~w_window()
{
	release();
}

static LRESULT CALLBACK MsgProc(HWND pHwnd, UINT pMessage, WPARAM pWParam, LPARAM pLParam)
{
	TCHAR _class_name[MAX_PATH];
	GetClassName(pHwnd, _class_name, MAX_PATH);
	
	std::wstring _w_class_name(_class_name);
	auto _message_proc_func = sMsgsProcFunctions.find(_w_class_name);
	if (_message_proc_func != sMsgsProcFunctions.end())
	{
		if (_message_proc_func->second != nullptr && _message_proc_func->second(pHwnd, pMessage, pWParam, pLParam) == S_OK) return S_OK;
	}
	return DefWindowProc(pHwnd, pMessage, pWParam, pLParam);
}

void w_window::initialize(std::function<HRESULT(HWND, UINT, WPARAM, LPARAM)> pMsgProcFunction)
{
	//Unregister all
	this->_hInstance = NULL;
	this->_hwnd = NULL;
	this->_hdc = NULL;
	if (this->_class_name.empty())
	{
		this->_class_name = L"Wolf Engine";
	}
	UnregisterClass(this->_class_name.c_str(), NULL);

	// Get the instance of this application.
	this->_hInstance = GetModuleHandle(NULL);

	sMsgsProcFunctions[this->_class_name] = pMsgProcFunction;

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS; //CS_DBLCLKS for double click
	wc.lpfnWndProc = MsgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = this->_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->_class_name.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Setup the screen
	DEVMODE dev_mode;

	if (this->_full_screen)
	{
		// Determine the resolution of the clients desktop screen and create full screen window
		std::memset(&dev_mode, 0, sizeof(dev_mode));

		dev_mode.dmSize = sizeof(dev_mode);
		dev_mode.dmPelsWidth = static_cast<unsigned long>(GetSystemMetrics(SM_CXSCREEN));
		dev_mode.dmPelsHeight = static_cast<unsigned long>(GetSystemMetrics(SM_CYSCREEN));
		dev_mode.dmBitsPerPel = 32;
		dev_mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Setup full screen mode
		ChangeDisplaySettings(&dev_mode, CDS_FULLSCREEN);
	}
	else
	{
		//Middle screen
		if (this->_screen_posX == -1 || this->_screen_posY == -1)
		{
			this->_screen_posX = (GetSystemMetrics(SM_CXSCREEN) - this->_screen_width) / 2;
			this->_screen_posY = (GetSystemMetrics(SM_CYSCREEN) - this->_screen_height) / 2;
		}
	}

	// Create the window with the screen settings and get the handle to it.
	this->_hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		this->_class_name.c_str(),
		this->_class_name.c_str(),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		this->_screen_posX,
		this->_screen_posY,
		this->_screen_width,
		this->_screen_height,
		NULL,
		NULL,
		this->_hInstance,
		NULL);
	if (!this->_hwnd)
	{
		V(S_FALSE, L"creating window handle", this->name, 3, true, false);
		return;
	}
	
	this->_hdc = GetDC(this->_hwnd);

	// Show window up on the screen
	ShowWindow(this->_hwnd, SW_SHOW);
	SetForegroundWindow(this->_hwnd);
	SetFocus(this->_hwnd);

	// Hide/Show the mouse cursor.
	ShowCursor(true);

}

void w_window::run(std::function<void(void)>& const pFunc)
{
	MSG msg;
	std::memset(&msg, 0, sizeof(msg));

	while (true)
	{
		if (this->_close)
		{
			break;
		}

		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_CLOSE:
				DestroyWindow(msg.hwnd);
				break;
			case WM_MOVE:
			{
				RECT _rect;

				GetClientRect(msg.hwnd, &_rect);
				MapWindowPoints(msg.hwnd, GetParent(msg.hwnd), (LPPOINT) &_rect, 2);

				//get width and height
				_rect.left -= 2;
				_rect.top -= 2;
				_rect.right += 2;
				_rect.bottom += 2;

				auto _width = _rect.right - _rect.left;
				auto _height = _rect.bottom - _rect.top;

				MoveWindow(msg.hwnd,
					msg.wParam,
					msg.lParam,
					_width,
					_height,
					TRUE);
			}
			break;
			case WM_SHOWWINDOW:
			{
				ShowWindow(msg.hwnd, msg.wParam);
				break;
			}
			case WM_ENABLE:
			{
				EnableWindow(msg.hwnd, msg.wParam);
				break;
			}
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			pFunc();
		}
	}
}

void w_window::close()
{
	this->_close = true;
}

ULONG w_window::release()
{
	if (is_released()) return 0;
	
	this->_hInstance = NULL;
	this->_hwnd = NULL;
	this->_hdc = NULL;

	UnregisterClass(this->_class_name.c_str(), NULL);
	this->_class_name.clear();

	return w_object::release();
}

#pragma region Setters

void w_window::set_class_name(_In_ LPWSTR pValue)
{
	this->_class_name = pValue;
}

void w_window::set_full_screen(bool pValue)
{
	this->_full_screen = pValue;
}

void w_window::set_fixed_time_step(bool pValue)
{
	this->_game_time.set_fixed_time_step(pValue);
}

void w_window::set_width(const int pValue)
{
	this->_screen_width = pValue;
}

void w_window::set_height(const int pValue)
{
	this->_screen_height = pValue;
}

void w_window::set_position(const int pX, const int pY)
{
	this->_screen_posX = pX;
	this->_screen_posY = pY;
}

#pragma endregion

#pragma region Getters

LPCWSTR w_window::get_class_name() const
{
	return this->_class_name.c_str();
}

HDC w_window::get_HDC() const
{
	return this->_hdc;
}

HINSTANCE w_window::get_HINSTANCE() const
{
	return this->_hInstance;
}

HWND w_window::get_HWND() const
{
	return this->_hwnd;
}

UINT w_window::get_width() const
{
	return this->_screen_width;
}

UINT w_window::get_height() const
{
	return this->_screen_height;
}

bool w_window::get_fixed_time_step() const
{
	return this->_game_time.get_fixed_timeStep();
}

bool w_window::get_full_screen() const
{
	return this->_full_screen;
}

#pragma endregion